// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "device_impl.hpp"

#include <astarteplatform/msghub/astarte_data.pb.h>
#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/message_hub_error.pb.h>
#include <astarteplatform/msghub/message_hub_service.grpc.pb.h>
#include <astarteplatform/msghub/node.pb.h>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <grpcpp/support/client_interceptor.h>
#include <grpcpp/support/status.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device.hpp"
#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
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

AstarteDevice::AstarteDeviceImpl::AstarteDeviceImpl(std::string server_addr, std::string node_uuid)
    : server_addr_(std::move(server_addr)), node_uuid_(std::move(node_uuid)) {}

void AstarteDevice::AstarteDeviceImpl::add_interface_from_json(
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

void AstarteDevice::AstarteDeviceImpl::connect() {
  spdlog::info("Connecting to the message hub");

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

  // Call the attach RPC.
  std::unique_ptr<ClientReader<gRPCMessageHubEvent>> reader = stub_->Attach(&client_context_, node);

  // Start a thread for the event stream
  event_handler_ = std::thread(&AstarteDeviceImpl::handle_events, this, std::move(reader));
}

void AstarteDevice::AstarteDeviceImpl::disconnect() {
  spdlog::info("Disconnecting from the message hub");

  // Call the dettach RPC.
  ClientContext context;
  google::protobuf::Empty response;
  const Status status = stub_->Detach(&context, google::protobuf::Empty(), &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
  }
}

void AstarteDevice::AstarteDeviceImpl::send_individual(
    const std::string &interface_name, const std::string &path, const AstarteData &data,
    const std::chrono::system_clock::time_point *timestamp) {
  spdlog::debug("Sending individual: {} {}", interface_name, path);
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
  spdlog::trace("Content: \n{}", message.DebugString());
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDevice::AstarteDeviceImpl::send_object(
    const std::string &interface_name, const std::string &path,
    const AstarteDatastreamObject &object, const std::chrono::system_clock::time_point *timestamp) {
  spdlog::debug("Sending object: {} {}", interface_name, path);
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
  spdlog::trace("Content: \n{}", message.DebugString());
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDevice::AstarteDeviceImpl::set_property(const std::string &interface_name,
                                                    const std::string &path,
                                                    const AstarteData &data) {
  spdlog::debug("Setting property: {} {}", interface_name, path);
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
  spdlog::trace("Content: \n{}", message.DebugString());
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteInvalidInputException(status.error_message());
  }
}

void AstarteDevice::AstarteDeviceImpl::unset_property(const std::string &interface_name,
                                                      const std::string &path) {
  spdlog::debug("Unsetting property: {} {}", interface_name, path);
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

auto AstarteDevice::AstarteDeviceImpl::poll_incoming() -> std::optional<AstarteMessage> {
  return rcv_queue_.pop();
}

void AstarteDevice::AstarteDeviceImpl::handle_events(
    std::unique_ptr<ClientReader<gRPCMessageHubEvent>> reader) {
  spdlog::debug("Event handler thread has been started");

  // Read the message stream.
  gRPCMessageHubEvent msghub_event;
  while (reader->Read(&msghub_event)) {
    spdlog::debug("Event from the message hub received.");
    std::optional<AstarteMessage> parsed_event =
        AstarteDeviceImpl::parse_message_hub_event(msghub_event);
    if (parsed_event.has_value()) {
      const AstarteMessage &parsed_message = parsed_event.value();
      this->rcv_queue_.push(parsed_message);
    }
  }
  spdlog::info("Message hub stream has been interrupted.");

  // Log an error if it the stream has been stopped due to a failure.
  const Status status = reader->Finish();
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
  }
}

auto AstarteDevice::AstarteDeviceImpl::parse_message_hub_event(const gRPCMessageHubEvent &event)
    -> std::optional<AstarteMessage> {
  std::optional<AstarteMessage> res = std::nullopt;
  if (event.has_message()) {
    const gRPCAstarteMessage &astarteMessage = event.message();
    GrpcConverterFrom converter;
    res = converter(astarteMessage);
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

}  // namespace AstarteDeviceSdk
