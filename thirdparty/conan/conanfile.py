from conan import ConanFile

class Parts(ConanFile):
    license = "BSD"
    author = "Antal Tátrai <antal.tatrai@gmail.com>"
    url = "https://github.com/tatraian/parts"

    settings = "os", "compiler", "build_type", "arch"
    generators = ["CMakeDeps"]

    def configure(self):
        self.options["zlib/*"].shared = False
        self.options["zlib/*"].fPIC = True
        self.options["gtest/*"].shared = False
        self.options["xz_utils/*"].shared = False
        self.options["xz_utils/*"].fPIC = True

    def requirements(self):
        self.requires("fmt/10.1.0")
        self.requires("gtest/1.14.0")
        self.requires("fakeit/2.4.0")
        self.requires("zlib/1.3")
        self.requires("xz_utils/5.4.5")
