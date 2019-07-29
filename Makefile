VERSION := $(shell cat .version)
VERSION := ${VERSION:v%=%}

default:
	g++ -O3 ddnsd.cpp -o ddnsd -lresolv -lcurl

install:
	@mkdir -p /etc/ddns/
	@cp ./ddnsd /usr/bin/
	@echo "[Unit]\nDescription=DDNS Daemon\nAfter=network.target bind9.service\n\n[Service]\nExecStart=/usr/bin/ddnsd\nPIDFile=/run/ddnsd.pid\n\n[Install]\nWantedBy=multi-user.target\n" > /lib/systemd/system/ddnsd.service
	systemctl enable ddnsd.service
	@echo "Installation successfully completed!"
	@service ddnsd start
	@echo "ddnsd: \033[0;31mIt looks like the service is started first time, creating configuration files...\033[0m"
	@echo "ddnsd: \033[0;31mConfig file created.\033[0m"
	@echo "ddnsd: \033[0;31mPlease edit /etc/ddns/ddnsd.conf.\033[0m"
	@echo "ddnsd: \033[0;31mStopping service, after configuration type \"service ddnsd start\" to start the service.\033[0m"
	@echo "ddnsd: \033[0;31mBefore you restart the service make sure that your server/computer is connected to the internet.\033[0m"
	@echo "ddnsd: \033[0;31mAlso check that your DNS Zone files contain your CURRENT IP-ADDRESS.\033[0m"

update:
	@service ddnsd stop
	@cp ddnsd /usr/bin
	@service ddnsd start
	@echo "Update successfully!"

uninstall:
	@systemctl stop ddnsd.service
	systemctl disable ddnsd.service
	@rm -f /lib/systemd/system/ddnsd.service
	@rm -f /usr/bin/ddnsd

clean:
	rm -f ddnsd
	rm -rf debian/tmp
	rm -f debian/files

deb-pkg:
	dpkg-buildpackage

deb-files:
	@mkdir -p debian/tmp/lib/systemd/system/
	@mkdir -p debian/tmp/usr/bin/
	@mkdir -p  debian/tmp/etc/ddns/
	@echo "0" > debian/tmp/etc/ddns/.installed.ddns
	@echo "" > debian/tmp/etc/ddns/.oldip.ddns
	@echo "" > debian/tmp/etc/ddns/.oldip6.ddns
	@echo "[Unit]\nDescription=DDNS Daemon\nAfter=network.target bind9.service\n\n[Service]\nExecStart=/usr/bin/ddnsd\nPIDFile=/run/ddnsd.pid\n\n[Install]\nWantedBy=multi-user.target\n" > debian/tmp/lib/systemd/system/ddnsd.service
	@cp ./ddnsd debian/tmp/usr/bin/
	@echo "ddnsd_${VERSION}_${DEB_TARGET_ARCH}.buildinfo utils optional" >> debian/files
	@echo "ddnsd_${VERSION}_${DEB_TARGET_ARCH}.deb utils optional" >> debian/files
	@mkdir -p debian/tmp/DEBIAN
	@cp debian/raw/conffiles debian/tmp/DEBIAN/
	@cp debian/raw/postinst debian/tmp/DEBIAN/
	@cp debian/raw/control debian/tmp/DEBIAN/
	@cp debian/raw/ddnsd.conf debian/tmp/etc/ddns/ddnsd.conf
	@sed -i "s/<ARCH>/${DEB_TARGET_ARCH}/g" debian/tmp/DEBIAN/control
	@sed -i "s/<VERSION>/${VERSION}/g" debian/tmp/DEBIAN/control
	dpkg-deb --build debian/tmp ../ddnsd_${VERSION}_${DEB_TARGET_ARCH}.deb
