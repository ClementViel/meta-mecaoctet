SUMMARY = "EspressOctet"
DESCRIPTION = "Populate Espressobin rootfs with custom files"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = 	"file://netif_startup \
		 	 file://netif_startup.service \
			"
S = "${WORKDIR}"

do_install_append () {
	install -d ${D}/usr/bin
	install -d ${D}/etc/systemd/system
	install -m 0755 netif_startup ${D}/usr/bin
	install -m 0755 netif_startup.service ${D}/etc/systemd/system
}

FILES_${PN} += "/usr/bin/*"
FILES_${PN} += "/etc/system/systemd/*"


