<!-- (C) Copyright 2025, SECO Mind Srl

SPDX-License-Identifier: Apache-2.0 -->

# Astarte device SDK C++ - Qt example

This example demonstrates how to use the **Astarte device SDK for C++** in a **Qt** application.

## 🛠️ Requirements

- **Qt**

  ⚠️ *Minimum required version: 5.15*

- **CMake**

    ⚠️ *Minimum required version: 3.19*

- **C++ compiler**

    ⚠️ *Requires support for at least C++20*

- **gRPC**

  ⚠️ *Minimum required gRPC version: 1.69.0*

- **Astarte device SDK C++**

## ⚙️ Building the example using Qt Creator UI

1. Open the `astarte-device-sdk-cpp/samples/grpc/qt` folder using **Qt Creator** (*Open Project*).
2. Qt Creator will detect `CMakeLists.txt` and prompt you to configure the project.
3. Click the **hammer icon** (🔨) in the lower-left corner of Qt Creator to build the project.
4. The build output will appear in a directory like:

```
samples/grpc/qt/build/Desktop_Qt_6_8_1-Debug/
```

> **Note:** The exact folder name may differ depending on your selected Qt kit.

To run the example from root:

```bash
./samples/grpc/qt/build/Desktop_Qt_6_8_1-Debug/app
```

### ⚠️ Qt 5 build configuration: Disable USE_QT6 option in Qt Creator

If you're using Qt 5, make sure to manually disable the **USE_QT6** CMake option inside Qt Creator:

1. In Qt Creator, open the **Projects** tab (🔧 wrench icon on the left).

2. Select the active build kit.

3. Under the **CMake** section, locate the **Key: USE_QT6** entry.

4. Set its value to **OFF**.

This step ensures that the project builds properly with Qt 5.

## 🧰 Building from terminal with script

Instead of using **Qt Creator**, you can build the project directly from the terminal using the provided script:

```bash
./build_sample.sh grpc/qt [OPTIONS]
```

After building, the executable can be found in `build` folder.

To run the example from root:

```bash
./samples/grpc/qt/build/app
```

### Conan system dependencies

The script builds by default using Conan to import most of its dependencies. However, a couple of system packages are required for this approach to work. On debian systems install the system dependencies with the following command.
```bash
apt install -y libx11-dev libx11-xcb-dev libfontenc-dev libice-dev libsm-dev libxau-dev libxaw7-dev libxcomposite-dev libxdamage-dev libxkbfile-dev libxmu-dev libxmuu-dev libxpm-dev libxres-dev libxtst-dev libxcb-glx0-dev libxcb-xkb-dev libxcb-icccm4-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-dri3-dev libxcb-dri2-0-dev libxcb-present-dev libxcb-composite0-dev libxcb-ewmh-dev libxcb-res0-dev libxcb-util-dev
```
Conan will notify you of exactly which system dependencies you are missing. So if in doubt run the build script and check the output.

## ⚠️ Notes

If you encounter the following error:

```
'main.moc' file not found
```

Please verify:

- Any class that inherits from `QObject` includes the `Q_OBJECT` macro.
- `main.cpp` is part of the CMake project and recognized by Qt Creator.
- `CMAKE_AUTOMOC` is enabled in your `CMakeLists.txt` (as shown above).

The `main.moc` file is generated automatically during the build if the project is properly configured.

### 📄 About the CMakeList.txt

Make sure your `CMakeLists.txt` file includes the following content:

```cmake
target_link_libraries(app
    PRIVATE astarte_device_sdk
)

# Add the root SDK directory as subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/lib_build)

set(CMAKE_AUTOMOC ON)
```

This configuration ensures that the **Astarte device SDK** is fetched from GitHub and properly linked with the Qt project. The `CMAKE_AUTOMOC` setting is essential for Qt’s meta-object compiler to generate required `.moc` files automatically.
