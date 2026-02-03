// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "grpc/device_grpc_impl.hpp"

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
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <optional>
#include <regex>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#if defined(ASTARTE_USE_TL_EXPECTED)
#include "tl/expected.hpp"
#else
#include <expected>
#endif

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/grpc/device_grpc.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "exponential_backoff.hpp"
#include "grpc/grpc_converter.hpp"
#include "grpc/grpc_interceptors.hpp"
#include "shared_queue.hpp"

namespace astarte::device {

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
using gRPCInterfacesJson = astarteplatform::msghub::InterfacesJson;
using gRPCInterfacesName = astarteplatform::msghub::InterfacesName;

AstarteDeviceGrpc::AstarteDeviceGrpcImpl::AstarteDeviceGrpcImpl(std::string server_addr,
                                                                std::string node_uuid)
    : server_addr_(std::move(server_addr)),
      node_uuid_(std::move(node_uuid)),
      connected_(std::atomic_bool(false)),
      grpc_stream_error_(std::atomic_bool(false)) {}

AstarteDeviceGrpc::AstarteDeviceGrpcImpl::~AstarteDeviceGrpcImpl() { ssource_.request_stop(); }

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::add_interface_from_file(
    const std::filesystem::path& json_file) -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Adding interface from file: {}", json_file.string());

