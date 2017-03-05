from conans import ConanFile
import os
from conans.tools import get
from conans.tools import unzip
from conans import CMake

class Luabind11Conan(ConanFile):
    name = "luabind11"
    version = "0.9.1"
    description = "C++11 fork of luabind, a library to help creating bindings between C++ and Lua"
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False]
    }
    default_options = "shared=True"
    url = "https://github.com/sixten-hilborn/luabind_cpp11"
    license = "MIT - https://opensource.org/licenses/mit-license.php"
    exports = ['CMakeLists.txt', 'src*', 'luabind*']
    requires = (
        'lua/5.1.4@hilborn/stable',
        'Boost/1.60.0@lasote/stable'
    )


    def build(self):
        cmake = CMake(self.settings)
        options = {
            'CMAKE_INSTALL_PREFIX': '../_build/install',
            'BUILD_TEST': False,
            'BUILD_SHARED': self.options.shared
        }
        cmake.configure(self, defs=options, build_dir='_build')
        cmake.build(self, target='install')

    def package(self):
        # Headers
        self.copy(pattern="*.h", dst="include", src="_build/install/include", keep_path=True)
        self.copy(pattern="*.hpp", dst="include", src="_build/install/include", keep_path=True)

        # libs
        self.copy(pattern="*.a", dst="lib", src="_build/install/lib", keep_path=False)
        self.copy(pattern="*.so", dst="lib", src="_build/install/lib", keep_path=False)
        self.copy(pattern="*.dylib", dst="lib", src="_build/install/lib", keep_path=False)
        self.copy(pattern="*.lib", dst="lib", src="_build/install/lib", keep_path=False)

        # binaries
        self.copy(pattern="*.dll", dst="bin", src="_build/install/bin", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ['luabind11']
        if self.options.shared:
            self.cpp_info.defines = ['LUABIND_DYNAMIC_LINK']
