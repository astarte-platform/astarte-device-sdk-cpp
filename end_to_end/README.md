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
# Configuration for gRPC
server_addr = ""
node_id = ""

# Configuration for HTTP requests
realm = ""
appengine_url = ""
appengine_token = ""
device_id = ""
```

## Building

You can use the `ent_to_end.sh` script found in the root of this repository to build the end to
end tests.

## Running

After building the executable `end_to_end/build/end_to_end_runner` can be used to run the tests.
