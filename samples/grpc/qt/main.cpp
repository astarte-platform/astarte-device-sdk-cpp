// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <chrono>
#include <filesystem>
#include <memory>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/grpc/device_grpc.hpp"
#include "astarte_device_sdk/msg.hpp"

using namespace astarte::device;
using namespace std::chrono_literals;

class AstarteWorker : public QObject {
  Q_OBJECT

 public:
  AstarteWorker(QObject* parent = nullptr) : QObject(parent) {
    QString server_addr = "localhost:50051";
    QString node_id = "aa04dade-9401-4c37-8c6a-d8da15b083ae";
    device = std::make_shared<AstarteDeviceGrpc>(server_addr.toStdString(), node_id.toStdString());

    addInterfaces();
    qInfo() << "Connecting the device";
    auto res = device->connect();
    if (!res) {
      qCritical() << "Device connection failed";
    }

    QTimer::singleShot(3000, this, &AstarteWorker::sendInitialData);

    pollingTimer = new QTimer(this);
    connect(pollingTimer, &QTimer::timeout, this, &AstarteWorker::pollMessages);
    pollingTimer->start(100);
  }

 private slots:
  void pollMessages() {
    auto incoming = device->poll_incoming(std::chrono::milliseconds(0));
    if (incoming.has_value()) {
      Message msg(incoming.value());
      qInfo() << "Received:" << QString::fromStdString(astarte_fmt::format("{}", msg));
    }
  }

  void sendInitialData() {
    {
      qInfo() << "Streaming individual data";
      QString interface = "org.astarte-platform.cpp.examples.DeviceDatastream";
      auto now = std::chrono::system_clock::now();

      // Basic type
      auto res =
          device->send_individual(interface.toStdString(), "/integer_endpoint", Data(43), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }
      res = device->send_individual(interface.toStdString(), "/double_endpoint", Data(43.5), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }
      res = device->send_individual(interface.toStdString(), "/longinteger_endpoint",
                                    Data(int64_t(8589934592)), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }
      res = device->send_individual(interface.toStdString(), "/boolean_endpoint", Data(true), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }
      res = device->send_individual(interface.toStdString(), "/string_endpoint",
                                    Data(std::string("Hello from Qt!")), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      // Binary blob
      std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
      res = device->send_individual(interface.toStdString(), "/binaryblob_endpoint",
                                    Data(binaryblob), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      // Datetime
      res = device->send_individual(interface.toStdString(), "/datetime_endpoint", Data(now), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      // Arrays
      std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
      res = device->send_individual(interface.toStdString(), "/integerarray_endpoint",
                                    Data(integerarray), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
      res = device->send_individual(interface.toStdString(), "/longintegerarray_endpoint",
                                    Data(longintegerarray), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      std::vector<double> doublearray = {0.0, 1.1, 2.2};
      res = device->send_individual(interface.toStdString(), "/doublearray_endpoint",
                                    Data(doublearray), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      std::vector<bool> booleanarray = {true, false, true};
      res = device->send_individual(interface.toStdString(), "/booleanarray_endpoint",
                                    Data(booleanarray), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      std::vector<std::string> stringarray = {"Hello", "from", "Qt!"};
      res = device->send_individual(interface.toStdString(), "/stringarray_endpoint",
                                    Data(stringarray), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
      res = device->send_individual(interface.toStdString(), "/binaryblobarray_endpoint",
                                    Data(binaryblobarray), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }

      std::vector<std::chrono::system_clock::time_point> datetimearray = {
          std::chrono::system_clock::now(),
          std::chrono::system_clock::now() + std::chrono::seconds(5)};
      res = device->send_individual(interface.toStdString(), "/datetimearray_endpoint",
                                    Data(datetimearray), &now);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }
    }

    {
      qInfo() << "Streaming object data";
      std::string interface_name = "org.astarte-platform.cpp.examples.DeviceAggregate";
      std::string common_path = "/sensor15";

      auto now = std::chrono::system_clock::now();

      // Fill aggregate object
      AstarteDatastreamObject data = {
          {"integer_endpoint", Data(43)},
          {"longinteger_endpoint", Data(int64_t(8589934592))},
          {"double_endpoint", Data(43.5)},
          {"boolean_endpoint", Data(true)},
          {"string_endpoint", Data(std::string("Hello from Qt!"))},
          {"binaryblob_endpoint", Data(std::vector<uint8_t>{10, 20, 30, 40, 50})},
          {"datetime_endpoint", Data(now)},
          {"integerarray_endpoint", Data(std::vector<int32_t>{10, 20, 30, 40, 50})},
          {"longintegerarray_endpoint",
           Data(std::vector<int64_t>{8589934592LL, 8589934593LL, 8589939592LL})},
          {"doublearray_endpoint", Data(std::vector<double>{0.0})},
          {"booleanarray_endpoint", Data(std::vector<bool>{true, false, true})},
          {"stringarray_endpoint",
           Data(std::vector<std::string>{"Hello ", "world ", "from ", "Qt"})},
          {"binaryblobarray_endpoint",
           Data(std::vector<std::vector<uint8_t>>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})},
          {"datetimearray_endpoint",
           Data(std::vector<std::chrono::system_clock::time_point>{now, now})}};

      // Send aggregate object
      auto res = device->send_object(interface_name, common_path, data, nullptr);
      if (!res) {
        qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
      }
    }
  }

 private:
  std::shared_ptr<AstarteDeviceGrpc> device;
  QTimer* pollingTimer;

  void addInterfaces() {
    qInfo() << "Adding interfaces";
    std::filesystem::path qt_dir = QCoreApplication::applicationDirPath().toStdString();
    std::filesystem::path interfaces_dir =
        qt_dir.parent_path().parent_path() / "grpc" / "native" / "interfaces";
    // Those paths assume the user is calling the Astarte executable from the root of this project.
    std::filesystem::path device_individual_interface_file_path =
        interfaces_dir / "org.astarte-platform.cpp.examples.DeviceDatastream.json";
    std::filesystem::path server_individual_interface_file_path =
        interfaces_dir / "org.astarte-platform.cpp.examples.ServerDatastream.json";
    std::filesystem::path device_property_interface_file_path =
        interfaces_dir / "org.astarte-platform.cpp.examples.DeviceProperty.json";
    std::filesystem::path device_aggregated_interface_file_path =
        interfaces_dir / "org.astarte-platform.cpp.examples.DeviceAggregate.json";
    std::filesystem::path server_aggregated_interface_file_path =
        interfaces_dir / "org.astarte-platform.cpp.examples.ServerAggregate.json";
    std::filesystem::path server_property_interface_file_path =
        interfaces_dir / "org.astarte-platform.cpp.examples.ServerProperty.json";

    auto res = device->add_interface_from_file(device_individual_interface_file_path);
    if (!res) {
      qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
    }
    res = device->add_interface_from_file(server_individual_interface_file_path);
    if (!res) {
      qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
    }
    res = device->add_interface_from_file(device_property_interface_file_path);
    if (!res) {
      qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
    }
    res = device->add_interface_from_file(device_aggregated_interface_file_path);
    if (!res) {
      qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
    }
    res = device->add_interface_from_file(server_aggregated_interface_file_path);
    if (!res) {
      qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
    }
    res = device->add_interface_from_file(server_property_interface_file_path);
    if (!res) {
      qCritical() << QString::fromStdString(astarte_fmt::format("{}", res.error()));
    }
  }
};

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  AstarteWorker worker;
  return app.exec();
}

#include "main.moc"
