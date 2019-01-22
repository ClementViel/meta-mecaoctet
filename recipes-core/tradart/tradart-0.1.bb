#
# 
# TradArt is a traductor from MecaOctet WebSocket
# to DMX console
#
SUMMARY = "WebSocket DMX traduction"
SECTION = "examples"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "libwebsockets libartnet"
RDEPENDS_${PN} = "libwebsockets libartnet"

SRC_URI = "file://wsclient.c \
           file://cJSON.c \
           file://cJSON.h \
           file://artnet-flood-tx.c \
		   file://hypervisor.sh \
           file://Makefile \
           "

S = "${WORKDIR}"

do_compile() {

    $CC -c cJSON.c
    $CC wsclient.c cJSON.o -o wsclient -lwebsockets 
    $CC artnet-flood-tx.c -o artnet -lartnet


}



do_install_append() {
             install -d ${D}/home/root
			 		  install -m 0755 hypervisor.sh ${D}/home/root
                      install -m 0755 wsclient ${D}/home/root
                      install -m 0755 artnet ${D}/home/root
}

FILES_${PN} += "/home/root/*"
