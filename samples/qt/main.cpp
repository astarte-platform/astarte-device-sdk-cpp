// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <filesystem>
#include <memory>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device.hpp"
#include "astarte_device_sdk/msg.hpp"

using namespace AstarteDeviceSdk;

class AstarteWorker : public QObject {
  Q_OBJECT

 public:
  AstarteWorker(QObject *parent = nullptr) : QObject(parent) {
    QString server_addr = "localhost:50051";
    QString node_id = "aa04dade-9401-4c37-8c6a-d8da15b083ae";
    device = std::make_shared<AstarteDevice>(server_addr.toStdString(), node_id.toStdString());

    addInterfaces();
    device->connect();

    QTimer::singleShot(3000, this, &AstarteWorker::sendInitialData);

    pollingTimer = new QTimer(this);
    connect(pollingTimer, &QTimer::timeout, this, &AstarteWorker::pollMessages);
    pollingTimer->start(100);
  }

 private slots:
  void pollMessages() {
    auto incoming = device->poll_incoming();
    if (incoming.has_value()) {
      AstarteMessage msg(incoming.value());
      qDebug() << "Received:" << QString::fromStdString(msg.format());
    }
  }

  void sendInitialData() {
    {
      QString interface = "org.astarte-platform.cpp.examples.DeviceDatastream";
      auto now = std::chrono::system_clock::now();

      // Basic type
      device->send_individual(interface.toStdString(), "/integer_endpoint", AstarteData(43), &now);
      device->send_individual(interface.toStdString(), "/double_endpoint", AstarteData(43.5), &now);
      device->send_individual(interface.toStdString(), "/longinteger_endpoint",
                              AstarteData(int64_t(8589934592)), &now);
      device->send_individual(interface.toStdString(), "/boolean_endpoint", AstarteData(true),
                              &now);
      device->send_individual(interface.toStdString(), "/string_endpoint",
                              AstarteData(std::string("Hello from Qt!")), &now);

      // Binary blob
      std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
      device->send_individual(interface.toStdString(), "/binaryblob_endpoint",
                              AstarteData(binaryblob), &now);

      // Datetime
      device->send_individual(interface.toStdString(), "/datetime_endpoint", AstarteData(now),
                              &now);

      // Arrays
      std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
      device->send_individual(interface.toStdString(), "/integerarray_endpoint",
                              AstarteData(integerarray), &now);

      std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
      device->send_individual(interface.toStdString(), "/longintegerarray_endpoint",
                              AstarteData(longintegerarray), &now);

      std::vector<double> doublearray = {0.0, 1.1, 2.2};
      device->send_individual(interface.toStdString(), "/doublearray_endpoint",
                              AstarteData(doublearray), &now);

      std::vector<bool> booleanarray = {true, false, true};
      device->send_individual(interface.toStdString(), "/booleanarray_endpoint",
                              AstarteData(booleanarray), &now);

      std::vector<std::string> stringarray = {"Hello", "from", "Qt!"};
      device->send_individual(interface.toStdString(), "/stringarray_endpoint",
                              AstarteData(stringarray), &now);

      std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
      device->send_individual(interface.toStdString(), "/binaryblobarray_endpoint",
                              AstarteData(binaryblobarray), &now);

      std::vector<std::chrono::system_clock::time_point> datetimearray = {
          std::chrono::system_clock::now(),
          std::chrono::system_clock::now() + std::chrono::seconds(5)};
      device->send_individual(interface.toStdString(), "/datetimearray_endpoint",
                              AstarteData(datetimearray), &now);
    }

    {
      std::string interface_name = "org.astarte-platform.cpp.examples.DeviceAggregate";
      std::string common_path = "/sensor15";

      auto now = std::chrono::system_clock::now();

      // Fill aggregate object
      AstarteObject data = {
          {"integer_endpoint", AstarteData(43)},
          {"longinteger_endpoint", AstarteData(int64_t(8589934592))},
          {"double_endpoint", AstarteData(43.5)},
          {"boolean_endpoint", AstarteData(true)},
          {"string_endpoint", AstarteData(std::string("Hello from Qt!"))},
          {"binaryblob_endpoint", AstarteData(std::vector<uint8_t>{10, 20, 30, 40, 50})},
          {"datetime_endpoint", AstarteData(now)},
          {"integerarray_endpoint", AstarteData(std::vector<int32_t>{10, 20, 30, 40, 50})},
          {"longintegerarray_endpoint",
           AstarteData(std::vector<int64_t>{8589934592LL, 8589934593LL, 8589939592LL})},
          {"doublearray_endpoint", AstarteData(std::vector<double>{0.0})},
          {"booleanarray_endpoint", AstarteData(std::vector<bool>{true, false, true})},
          {"stringarray_endpoint",
           AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "Qt"})},
          {"binaryblobarray_endpoint",
           AstarteData(std::vector<std::vector<uint8_t>>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})},
          {"datetimearray_endpoint",
           AstarteData(std::vector<std::chrono::system_clock::time_point>{now, now})}};

      // Send aggregate object
      device->send_object(interface_name, common_path, data, nullptr);
    }
  }

 private:
  std::shared_ptr<AstarteDevice> device;
  QTimer *pollingTimer;

  void addInterfaces() {
    // Those paths assume the user is calling the Astarte executable from the root of this project.
    std::filesystem::path device_individual_interface_file_path =
        "samples/simple/interfaces/org.astarte-platform.cpp.examples.DeviceDatastream.json";
    std::filesystem::path server_individual_interface_file_path =
        "samples/simple/interfaces/org.astarte-platform.cpp.examples.ServerDatastream.json";
    std::filesystem::path device_property_interface_file_path =
        "samples/simple/interfaces/org.astarte-platform.cpp.examples.DeviceProperty.json";
    std::filesystem::path device_aggregated_interface_file_path =
        "samples/simple/interfaces/org.astarte-platform.cpp.examples.DeviceAggregate.json";
    std::filesystem::path server_aggregated_interface_file_path =
        "samples/simple/interfaces/org.astarte-platform.cpp.examples.ServerAggregate.json";
    std::filesystem::path server_property_interface_file_path =
        "samples/simple/interfaces/org.astarte-platform.cpp.examples.ServerProperty.json";

    device->add_interface_from_json(device_individual_interface_file_path);
    device->add_interface_from_json(server_individual_interface_file_path);
    device->add_interface_from_json(device_property_interface_file_path);
    device->add_interface_from_json(device_aggregated_interface_file_path);
    device->add_interface_from_json(server_aggregated_interface_file_path);
    device->add_interface_from_json(server_property_interface_file_path);
  }
};

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  AstarteWorker worker;
  return app.exec();
}

#include "main.moc"
