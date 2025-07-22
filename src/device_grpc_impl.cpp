// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "device_grpc_impl.hpp"

#include <astarteplatform/msghub/astarte_data.pb.h>
#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/interface.pb.h>
#include <astarteplatform/msghub/message_hub_error.pb.h>
#include <astarteplatform/msghub/message_hub_service.grpc.pb.h>
#include <astarteplatform/msghub/node.pb.h>
#include <astarteplatform/msghub/property.pb.h>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <grpcpp/support/client_interceptor.h>
#include <grpcpp/support/status.h>
#include <spdlog/spdlog.h>

#include <atomic>
#include <chrono>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device_grpc.hpp"
#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "exponential_backoff.hpp"
#include "grpc_converter.hpp"
#include "grpc_interceptors.hpp"
#include "shared_queue.hpp"

namespace AstarteDeviceSdk {

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using grpc::experimental::ClientInterceptorFactoryInterface;

using gRPCAstarteData = astarteplatform::msghub::AstarteData;
using gRPCAstarteDatastreamObject = astarteplatform::msghub::AstarteDatastreamObject;
using gRPCAstartePropertyIndividual = astarteplatform::msghub::AstartePropertyIndividual;
using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;
using gRPCMessageHub = astarteplatform::msghub::MessageHub;
using gRPCMessageHubError = astarteplatform::msghub::MessageHubError;
using gRPCMessageHubEvent = astarteplatform::msghub::MessageHubEvent;
using gRPCNode = astarteplatform::msghub::Node;
using gRPCInterfaceName = astarteplatform::msghub::InterfaceName;
using gRPCStoredProperties = astarteplatform::msghub::StoredProperties;
using gRPCOwnership = astarteplatform::msghub::Ownership;
using gRPCPropertyFilter = astarteplatform::msghub::PropertyFilter;
using gRPCPropertyIdentifier = astarteplatform::msghub::PropertyIdentifier;

AstarteDeviceGRPC::AstarteDeviceGRPCImpl::AstarteDeviceGRPCImpl(std::string server_addr,
                                                                std::string node_uuid)
    : server_addr_(std::move(server_addr)),
      node_uuid_(std::move(node_uuid)),
      connection_stop_flag_(std::atomic_bool(false)),
      grpc_stream_error_(false) {}

AstarteDeviceGRPC::AstarteDeviceGRPCImpl::~AstarteDeviceGRPCImpl() { disconnect(); }

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::add_interface_from_json(
    const std::filesystem::path &json_file) {
  spdlog::debug("Adding interface from file: {}", json_file.string());
  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    spdlog::error("Could not open the interface file: {}", json_file.string());
    throw AstarteFileOpenException(json_file.string());
  }
  // Read the entire JSON file content into a string
  const std::string interface_json((std::istreambuf_iterator<char>(interface_file)),
                                   std::istreambuf_iterator<char>());
  interfaces_bins_.push_back(interface_json);
  spdlog::trace("Added interface: \n{}", interface_json);
  interface_file.close();
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::add_interface_from_str(std::string_view json) {
  spdlog::debug("Adding interface");
  interfaces_bins_.emplace_back(json);
  spdlog::trace("Added interface: \n{}", json);
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::connect() {
  spdlog::info("Connection requested.");
  if (connection_thread_.joinable()) {
    spdlog::warn("Connection process is already running.");
    return;
  }
  connection_thread_ = std::thread([this]() { this->connection_loop(); });
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::is_connected(
    const std::chrono::milliseconds &timeout) const -> bool {
  // The event handler is started when a connection attempt is performed.
  // If a connection cannot be established it will exit quickly.
  if (!event_handler_.joinable()) {
    return false;
  }
  std::this_thread::sleep_for(timeout);
  return event_handler_.joinable();
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::disconnect() {
  spdlog::info("Disconnection requested.");

  // If the device is still attempting to connect request a stop.
  if (connection_thread_.joinable()) {
    connection_stop_flag_.store(true);
  }

  // If the device is connected (an event_handler_ exists) call the detach function.
  // Additionally if the stream error flag is set at this point it means the gRPC stream has seen
  // an error but no Detach has been called since. For consistency we always call a detach in this
  // situation. This might not always be the desired behaviour and might change in the future.
  if (event_handler_.joinable() || grpc_stream_error_.load()) {
    ClientContext context;
    google::protobuf::Empty response;
    const Status status = stub_->Detach(&context, google::protobuf::Empty(), &response);
    if (!status.ok()) {
      spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    }
    // Reset the stream error flag
    grpc_stream_error_.store(false);
  }

  // Wait for the connection thread to stop.
  if (connection_thread_.joinable()) {
    connection_thread_.join();
  }

  // Reset the stop flag for a future reconnection
  connection_stop_flag_.store(false);
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::send_individual(
    std::string_view interface_name, std::string_view path, const AstarteData &data,
    const std::chrono::system_clock::time_point *timestamp) {
  spdlog::debug("Sending individual: {} {}", interface_name, path);
  if (!event_handler_.joinable()) {
    const std::string msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstarteDatastreamIndividual> grpc_datastream_individual =
      converter(data, timestamp);
  message.set_allocated_datastream_individual(grpc_datastream_individual.release());

  ClientContext context;
  google::protobuf::Empty response;
  spdlog::trace("Sending data: {} {}", interface_name, path);
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::send_object(
    std::string_view interface_name, std::string_view path, const AstarteDatastreamObject &object,
    const std::chrono::system_clock::time_point *timestamp) {
  spdlog::debug("Sending object: {} {}", interface_name, path);
  if (!event_handler_.joinable()) {
    const std::string msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstarteDatastreamObject> grpc_datastream_object =
      converter(object, timestamp);
  message.set_allocated_datastream_object(grpc_datastream_object.release());

  ClientContext context;
  google::protobuf::Empty response;
  spdlog::trace("Sending data: {} {}", interface_name, path);
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::set_property(std::string_view interface_name,
                                                            std::string_view path,
                                                            const AstarteData &data) {
  spdlog::debug("Setting property: {} {}", interface_name, path);
  if (!event_handler_.joinable()) {
    const std::string msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  const std::optional<AstarteData> &opt_data = data;
  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstartePropertyIndividual> grpc_property_individual = converter(opt_data);
  message.set_allocated_property_individual(grpc_property_individual.release());

  ClientContext context;
  google::protobuf::Empty response;
  spdlog::trace("Sending data: {} {}", interface_name, path);
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::unset_property(std::string_view interface_name,
                                                              std::string_view path) {
  spdlog::debug("Unsetting property: {} {}", interface_name, path);
  if (!event_handler_.joinable()) {
    const std::string msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  const std::optional<AstarteData> opt_data = std::nullopt;
  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstartePropertyIndividual> grpc_property_individual = converter(opt_data);
  message.set_allocated_property_individual(grpc_property_individual.release());

  ClientContext context;
  google::protobuf::Empty response;
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::poll_incoming(
    const std::chrono::milliseconds &timeout) -> std::optional<AstarteMessage> {
  return rcv_queue_.pop(timeout);
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::get_all_properties(
    const std::optional<AstarteOwnership> &ownership) -> std::list<AstarteStoredProperty> {
  if (ownership.has_value()) {
    spdlog::debug("Getting all stored properties {} owned.", ownership_as_str(ownership.value()));
  } else {
    spdlog::debug("Getting all stored properties for all owners.");
  }

  if (!event_handler_.joinable()) {
    const std::string msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }

  gRPCPropertyFilter filter;
  if (ownership.has_value()) {
    filter.set_ownership((ownership == AstarteOwnership::kDevice) ? gRPCOwnership::DEVICE
                                                                  : gRPCOwnership::SERVER);
  }

  ClientContext context;
  gRPCStoredProperties response;
  const Status status = stub_->GetAllProperties(&context, filter, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }

  return GrpcConverterFrom{}(response);
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::get_properties(std::string_view interface_name)
    -> std::list<AstarteStoredProperty> {
  spdlog::debug("Getting stored properties for interface: {}", interface_name);
  if (!event_handler_.joinable()) {
    const std::string msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }

  gRPCInterfaceName grpc_interface_name;
  grpc_interface_name.set_name(interface_name);

  ClientContext context;
  gRPCStoredProperties response;
  const Status status = stub_->GetProperties(&context, grpc_interface_name, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }

  return GrpcConverterFrom{}(response);
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::get_property(std::string_view interface_name,
                                                            std::string_view path)
    -> AstartePropertyIndividual {
  spdlog::debug("Getting stored property for interface '{}' and path '{}'", interface_name, path);
  if (!event_handler_.joinable()) {
    const std::string msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    throw AstarteOperationRefusedException(msg);
  }

  gRPCPropertyIdentifier identifier;
  identifier.set_interface_name(interface_name);
  identifier.set_path(path);

  ClientContext context;
  gRPCAstartePropertyIndividual response;
  const Status status = stub_->GetProperty(&context, identifier, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }

  return GrpcConverterFrom{}(response);
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::connection_attempt() {
  if (event_handler_.joinable()) {
    spdlog::warn("Device is already connected.");
    return;
  }
  spdlog::debug("Attempting to connect to the message hub at {}", server_addr_);

  // Create a new channel and initialize the gRPC stub
  const grpc::ChannelArguments args;
  std::vector<std::unique_ptr<ClientInterceptorFactoryInterface>> interceptor_creators;
  interceptor_creators.push_back(std::make_unique<NodeIdInterceptorFactory>(node_uuid_));

  const std::shared_ptr<Channel> channel = CreateCustomChannelWithInterceptors(
      server_addr_, grpc::InsecureChannelCredentials(), args, std::move(interceptor_creators));

  stub_ = gRPCMessageHub::NewStub(channel);

  // Create the node message for the attach RPC.
  gRPCNode node;
  for (const std::string &interface_json : interfaces_bins_) {
    node.add_interfaces_json(interface_json);
  }

  // Generate a new client context for the Attach method.
  // From the documentation it appears that a context needs to be valid for the duration of the RPC
  // In this case the RPC ends when the return stream is closed, so the context should survive at
  // least for that long.
  // See: https://grpc.github.io/grpc/cpp/classgrpc_1_1_client_context.html
  std::unique_ptr<ClientContext> client_context = std::make_unique<ClientContext>();

  // Call the attach RPC.
  std::unique_ptr<ClientReader<gRPCMessageHubEvent>> reader =
      stub_->Attach(client_context.get(), node);

  // Start a thread for the event stream.
  event_handler_ = std::thread(&AstarteDeviceGRPCImpl::handle_events, this,
                               std::move(client_context), std::move(reader));

  // Reset the stream error flag
  grpc_stream_error_.store(false);

  // Wait for the event stream to finish.
  // N.B. This makes this function blocking!
  if (event_handler_.joinable()) {
    event_handler_.join();
  }
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::handle_events(
    std::unique_ptr<grpc::ClientContext> context,
    std::unique_ptr<ClientReader<gRPCMessageHubEvent>> reader) {
  (void)context;

  spdlog::debug("Event handler thread has been started");

  // Read the message stream.
  gRPCMessageHubEvent msghub_event;
  while (reader->Read(&msghub_event)) {
    spdlog::debug("Event from the message hub received.");
    std::optional<AstarteMessage> parsed_event =
        AstarteDeviceGRPCImpl::parse_message_hub_event(msghub_event);
    if (parsed_event.has_value()) {
      const AstarteMessage &parsed_message = parsed_event.value();
      this->rcv_queue_.push(parsed_message);
    }
  }
  spdlog::info("Message hub stream has been interrupted.");

  // Log an error if it the stream has been stopped due to a failure.
  const Status status = reader->Finish();
  if (!status.ok()) {
    grpc_stream_error_.store(true);
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
  }
}

auto AstarteDeviceGRPC::AstarteDeviceGRPCImpl::parse_message_hub_event(
    const gRPCMessageHubEvent &event) -> std::optional<AstarteMessage> {
  spdlog::trace("Parsing message hub event.");
  std::optional<AstarteMessage> res = std::nullopt;
  if (event.has_message()) {
    const gRPCAstarteMessage &astarteMessage = event.message();
    res = GrpcConverterFrom{}(astarteMessage);
  } else if (event.has_error()) {
    const gRPCMessageHubError &error = event.error();
    spdlog::error("Message hub error: {}", error.description());
    spdlog::error("Error source backtrace: ");
    for (const std::string &source : error.source()) {
      spdlog::error("  {}", source);
    }
  } else {
    spdlog::error("Unknown event type!");
  }
  return res;
}

void AstarteDeviceGRPC::AstarteDeviceGRPCImpl::connection_loop() {
  spdlog::trace("Connection loop started.");
  ExponentialBackoff backoff(std::chrono::seconds(2), std::chrono::minutes(1));

  // The loop now checks if a stop has been requested.
  while (!connection_stop_flag_.load()) {
    try {
      // This is a blocking function.
      // It will return only if the connection fails or the device is disconnected.
      connection_attempt();
    } catch (const std::exception &e) {
      spdlog::error("Connection failed: {}", e.what());
    }

    if (connection_stop_flag_.load()) {
      spdlog::info("Stop requested, will not attempt to reconnect.");
      break;
    }

    auto delay = backoff.getNextDelay();
    spdlog::info("Will attempt to reconnect in {} seconds.",
                 std::chrono::duration_cast<std::chrono::seconds>(delay).count());
    std::this_thread::sleep_for(delay);
  }

  spdlog::info("Connection loop has been terminated.");
}

}  // namespace AstarteDeviceSdk
