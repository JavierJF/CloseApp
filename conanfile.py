from conans import ConanFile, CMake, tools
import os

class CloseAppConan(ConanFile):
    name = "CloseApplication"
    version = "0.1"
    license = "Raising The Floor"
    url = "https://gitlab.com/javjarfer/CloseApplication"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "tests": [True, False]}
    generators = "cmake"
    requires = (
        "gtest/1.8.0@lasote/stable",
        "UErrno/0.1@javjarfer/testing",
        "CArray/0.1@javjarfer/testing",
        "CVariant/0.1@javjarfer/testing",
        "FUtils/0.1@javjarfer/testing",
        "UIHandler/0.1@javjarfer/testing"
    )
    default_options = (
        "shared=False",
        "tests=False",
        #  GTest options
        "gtest:shared=False",
        "gtest:no_gmock=True",
        "gtest:no_main=True"
    )

    def source(self):
        self.run("git clone https://gitlab.com/javjarfer/CloseApplication")
        self.run("cd CloseApplication && git checkout develop")

    def build(self):
        cmake = CMake(self, parallel=True)
        if self.options.tests:
            cmake.definitions["CMAKE_BUILD_TESTS"] = "ON"
        cmake.configure(source_folder="CloseApplication")
        cmake.build(target="install")

    def imports(self):
       self.copy("*.dll", "bin")

