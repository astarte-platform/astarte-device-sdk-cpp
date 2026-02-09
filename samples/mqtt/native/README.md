<!--
Copyright 2025 SECO Mind Srl

SPDX-License-Identifier: Apache-2.0
-->

# Astarte device SDK for C++: MQTT example

This sample demonstrates the capabilities of the Astarte Ddvice SDK for C++ over an MQTT connection. It covers:
- Registering a new device.
- Connecting to Astarte via MQTT.
- Sending data.

## Configuration

You must configure the example by creating a `config.toml` file.

Below is a template for `config.toml`:
```
astarte_base_url = ""
realm = ""
device_id = ""
store_dir = ""
pairing_token = ""
#credential_secret = ""

[features]
DEVICE_REGISTRATION = true
DEVICE_CONNECTION = true

```

### Configuration parameters

| Parameter    | Description |
| -------- | ------- |
| astarte_base_url  | The API URL of your Astarte instance. |
| realm | The name of the Astarte realm where the device will be registered. |
| device_id | A 128-bit URL-encoded Base64 string that uniquely identifies the device. |
| pairing_token | The token required to authenticate and register the device with Astarte. |

## Building

To build the sample, run the `build_sample.sh` script located in the project root.
```bash
./build_sample.sh mqtt/native --transport mqtt
```

## Running

After a successful build, execute the binary directly:
```bash
./samples/mqtt/native/build/app
```
