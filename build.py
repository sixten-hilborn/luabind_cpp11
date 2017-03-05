from conan.packager import ConanMultiPackager
import platform

if __name__ == "__main__":
    builder = ConanMultiPackager()
    builder.add_common_builds(shared_option_name="luabind11:shared", pure_c=False)
    # Remove compiler versions that don't support used C++14 features
    builder.builds = [
        [settings, options]
        for settings, options in builder.builds
        if  not (settings["compiler"] == "Visual Studio" and int(settings["compiler.version"]) < 13)
        and not (settings["compiler"] == "gcc" and float(settings["compiler.version"]) < 4.8)
    ]
    builder.run()

