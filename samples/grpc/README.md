# Astarte device library sample

This sample shows the usage of the Astarte device library APIs for C++.

## Building the sample

To build the sample you can run the utility script `build_samples.sh` located in the root of this
repository. Make sure to execute it from its location.

Once built, you can find the build sample in the `samples/grpc/build` folder with the name `app`.
Before running the executable make sure that you have an instance of the Astarte message
hub running on your host machine and connected to Astarte.
Find more info regarding the Astarte message hub
[here](https://github.com/astarte-platform/astarte-message-hub).

The Astarte message hub should be configured to run gRPC with socket host `localhost` and port
`50051`.
