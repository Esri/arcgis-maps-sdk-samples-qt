from conans import ConanFile


class ZlibNgConan(ConanFile):
    name = "zlib-ng"
    version = "2.1.6"
    url = "https://github.com/Esri/zlib-ng/tree/runtimecore"
    license = "https://github.com/Esri/zlib-ng/blob/runtimecore/LICENSE.md"
    description = "zlib replacement with optimizations for \"next generation\" systems."

    # RTC specific triple
    settings = "platform_architecture_target"

    def package(self):
        base = self.source_folder + "/"
        relative = "3rdparty/zlib-ng/"

        # headers
        self.copy("zlib.h", src=base, dst=relative)
        self.copy("zconf.h", src=base, dst=relative)
        self.copy("zlib_name_mangling.h", src=base, dst=relative)

        # libraries
        output = "output/" + str(self.settings.platform_architecture_target) + "/staticlib"
        self.copy("*" + self.name + "*", src=base + "../../" + output, dst=output)