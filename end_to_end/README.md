<!--
Copyright 2025 SECO Mind Srl

SPDX-License-Identifier: Apache-2.0
-->

# End to end tests

This is a simple end to end framework that tests basic functionality for this SDK.

## Configuration

The end to end test expects a `config.toml` file to be found in this folder (`end_to_end`).
The structure should be as follows:

```toml
# Configuration for HTTP requests
realm = ""
astarte_base_url = ""
appengine_token = ""
device_id = ""
```

Some configuration arguments are transport specific and should be added in the `config.toml` based
on the desired transport.

### gRPC specific

```toml
# Configuration for gRPC
[grpc]
server_addr = ""
node_id = ""
```

### MQTT specific

```toml
# Configuration for MQTT
[mqtt]
store_dir = ""
pairing_token = ""
```

## Building

You can use the `ent_to_end.sh` script found in the root of this repository to build the end to
end tests.

## Running

After building the executable `end_to_end/build/app` can be used to run the tests.