  // Check file validity
  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    spdlog::error("Could not open the interface file: {}", json_file.string());
    return astarte_tl::unexpected(AstarteFileOpenError{json_file.string()});
  }

  // Read the entire JSON file content into a string
  const std::string interface_json((std::istreambuf_iterator<char>(interface_file)),
                                   std::istreambuf_iterator<char>());

  // Close the file
  interface_file.close();

  // Add the interface from the fetched string
  return add_interface_from_str(interface_json);
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::add_interface_from_str(std::string_view json)
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Adding interface from string");

  // If the device is connected, notify the message hub
  if (is_connected()) {
    gRPCInterfacesJson grpc_interfaces_json;
    grpc_interfaces_json.add_interfaces_json(json);
    ClientContext context;
    google::protobuf::Empty response;
    const Status status = stub_->AddInterfaces(&context, grpc_interfaces_json, &response);
    if (!status.ok()) {
      spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
      return astarte_tl::unexpected(AstarteGrpcLibError{
          static_cast<std::uint64_t>(status.error_code()), status.error_message()});
    }
  }

  interfaces_bins_.emplace_back(json);
  spdlog::trace("Added interface: \n{}", json);
  return {};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::remove_interface(const std::string& interface_name)
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Removing interface: {}", interface_name);
  const std::string escaped_interface_name =
      std::regex_replace(interface_name, std::regex("\\."), "\\.");
  const std::string pattern_string =
      R"(\"interface_name\":\s*\")" + escaped_interface_name + R"(\")";
  const std::regex pattern(pattern_string);

  for (auto i = interfaces_bins_.begin(); i != interfaces_bins_.end(); ++i) {
    const std::string& interface_json = *i;
    std::smatch match;
    if (std::regex_search(interface_json, match, pattern)) {
      if (is_connected()) {
        gRPCInterfacesName grpc_interface_names;
        grpc_interface_names.add_names(interface_name);
        ClientContext context;
        google::protobuf::Empty response;
        const Status status = stub_->RemoveInterfaces(&context, grpc_interface_names, &response);
        if (!status.ok()) {
          spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
          return astarte_tl::unexpected(AstarteGrpcLibError{
              static_cast<std::uint64_t>(status.error_code()), status.error_message()});
        }
      }
      interfaces_bins_.erase(i);
      break;
    }
  }
  return {};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::connect()
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::info("Connection requested.");
  if (connection_thread_) {
    spdlog::warn("Connection process is already running.");
    return astarte_tl::unexpected(
        AstarteOperationRefusedError{"Connection process is already in progress"});
  }

  // create a fresh stop source for this new connection session
  ssource_ = std::stop_source();

  // start the connection loop, passing it the token from our source.
  connection_thread_.emplace(
      [this](const std::stop_token& token) {
        auto res = this->connection_loop(token);
        // TODO(sorru94): Evaluate the use of futures to communicate errors with the user
        if (!res) {
          spdlog::error("Connection loop error.");
          spdlog::error(res.error());
        }
      },
      ssource_.get_token());
  return {};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::is_connected() const -> bool {
  return connected_.load();
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::disconnect()
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::info("Disconnection requested.");
  astarte_tl::expected<void, AstarteError> res = {};

  // request a stop to signal connection_loop and handle_events
  ssource_.request_stop();

  if (connected_.load() || grpc_stream_error_.load()) {
    ClientContext context;
    google::protobuf::Empty response;
    const Status status = stub_->Detach(&context, google::protobuf::Empty(), &response);
    if (!status.ok()) {
      spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
      res = astarte_tl::unexpected(AstarteGrpcLibError{
          static_cast<std::uint64_t>(status.error_code()), status.error_message()});
    }
    grpc_stream_error_.store(false);
  }

  // clear the thread object by invoking the destructor on the internal thread.
  // jthread's destructor will join
  connection_thread_.reset();
  return res;
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::send_individual(
    std::string_view interface_name, std::string_view path, const Data& data,
    const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Sending individual: {} {}", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    return astarte_tl::unexpected(AstarteOperationRefusedError{msg});
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
    return astarte_tl::unexpected(AstarteGrpcLibError{
        static_cast<std::uint64_t>(status.error_code()), status.error_message()});
  }
  return {};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::send_object(
    std::string_view interface_name, std::string_view path, const AstarteDatastreamObject& object,
    const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Sending object: {} {}", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    return astarte_tl::unexpected(AstarteOperationRefusedError{msg});
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
    return astarte_tl::unexpected(AstarteGrpcLibError{
        static_cast<std::uint64_t>(status.error_code()), status.error_message()});
  }
  return {};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::set_property(std::string_view interface_name,
                                                            std::string_view path,
                                                            const Data& data)
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Setting property: {} {}", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    return astarte_tl::unexpected(AstarteOperationRefusedError{msg});
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  const std::optional<Data>& opt_data = data;
  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstartePropertyIndividual> grpc_property_individual = converter(opt_data);
  message.set_allocated_property_individual(grpc_property_individual.release());

  ClientContext context;
  google::protobuf::Empty response;
  spdlog::trace("Sending data: {} {}", interface_name, path);
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    return astarte_tl::unexpected(AstarteGrpcLibError{
        static_cast<std::uint64_t>(status.error_code()), status.error_message()});
  }
  return {};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::unset_property(std::string_view interface_name,
                                                              std::string_view path)
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Unsetting property: {} {}", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    return astarte_tl::unexpected(AstarteOperationRefusedError{msg});
  }
  gRPCAstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);

  const std::optional<Data> opt_data = std::nullopt;
  GrpcConverterTo converter;
  std::unique_ptr<gRPCAstartePropertyIndividual> grpc_property_individual = converter(opt_data);
  message.set_allocated_property_individual(grpc_property_individual.release());

  ClientContext context;
  google::protobuf::Empty response;
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    return astarte_tl::unexpected(AstarteGrpcLibError{
        static_cast<std::uint64_t>(status.error_code()), status.error_message()});
  }
  return {};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::poll_incoming(
    const std::chrono::milliseconds& timeout) -> std::optional<AstarteMessage> {
  return rcv_queue_.pop(timeout);
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::get_all_properties(
    const std::optional<AstarteOwnership>& ownership)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> {
  if (ownership.has_value()) {
    spdlog::debug("Getting all stored properties {} owned.", ownership.value());
  } else {
    spdlog::debug("Getting all stored properties for all owners.");
  }

  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    return astarte_tl::unexpected(AstarteOperationRefusedError{msg});
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
    return astarte_tl::unexpected(AstarteGrpcLibError(
        static_cast<std::uint64_t>(status.error_code()), status.error_message()));
  }

  return GrpcConverterFrom{}(response);
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::get_properties(std::string_view interface_name)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> {
  spdlog::debug("Getting stored properties for interface: {}", interface_name);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    return astarte_tl::unexpected(AstarteOperationRefusedError{msg});
  }

  gRPCInterfaceName grpc_interface_name;
  grpc_interface_name.set_name(interface_name);

  ClientContext context;
  gRPCStoredProperties response;
  const Status status = stub_->GetProperties(&context, grpc_interface_name, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    return astarte_tl::unexpected(AstarteGrpcLibError(
        static_cast<std::uint64_t>(status.error_code()), status.error_message()));
  }

  return GrpcConverterFrom{}(response);
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::get_property(std::string_view interface_name,
                                                            std::string_view path)
    -> astarte_tl::expected<AstartePropertyIndividual, AstarteError> {
  spdlog::debug("Getting stored property for interface '{}' and path '{}'", interface_name, path);
  if (!connected_.load()) {
    const std::string_view msg("Device disconnected, operation aborted.");
    spdlog::warn(msg);
    return astarte_tl::unexpected(AstarteOperationRefusedError{msg});
  }

  gRPCPropertyIdentifier identifier;
  identifier.set_interface_name(interface_name);
  identifier.set_path(path);

  ClientContext context;
  gRPCAstartePropertyIndividual response;
  const Status status = stub_->GetProperty(&context, identifier, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    return astarte_tl::unexpected(AstarteGrpcLibError(
        static_cast<std::uint64_t>(status.error_code()), status.error_message()));
  }

  return GrpcConverterFrom{}(response);
}

// Private helper to set up the gRPC channel and stub
void AstarteDeviceGrpc::AstarteDeviceGrpcImpl::setup_grpc_channel() {
  const grpc::ChannelArguments args;
  std::vector<std::unique_ptr<ClientInterceptorFactoryInterface>> interceptor_creators;
  interceptor_creators.push_back(std::make_unique<NodeIdInterceptorFactory>(node_uuid_));

  const std::shared_ptr<Channel> channel = CreateCustomChannelWithInterceptors(
      server_addr_, grpc::InsecureChannelCredentials(), args, std::move(interceptor_creators));

  stub_ = gRPCMessageHub::NewStub(channel);
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::perform_attach()
    -> astarte_tl::expected<AttachResult, AstarteError> {
  // Create the node message for the attach RPC.
  gRPCNode node;
  for (const std::string& interface_json : interfaces_bins_) {
    node.add_interfaces_json(interface_json);
  }

  // Generate a new client context for the Attach method.
  // From the documentation it appears that a context needs to be valid for the duration of the RPC
  // In this case the RPC ends when the return stream is closed, so the context should survive at
  // least for that long.
  // See: https://grpc.github.io/grpc/cpp/classgrpc_1_1_client_context.html
  std::unique_ptr<ClientContext> context = std::make_unique<ClientContext>();
  std::unique_ptr<ClientReader<gRPCMessageHubEvent>> reader = stub_->Attach(context.get(), node);

  reader->WaitForInitialMetadata();
  auto server_metadata = context->GetServerInitialMetadata();
  if (server_metadata.empty()) {
    spdlog::warn("No metadata from server");
    spdlog::error("Attach to server failed");
    grpc_stream_error_.store(true);
    return astarte_tl::unexpected(AstarteGrpcLibError{"Attach to server failed"});
  }

  grpc_stream_error_.store(false);

  return AttachResult{.context = std::move(context), .reader = std::move(reader)};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::connection_attempt(const std::stop_token& token)
    -> astarte_tl::expected<void, AstarteError> {
  if (connected_.load()) {
    spdlog::warn("Device is already connected.");
    return astarte_tl::unexpected(AstarteOperationRefusedError{"The device is already connected"});
  }
  spdlog::debug("Attempting to connect to the message hub at {}", server_addr_);

  // Create a new channel and initialize the gRPC stub
  setup_grpc_channel();

  return perform_attach()
      .transform_error([](AstarteError error) {
        spdlog::error("Failed to attach to the message hub");
        return error;
      })
      .and_then([&](auto&& attach_res) {
        connected_.store(true);
        spdlog::info("Node connected");
        return handle_events(token, std::move(attach_res.context), std::move(attach_res.reader));
      })
      .transform([&]() {
        connected_.store(false);
        spdlog::info("Node disconnected");
      });
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::handle_events(
    const std::stop_token& token, std::unique_ptr<grpc::ClientContext> context,
    std::unique_ptr<ClientReader<gRPCMessageHubEvent>> reader)
    -> astarte_tl::expected<void, AstarteError> {
  (void)context;
  spdlog::debug("Event handler thread has been started");

  gRPCMessageHubEvent msghub_event;
  while (!token.stop_requested() && reader->Read(&msghub_event)) {
    spdlog::debug("Event from the message hub received.");
    auto parsed_message = AstarteDeviceGrpcImpl::parse_message_hub_event(msghub_event);
    if (!parsed_message) {
      return astarte_tl::unexpected(parsed_message.error());
    }
    this->rcv_queue_.push(parsed_message.value());
  }
  spdlog::info("Message hub stream has been interrupted.");

  // Log an error if the stream has been stopped due to a failure.
  const Status status = reader->Finish();
  if (!status.ok() && !token.stop_requested()) {
    grpc_stream_error_.store(true);
    const std::string msg =
        astarte_fmt::format("gRPC stream closed with error '{}' '{}'",
                            static_cast<int>(status.error_code()), status.error_message());
    spdlog::error(msg);
    return astarte_tl::unexpected(AstarteGrpcLibError{
        static_cast<std::uint64_t>(status.error_code()), status.error_message()});
  }
  return {};
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::parse_message_hub_event(
    const gRPCMessageHubEvent& event) -> astarte_tl::expected<AstarteMessage, AstarteError> {
  spdlog::trace("Parsing message hub event.");
  if (event.has_message()) {
    const gRPCAstarteMessage& astarteMessage = event.message();
    return GrpcConverterFrom{}(astarteMessage);
  }
  if (event.has_error()) {
    const gRPCMessageHubError& error = event.error();
    spdlog::error("Message hub error: {}", error.description());
    spdlog::error("Error source backtrace: ");
    for (const std::string& source : error.source()) {
      spdlog::error("  {}", source);
    }
    return astarte_tl::unexpected(
        AstarteMsgHubError{"Received gRPC error: " + error.description()});
  }
  spdlog::error("Unknown event type!");
  return astarte_tl::unexpected(AstarteInternalError{"Message hub event is of unknown type"});
}

auto AstarteDeviceGrpc::AstarteDeviceGrpcImpl::connection_loop(const std::stop_token& token)
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::trace("Connection loop started.");
  return ExponentialBackoff::create(std::chrono::seconds(2), std::chrono::minutes(1))
      .and_then([&](auto exp_backoff) -> astarte_tl::expected<void, AstarteError> {
        while (!token.stop_requested()) {
          (void)connection_attempt(token).or_else([&](const AstarteError& err) {
            // TODO(sorru94): Check the error type and conclude the loop if required.
            spdlog::error("Connection attempt failed with the following error.");
            spdlog::error(err);
            return astarte_tl::expected<void, AstarteError>{};
          });

          if (token.stop_requested()) {
            spdlog::info("Stop requested, will not attempt to reconnect.");
            break;
          }

          auto delay = exp_backoff.getNextDelay();
          spdlog::info("Will attempt to reconnect in {} seconds.",
                       std::chrono::duration_cast<std::chrono::seconds>(delay).count());
          std::this_thread::sleep_for(delay);
        }

        spdlog::info("Connection loop has been terminated.");
        return {};
      });
}

}  // namespace astarte::device
