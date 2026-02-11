# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan import ConanFile

class Pkg(ConanFile):
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "transport": ["grpc", "mqtt"]}
    default_options = {"shared": False, "fPIC": True, "transport": "grpc"}

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def requirements(self):
        self.requires("cpr/1.11.2")
        self.requires("nlohmann_json/3.12.0")
        self.requires("tomlplusplus/3.4.0")
        if self.options.transport == "grpc":
            self.requires("astarte-device-sdk/0.9.0", options={"transport": "grpc"})
        else:
            self.requires("astarte-device-sdk/0.9.0", options={"transport": "mqtt"})

    def generate(self):
        tc = CMakeToolchain(self)
        if self.options.transport == "grpc":
            tc.variables["ASTARTE_TRANSPORT_GRPC"] = "ON"
        else:
            tc.variables["ASTARTE_TRANSPORT_GRPC"] = "OFF"
        tc.generate()
        cmake_deps = CMakeDeps(self)
        cmake_deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
