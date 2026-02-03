# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan import ConanFile

class Pkg(ConanFile):
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "qt6": [True, False]}
    default_options = {"shared": False, "fPIC": True, "qt6": True}

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def requirements(self):
        if self.options.qt6:
            self.requires("qt/6.5.3")
            self.output.info("Qt6 added as a requirement.")
        else:
            self.requires("qt/5.15.16")
            self.output.info("Qt5 added as a requirement.")
        self.requires("astarte-device-sdk/0.8.1")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["USE_QT6"] = self.options.qt6
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
