SUMMARY = "Protocol Buffers - Google's data interchange format"
DESCRIPTION = "Protocol Buffers are a way of encoding structured data in \
an efficient yet extensible format"
SECTION = "libs"

DEPENDS = "protobuf-native"
DEPENDS_virtclass-native = ""
BBCLASSEXTEND = "native"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=af6809583bfde9a31595a58bb4a24514"


SRCREV = "31c54d1289f2ae44158eff631526762366ba1707"

SRC_URI = "git://github.com/google/protobuf.git;protocol=http"

EXTRA_OECONF += " --host=arm-linux-gnueabi --with-protoc=protoc"

inherit autotools
