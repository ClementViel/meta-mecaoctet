#
# Recipe describing the compilation of
# Open Lightning Architecture framework
#

DESCRIPTION = "Open Lightning Architecture"
SECTION = ""
DEPENDS = "cppunit protobuf ossp-uuid"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENCE;md5=7aa5f01584d845ad733abfa9f5cad2a1"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRCREV = "41456ae7256b148c1164070588531d3e218f892c"
SRC_URI = "git://github.com/OpenLightingProject/ola.git;protocol=http"
lib_ld_aarch64 = "${PKG_CONFIG_SYSROOT_DIR}/lib64/ld-linux-aarch64.so.1"
S = "${WORKDIR}/git"
inherit autotools

