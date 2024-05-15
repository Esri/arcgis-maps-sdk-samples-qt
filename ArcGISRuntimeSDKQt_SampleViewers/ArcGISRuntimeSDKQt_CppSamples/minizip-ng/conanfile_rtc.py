from conans import ConanFile


class MinizipNgConan(ConanFile):
    name = "minizip-ng"
    version = "4.0.4"
    url = "https://github.com/Esri/minizip-ng/tree/runtimecore"
    license = "https://github.com/Esri/minizip-ng/blob/runtimecore/LICENSE"
    description = "Fork of the popular zip manipulation library found in the zlib distribution."

    # RTC specific triple
    settings = "platform_architecture_target"

    def package(self):
        base = self.source_folder + "/"
        relative = "3rdparty/minizip-ng/"

        # headers
        self.copy("ioapi.h", src=base, dst=relative)
        self.copy("mz.h", src=base, dst=relative)
        self.copy("mz_compat.h", src=base, dst=relative)
        self.copy("unzip.h", src=base, dst=relative)
        self.copy("zip.h", src=base, dst=relative)

        # libraries
        output = "output/" + str(self.settings.platform_architecture_target) + "/staticlib"
        self.copy("*" + self.name + "*", src=base + "../../" + output, dst=output)