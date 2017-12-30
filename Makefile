default:
	g++ ddnsd.cpp -I./ -o ddnsd

install:
	@cp ./ddnsd /usr/bin/
	@echo "[Unit]\nDescription=DDNS Daemon\n\n[Service]\nExecStart=/usr/bin/ddnsd\nPIDFile=/run/ddnsd.pid\n\n[Install]\nWantedBy=multi-user.target\n" > /lib/systemd/system/ddnsd.service
	systemctl enable ddnsd.service
	@echo "Installation successfully completed!"
	@service ddnsd start
	@echo -e "ddnsd: \033[0;31mIt looks like the service is started first time, creating configuration files...\033[0m"
	@echo -e "ddnsd: \033[0;31mConfig file created.\033[0m"
	@echo -e "ddnsd: \033[0;31mPlease edit /etc/ddns/ddnsd.conf.\033[0m"
	@echo -e "ddnsd: \033[0;31mStopping service, after configuration type "service ddnsd start" to start the service.\033[0m"

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
