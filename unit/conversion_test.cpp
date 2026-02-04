// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "astarte_device_sdk/data.hpp"

#if defined(ASTARTE_TRANSPORT_GRPC)
#include "grpc/grpc_converter.hpp"

using astarte::device::Data;
using astarte::device::grpc::gRPCAstarteData;
using astarte::device::grpc::GrpcConverterFrom;
using astarte::device::grpc::GrpcConverterTo;

TEST(AstarteTestConversion, DataToGrpc) {
  int32_t value = 199;
  auto data = Data(value);
  std::unique_ptr<gRPCAstarteData> grpc_individual =
      std::visit(GrpcConverterTo(), data.get_raw_data());
  EXPECT_EQ(grpc_individual->astarte_data_case(), gRPCAstarteData::kInteger);
  EXPECT_EQ(grpc_individual->integer(), value);
  GrpcConverterFrom converter;
  Data original = converter(*grpc_individual).value();
  EXPECT_EQ(original.into<int32_t>(), value);
}
#endif
