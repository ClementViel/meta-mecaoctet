#
# Recipe describing the compilation of
# libcjson : simple json parsing primitives in C
#

DESCRIPTION = "libcjson"
SECTION = "libs"
DEPENDS = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=218947f77e8cb8e2fa02918dc41c50d0"

BB_STRICT_CHECKSUM = "0"
BBCLASSEXTENDS = "native nativesdk"
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRCREV = "7cc52f60356909b3dd260304c7c50c0693699353"
SRC_URI = "git://github.com/DaveGamble/cJSON.git;protocol=http"
S = "${WORKDIR}/git"

