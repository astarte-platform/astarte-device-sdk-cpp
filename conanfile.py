# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan import ConanFile
from conan.tools.build import check_min_cppstd

class Pkg(ConanFile):
    name = "astarte-device-sdk"
    version = "0.7.0"
    package_type = "library"
    languages = "C++"
    license = "Apache-2.0"
    author = "Simone Orru (simone.orru@secomind.com)"
    url = "https://github.com/astarte-platform/astarte-device-sdk-cpp"
    description = "Package containing the proto definitions for the Astarte message hub"

    # Settings and options
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Build configuration
    build_policy = "missing"
    exports_sources = "CMakeLists.txt", "cmake/Config.cmake.in", "cmake/pkg-config-template.pc.in", "src/*", "include/*", "private/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def requirements(self):
        self.requires("grpc/1.72.0")
        self.requires("protobuf/6.30.1", override = True)
        self.requires("spdlog/1.15.3", options={"use_std_fmt": "True"}, transitive_headers=True, transitive_libs=True)

    def build_requirements(self):
        self.tool_requires("protobuf/6.30.1")

    def validate(self):
        check_min_cppstd(self, 20)
        # TODO: de-activate use of stdfmt when the compiler is not compatible

    def package_info(self):
        self.cpp_info.libs = ["astarte_device_sdk", "astarte_msghub_proto"]
        self.cpp_info.set_property("cmake_file_name", "astarte_device_sdk")
        self.cpp_info.set_property("cmake_target_name", "astarte_device_sdk::astarte_device_sdk")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["ASTARTE_USE_SYSTEM_SPDLOG"] = "ON"
        tc.variables["ASTARTE_USE_SYSTEM_GRPC"] = "ON"
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
