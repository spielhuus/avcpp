from conans import ConanFile, CMake, tools
import zipfile

class AvcppConan(ConanFile):
    name = "avcpp"
    version = "0.2.4"
    url = 'https://github.com/cppwrappers/LibAvCpp'
    license = 'LGPL'
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    description = "C++ wrapper for ffmpeg libraries"
    requires = "ffmpeg/3.4.1@conan-cpp/latest", "chromaprint/1.4.3@conan-cpp/latest", "OpenSSL/1.0.2m@conan/stable", "gtest/1.8.0@bincrafters/stable"
    options = { "shared": [True, False], "build_tests":  [True, False], "build_samples": [True, False]}
    default_options = "shared=False", "*:shared=False", "build_tests=False", "build_samples=False"
    exports_sources = "*", "av/"

    def build(self):
        cmake = CMake(self)
        cmake.configure(defs={
                "CMAKE_INSTALL_PREFIX": self.package_folder
            }, source_dir=".")
        cmake.build(target="install")

    def package(self):
        self.copy('*.h', dst='include/av', src='av/')
        if self.options.shared:
            if self.settings.os == "Macos":
                self.copy(pattern="*.dylib", dst="lib", keep_path=False)
            else:
                self.copy(pattern="*.so*", dst="lib", keep_path=False)
        else:
            self.copy(pattern="*.a", dst="lib", src='lib', keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["av"]

