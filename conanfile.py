from conans import ConanFile
import os
from conans.tools import get
from conans.tools import unzip
from conans import CMake

class Luabind11Conan(ConanFile):
    name = "luabind11"
    version = "0.9.1"
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
        self.makedir('_build')
        cmake = CMake(self.settings)
        cd_build = 'cd _build'
        shared_option = 1 if self.options.shared else 0
        options = (
            '-DCMAKE_INSTALL_PREFIX=../_build/install '
            '-DBUILD_TEST=0 '
            '-DBUILD_SHARED={0} ').format(shared_option)
        build_options = ''
        self.run_and_print('%s && cmake .. %s %s' % (cd_build, cmake.command_line, options))
        self.run_and_print("%s && cmake --build . --target install %s %s" % (cd_build, cmake.build_config, build_options))

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
        self.copy(pattern="*.dylib", dst="bin", src="_build/install/bin", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ['luabind11']
        if self.options.shared:
            self.cpp_info.defines = ['LUABIND_DYNAMIC_LINK']

    def makedir(self, path):
        if self.settings.os == "Windows":
            self.run("IF not exist {0} mkdir {0}".format(path))
        else:
            self.run("mkdir {0}".format(path))

    def run_and_print(self, command):
        self.output.info(command)
        self.run(command)
