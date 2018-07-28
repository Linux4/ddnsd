#include "ddnsd.h"

int main(int argc, char** argv) {
	std::fstream f;
	std::string version = "v5.1.5";
	std::string release_date = "28.07.2018";
	std::string config = "/etc/ddns/ddnsd.conf";
	std::string update_checker = utils::read_config(config, "update_checker = ");
	if (update_checker == "true") {
		std::string remote_version = utils::shell_exec("curl --silent https://raw.githubusercontent.com/Schmorzel/ddnsd/master/.version");
     	   	boost::replace_all(remote_version, "\n", "");
        	boost::replace_all(remote_version, "\r", "");
        	if (remote_version != version) {
			if (remote_version.length() == 0) {
				std::cout << "Could not check for updates!" << std::endl;
			}
			else {
                		std::cout << "Update to version " << remote_version << " available!" << std::endl;
                		std::cout << "To update clone the following Git Repository: https://github.com/Schmorzel/ddnsd" << std::endl;
				std::cout << "Or use the APT Repository http://deb.server24-7.eu/" << std::endl;
			}
        	}
	}
	if (argc > 1) {
		if (std::string(argv[1]) == "-version" || std::string(argv[1]) == "--version") {
			std::cout << "DDNSD " << version << " " << release_date << std::endl;
			exit(0);
		} else if(std::string(argv[1]) == "-firstrun" || std::string(argv[1]) == "--firstrun") {
			system("curl --silent https://v4.ident.me >/etc/ddns/.oldip.ddns");
			system("curl --silent https://v6.ident.me >/etc/ddns/.oldip6.ddns");
			exit(0);
		}
	}
	//Read config
	std::string enabled;
	enabled = utils::read_config(config, "enabled = ");
	if (enabled == "false") {
		std::cout << "/etc/ddns/ddnsd.conf: enabled is set to false... Stopping service..." << std::endl;
		exit(0);
	}
	std::string update_freq_string;
	update_freq_string = utils::read_config(config, "update_freq = ");
	std::string zones_string;
	zones_string = utils::read_config(config, "zones = ");
	std::vector<std::string> zones;
	boost::split(zones, zones_string, boost::is_any_of(","));
	std::string cmds_string;
	cmds_string = utils::read_config(config, "post_update_cmds = ");
	std::vector<std::string> cmds;
	boost::split(cmds, cmds_string, boost::is_any_of(","));
	std::string config_version;
	config_version = utils::read_config(config, "config_version = ");
	f.open("/run/ddnsd.pid", std::ios::out );
	f << getpid();
	f << "\n";
	f.close();
	//Check if config exists, if not create config files
	if (config_version.length() == 0) {
		std::cout << "It looks like the service is started first time, creating configuration files..." << std::endl;
		system("mkdir -p /etc/ddns & echo \"#DDNSD Configuration\n\n#Enable (true)/Disable (false) the service:\nenabled = true\n\n#IP-Address update frequency:\nupdate_freq = 60\n\n#Domain Name and Path to DNS zone files (format: yourdomain.com:/etc/bind/db.yourdomain.com) seperated by comma:\nzones = example.com:/etc/bind/db.example.com,example2.com:/etc/bind/db.example2.com\n\n#Commands that will be executed after DNS zone update seperated by comma:\npost_update_cmds = service bind9 restart,custom_cmd,custom_cmd2\n\n#Enable (true)/Disable (false) the Update Checker:\nupdate_checker = true\n\n#Do not touch:\nconfig_version = 2\n\" >/etc/ddns/ddnsd.conf");
		system("curl --silent https://v4.ident.me >/etc/ddns/.oldip.ddns");
		system("curl --silent https://v6.ident.me >/etc/ddns/.oldip6.ddns");
		std::cout << "Config file created." << std::endl;
		std::cout << "Please edit /etc/ddns/ddnsd.conf" << std::endl;;
		std::cout << "Stopping service, after configuration type \"service ddnsd start\" to start the service." << std::endl;
		exit(1);
	}
        int update_freq = atoi(update_freq_string.c_str());
	update_freq_string.clear();
	//Check if update frequency is a number and greater 0
        if(update_freq == 0) {
                std::cerr << "ERROR: The given update frequency (" << update_freq_string << ") is not a valid number!" << std::endl;
                std::cerr << "The number has to be greater than 0!" << std::endl;
                exit(1);
        }
	while(true) {
		//Wait as long as update_freq
		std::this_thread::sleep_for (std::chrono::seconds(update_freq));
		while(true) {
			//Read last known IP-Adress
			f.open("/etc/ddns/.oldip.ddns", std::fstream::in );
			std::string OLDIP;
			getline( f, OLDIP, '\0');
			f.close();
			f.open("/etc/ddns/.oldip6.ddns", std::fstream::in);
			std::string OLDIP6;
			getline(f, OLDIP6, '\0');
			f.close();
			//Get current IP-Adress
			std::string IP = utils::shell_exec("curl --silent https://v4.ident.me/");
			std::string IP6 = utils::shell_exec("curl --silent https://v6.ident.me/");
			//Check if IP is a valid IP-Adress
			//e.g if no internet connection is available
			bool ipv4 = true;
			if (!ddnsd::is_ipv4_address(IP)) {
				std::cerr << "ERROR: Failed to get valid IPv4-Address" << std::endl;
				//Set IP to OLDIP to skip updating DNS Zone
				IP = OLDIP;
				ipv4 = false;
			}
			if(!ddnsd::is_ipv6_address(IP6)) {
				IP6 = OLDIP6;
				if(!ipv4) {
					std::cerr << "ERROR: Failed to get valid IPv6 Address" << std::endl;
				}
			}
			if (IP != OLDIP || IP6 != OLDIP6) {
				if(IP != OLDIP) {
					for(std::string tmpStr : zones) {
						ddnsd::updateip(tmpStr, OLDIP, IP, false);
					}
				}
				if(IP6 != OLDIP6) {
					for(std::string tmpStr : zones) {
						ddnsd::updateip(tmpStr, OLDIP6, IP6, true);
					}
				}
				for(std::string tmpStr : cmds) {
					char cmd[sizeof(tmpStr)];
					strncpy(cmd, tmpStr.c_str(), sizeof(tmpStr));
					system(cmd);
				}
			}
			break;
		}
	}
}
