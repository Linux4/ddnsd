default:
	g++ ddnsd.cpp -I./ -o ddnsd
	g++ ddnsd-config.cpp -I./ -o ddnsd-config

install:
	@cp ./ddnsd /usr/bin/
	@cp ./ddnsd-config /usr/bin/
	@echo "[Unit]\nDescription=DDNS Daemon\n\n[Service]\nExecStart=/usr/bin/ddnsd\nPIDFile=/run/ddnsd.pid\n\n[Install]\nWantedBy=multi-user.target\n" > /lib/systemd/system/ddnsd.service
	systemctl enable ddnsd.service

uninstall:
	@systemctl stop ddnsd.service
	systemctl disable ddnsd.service
	@rm -f /lib/systemd/system/ddnsd.service
	@rm -f /usr/bin/ddnsd
	@rm -f /usr/bin/ddnsd-config
