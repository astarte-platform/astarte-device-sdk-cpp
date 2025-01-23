// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

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

#include <ctime>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "astarte_device_sdk/device.h"
#include "astarte_device_sdk/exceptions.h"
#include "astarte_device_sdk/individual.h"
#include "grpc_interceptors.h"
#include "individual_private.h"

namespace AstarteDeviceSdk {

using std::make_unique;
using std::shared_ptr;
using std::time;

using grpc::Channel;
using grpc::Status;

using grpc::experimental::ClientInterceptorFactoryInterface;

using astarteplatform::msghub::AstarteDataType;
using astarteplatform::msghub::AstarteDataTypeIndividual;
using astarteplatform::msghub::AstarteMessage;
using astarteplatform::msghub::MessageHub;
using astarteplatform::msghub::Node;

AstarteDevice::AstarteDevice(std::string server_addr, std::string node_uuid)
    : server_addr_(std::move(server_addr)), node_uuid_(std::move(node_uuid)) {}

void AstarteDevice::add_interface_from_json(const std::filesystem::path &json_file) {
  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    std::cerr << "Could not open the interface file: " << json_file << "\n";
    throw AstarteFileOpenException(json_file.string());
  }
  // Read the entire JSON file content into a string
  const std::string interface_json((std::istreambuf_iterator<char>(interface_file)),
                                   std::istreambuf_iterator<char>());
  interfaces_bins_.push_back(interface_json);
  interface_file.close();
}

void AstarteDevice::connect() {
  std::cout << "Connecting to the message hub\n";

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
  std::cout << "Disonnecting from the message hub\n";

  // Call the dettach RPC.
  ClientContext context;
  google::protobuf::Empty response;
  stub_->Detach(&context, google::protobuf::Empty(), &response);
}

void AstarteDevice::stream_data(const std::string &interface_name, const std::string &path,
                                AstarteIndividual &data) {
  auto *grpc_data = new AstarteDataType();
  AstarteDataTypeIndividual *grpc_individual =
      std::visit(AstarteIndividualToAstarteDataTypeIndividual(), data);
  grpc_data->set_allocated_astarte_individual(grpc_individual);

  auto *grpc_timestamp = new google::protobuf::Timestamp();
  const time_t now = time(nullptr);
  grpc_timestamp->set_seconds(now);
  grpc_timestamp->set_nanos(0);

  AstarteMessage message;
  message.set_interface_name(interface_name);
  message.set_path(path);
  // The deallocation of grpc_data and grpc_timestamp is handled by gRPC
  // NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
  message.set_allocated_astarte_data(grpc_data);
  message.set_allocated_timestamp(grpc_timestamp);

  ClientContext context;
  google::protobuf::Empty response;
  stub_->Send(&context, message, &response);
  // NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
}

void AstarteDevice::handle_events(std::unique_ptr<ClientReader<MessageHubEvent>> reader) {
  std::cout << "Event handler thread has been started\n";

  // Read the message stream.
  MessageHubEvent msghub_event;
  while (reader->Read(&msghub_event)) {
    std::cout << "Received an event!\n";
    std::cout << msghub_event.DebugString() << "\n";
  }
  std::cout << "Message hub stream has been interrupted\n";

  // Log an error if it the stream has been stopped due to a failure.
  const Status status = reader->Finish();
  if (!status.ok()) {
    std::cerr << status.error_code() << ": " << status.error_message() << "\n";
  }
}

}  // namespace AstarteDeviceSdk
