#
# Recipe describing the compilation of
# LibArtNet library
#

DESCRIPTION = "LibArtNet"
SECTION = "libs"
DEPENDS = ""
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=7fbc338309ac38fefcd64b04bb903e34"

BB_STRICT_CHECKSUM = "0"
BBCLASSEXTENDS = "nativesdk"
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRCREV = "713efee8b0d9dfd19d5267fca0685810e093c2fa"
SRC_URI = "git://github.com/OpenLightingProject/libartnet.git;protocol=http"
S = "${WORKDIR}/git"
inherit autotools
EXTRA_OECONF = "ac_cv_func_malloc_0_nonnull=yes  ac_cv_func_realloc_0_nonnull=yes"

