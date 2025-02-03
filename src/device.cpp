// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/device.h"

#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/astarte_type.pb.h>
#include <astarteplatform/msghub/node.pb.h>
#include <google/protobuf/empty.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <grpcpp/support/client_interceptor.h>
#include <grpcpp/support/status.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "aggregate_private.h"
#include "astarte_device_sdk/exceptions.h"
#include "astarte_device_sdk/individual.h"
#include "grpc_interceptors.h"
#include "individual_private.h"

namespace AstarteDeviceSdk {

using std::make_unique;
using std::shared_ptr;

using grpc::Channel;
using grpc::Status;

using grpc::experimental::ClientInterceptorFactoryInterface;

using astarteplatform::msghub::AstarteDataType;
using astarteplatform::msghub::AstarteDataTypeIndividual;
using astarteplatform::msghub::AstarteMessage;
using astarteplatform::msghub::AstarteUnset;
using astarteplatform::msghub::MessageHub;
using astarteplatform::msghub::Node;

AstarteDevice::AstarteDevice(std::string server_addr, std::string node_uuid)
    : server_addr_(std::move(server_addr)), node_uuid_(std::move(node_uuid)) {}

void AstarteDevice::add_interface_from_json(const std::filesystem::path &json_file) {
  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    spdlog::error("Could not open the interface file: {}", json_file.string());
    throw AstarteFileOpenException(json_file.string());
  }
  // Read the entire JSON file content into a string
  const std::string interface_json((std::istreambuf_iterator<char>(interface_file)),
                                   std::istreambuf_iterator<char>());
  interfaces_bins_.push_back(interface_json);
  spdlog::debug("Adding interface to list of interfaces: \n{}", interface_json);
  interface_file.close();
}

void AstarteDevice::connect() {
  spdlog::info("Connecting to the message hub");

  // Create a new channel and initialize the gRPC stub
  const grpc::ChannelArguments args;
  std::vector<std::unique_ptr<ClientInterceptorFactoryInterface>> interceptor_creators;
  interceptor_creators.push_back(make_unique<NodeIdInterceptorFactory>(node_uuid_));

  const shared_ptr<Channel> channel = CreateCustomChannelWithInterceptors(
      server_addr_, grpc::InsecureChannelCredentials(), args, std::move(interceptor_creators));

  stub_ = MessageHub::NewStub(channel);

  // Create the node message for the attach RPC.
  Node node;
  for (const std::string &interface_json : interfaces_bins_) {
    node.add_interfaces_json(interface_json);
  }

  // Call the attach RPC.
  std::unique_ptr<ClientReader<MessageHubEvent>> reader = stub_->Attach(&client_context_, node);

  // Starte a thread for the event stream
  event_handler_ = std::thread(AstarteDevice::handle_events, std::move(reader));
}

void AstarteDevice::disconnect() {
  spdlog::info("Disonnecting from the message hub");

  // Call the dettach RPC.
  ClientContext context;
  google::protobuf::Empty response;
  const Status status = stub_->Detach(&context, google::protobuf::Empty(), &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
  }
}

void AstarteDevice::stream_individual(const std::string &interface_name, const std::string &path,
                                      AstarteIndividual &individual,
                                      std::chrono::system_clock::time_point *timestamp) {
  auto *grpc_data = new AstarteDataType();
  AstarteDataTypeIndividual *grpc_individual =
      std::visit(AstarteIndividualToAstarteDataTypeIndividual(), individual);
  grpc_data->set_allocated_astarte_individual(grpc_individual);

  google::protobuf::Timestamp *grpc_timestamp = nullptr;
  if (timestamp != nullptr) {
    const std::chrono::system_clock::duration t_duration = timestamp->time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    grpc_timestamp = new google::protobuf::Timestamp();
    grpc_timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    grpc_timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  }

  AstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);
  message.set_allocated_astarte_data(grpc_data);
  if (timestamp != nullptr) {
    message.set_allocated_timestamp(grpc_timestamp);
  }

  ClientContext context;
  google::protobuf::Empty response;
  spdlog::debug("Streaming individual data: {} {} {}", interface_name, path, message.DebugString());
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteIncompatibleInputException(status.error_message());
  }
}

void AstarteDevice::stream_aggregated(
    const std::string &interface_name, const std::string &path,
    std::unordered_map<std::string, AstarteIndividual> &aggregated,
    std::chrono::system_clock::time_point *timestamp) {
  auto *grpc_data = new AstarteDataType();
  const AstarteAggregateToAstarteDataTypeObject converter;
  AstarteDataTypeObject *grpc_object = converter(aggregated);
  grpc_data->set_allocated_astarte_object(grpc_object);

  google::protobuf::Timestamp *grpc_timestamp = nullptr;
  if (timestamp != nullptr) {
    const std::chrono::system_clock::duration t_duration = timestamp->time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    grpc_timestamp = new google::protobuf::Timestamp();
    grpc_timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    grpc_timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  }

  AstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);
  message.set_allocated_astarte_data(grpc_data);
  if (timestamp != nullptr) {
    message.set_allocated_timestamp(grpc_timestamp);
  }

  ClientContext context;
  google::protobuf::Empty response;
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteIncompatibleInputException(status.error_message());
  }
}

void AstarteDevice::set_property(const std::string &interface_name, const std::string &path,
                                 AstarteIndividual &data) {
  this->stream_individual(interface_name, path, data, nullptr);
}

void AstarteDevice::unset_property(const std::string &interface_name, const std::string &path) {
  auto *grpc_data = new AstarteUnset();
  AstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);
  message.set_allocated_astarte_unset(grpc_data);

  ClientContext context;
  google::protobuf::Empty response;
  const Status status = stub_->Send(&context, message, &response);
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    throw AstarteIncompatibleInputException(status.error_message());
  }
}

void AstarteDevice::handle_events(std::unique_ptr<ClientReader<MessageHubEvent>> reader) {
  spdlog::debug("Event handler thread has been started");

  // Read the message stream.
  MessageHubEvent msghub_event;
  while (reader->Read(&msghub_event)) {
    spdlog::debug("Event from the message hub received.");
    spdlog::debug(msghub_event.DebugString());
  }
  spdlog::info("Message hub stream has been interrupted.");

  // Log an error if it the stream has been stopped due to a failure.
  const Status status = reader->Finish();
  if (!status.ok()) {
    spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
  }
}

}  // namespace AstarteDeviceSdk
