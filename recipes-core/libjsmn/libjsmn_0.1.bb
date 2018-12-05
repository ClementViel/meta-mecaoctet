#
# Recipe describing the compilation of
# libjsmn : simple json parsing primitives
#

DESCRIPTION = "libjsmn"
SECTION = "libs"
DEPENDS = ""
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=5adc94605a1f7a797a9a834adbe335e3"

BB_STRICT_CHECKSUM = "0"
BBCLASSEXTENDS = "nativesdk"
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRCREV = "35086597a72d94d8393e6a90b96e553d714085bd"
SRC_URI = "git://github.com/zserge/jsmn.git;protocol=http"
S = "${WORKDIR}/git"

