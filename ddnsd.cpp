#include "ddnsd.h"

#define BUILD_DATE util::Time(time(0), "%Y-%m-%d")
#define VERSION "v5.3.0"

int main(int argc, char** argv) {
	std::fstream f;
	std::string config = "/etc/ddns/ddnsd.conf";
	std::string update_checker = util::read_config(config, "update_checker = ");
	if (update_checker == "true") {
		std::string remote_version =
				www::get_content(
						"https://raw.githubusercontent.com/Server24-7/ddnsd/master/.version");
		boost::replace_all(remote_version, "\n", "");
		boost::replace_all(remote_version, "\r", "");
		if (remote_version != VERSION) {
			if (remote_version.length() == 0) {
				std::cout << "Could not check for updates!" << std::endl;
			} else {
				std::cout << "Update to version " << remote_version
						<< " available!" << std::endl;
				std::cout
						<< "To update clone the following Git Repository: https://github.com/Server24-7/ddnsd"
						<< std::endl;
				std::cout
						<< "Or use the APT Repository http://deb.server24-7.eu/"
						<< std::endl;
			}
		}
	}
	if (argc > 1) {
		if (std::string(argv[1]) == "-version"
				|| std::string(argv[1]) == "--version") {
			std::cout << "DDNSD " << VERSION << " " << BUILD_DATE<< " (g++ version " << __VERSION__ << ")" << std::endl;
			exit(0);
		} else if(std::string(argv[1]) == "-firstrun" || std::string(argv[1]) == "--firstrun") {
			f.open("/etc/ddns/.oldip.ddns", std::ios::out);
			f << www::get_content("https://v4.ident.me");
			f.close();
			f.open("/etc/ddns/.oldip6.ddns", std::ios::out);
			f << www::get_content("https://v6.ident.me");
			f.close();
			exit(0);
		}
	}

	//Read config
	std::string enabled;
	enabled = util::read_config(config, "enabled = ");
	if (enabled == "false") {
		std::cout
				<< "/etc/ddns/ddnsd.conf: enabled is set to false... Stopping service..."
				<< std::endl;
		exit(0);
	}
	std::string update_freq_string;
	update_freq_string = util::read_config(config, "update_freq = ");
	std::string zones_string;
	zones_string = util::read_config(config, "zones = ");
	std::vector<std::string> zones;
	boost::split(zones, zones_string, boost::is_any_of(","));
	std::string cmds_string;
	cmds_string = util::read_config(config, "post_update_cmds = ");
	std::vector<std::string> cmds;
	boost::split(cmds, cmds_string, boost::is_any_of(","));
	std::string config_version;
	config_version = util::read_config(config, "config_version = ");
	std::string use_puckdns;
	use_puckdns = util::read_config(config, "use_puckdns = ");
	std::string puckdns_username;
	puckdns_username = util::read_config(config, "puckdns_username = ");
	std::string puckdns_password;
	puckdns_password = util::read_config(config, "puckdns_password = ");
	std::string puckdns_type = util::read_config(config, "puckdns_type = ");
	std::string puckdns_domains_string;
	puckdns_domains_string = util::read_config(config, "puckdns_domains = ");
	std::vector<std::string> puckdns_domains;
	boost::split(puckdns_domains, puckdns_domains_string,
			boost::is_any_of(","));

	f.open("/run/ddnsd.pid", std::ios::out);
	f << getpid();
	f << "\n";
	f.close();

	//Check if config exists, if not create config files
	if (config_version.length() == 0) {
		std::cout
				<< "It looks like the service is started first time, creating configuration files..."
				<< std::endl;
		mkdir("/etc/ddns", 640);
		f.open("/etc/ddns/ddnsd.conf", std::ios::out);
		f << "#DDNSD Configuration" << std::endl << std::endl
				<< "#Enable (true)/Disable (false) the service:" << std::endl
				<< "enabled = true" << std::endl << std::endl
				<< "#IP-Address update frequency:" << std::endl << std::endl
				<< "update_freq = 60" << std::endl << std::endl
				<< "#Domain Name and Path to DNS zone files (format: yourdomain.com:/etc/bind/db.yourdomain.com) separated by comma:"
				<< std::endl
				<< "zones = example.com:/etc/bind/db.example.com,example2.com:/etc/bind/db.example2.com"
				<< std::endl << std::endl
				<< "#Commands that will be executed after DNS zone update separated by comma:"
				<< std::endl
				<< "post_update_cmds = service bind9 restart,custom_cmd,custom_cmd2"
				<< std::endl << std::endl
				<< "#Enable (true)/Disable (false) the Update Checker:"
				<< std::endl << "update_checker = true" << std::endl
				<< std::endl << std::endl
				<< "#Puck.Nether.net Secondary DNS section:" << std::endl
				<< std::endl << "#Enable puck.nether.net Master-IP Updater:"
				<< std::endl << "use_puckdns = false" << std::endl << std::endl
				<< "#Username and password:" << std::endl
				<< "puckdns_username = user" << std::endl
				<< "puckdns_password = password" << std::endl << std::endl
				<< "#IP type (0 = auto, 1 = ipv4, 2 = ipv6):" << std::endl
				<< "puckdns_type = 0" << std::endl << std::endl
				<< "#Domains to update separated by comma:" << std::endl
				<< "puckdns_domains = example.com" << std::endl << std::endl
				<< std::endl << "#Do not touch:\nconfig_version = 3"
				<< std::endl;
		f.close();
		f.open("/etc/ddns/.oldip.ddns", std::ios::out);
		f << www::get_content("https://v4.ident.me");
		f.close();
		f.open("/etc/ddns/.oldip6.ddns", std::ios::out);
		f << www::get_content("https://v6.ident.me");
		f.close();
		std::cout << "Config file created." << std::endl;
		std::cout << "Please edit /etc/ddns/ddnsd.conf" << std::endl;
		std::cout
				<< "Stopping service, after configuration type \"service ddnsd start\" to start the service."
				<< std::endl;
		exit(1);
	}

	int update_freq = atoi(update_freq_string.c_str());
	update_freq_string.clear();

	//Check if update frequency is a number and greater 0
	if (update_freq == 0) {
		std::cerr << "ERROR: The given update frequency (" << update_freq_string
				<< ") is not a valid number!" << std::endl;
		std::cerr << "The number has to be greater than 0!" << std::endl;
		exit(1);
	}

	while (true) {
		//Wait as long as update_freq
		std::this_thread::sleep_for(std::chrono::seconds(update_freq));
		{
			//Read last known IP-Adress
			f.open("/etc/ddns/.oldip.ddns", std::fstream::in);
			std::string OLDIP;
			getline(f, OLDIP, '\0');
			f.close();
			f.open("/etc/ddns/.oldip6.ddns", std::fstream::in);
			std::string OLDIP6;
			getline(f, OLDIP6, '\0');
			f.close();

			//Get current IP-Adress
			std::string IP = www::get_content("https://v4.ident.me/");
			std::string IP6 = www::get_content("https://v6.ident.me/");

			//Check if IP is a valid IP-Adress
			//e.g if no internet connection is available
			bool ipv4 = true;
			if (!ddnsd::is_ipv4_address(IP)) {
				std::cerr << "ERROR: Failed to get valid IPv4-Address"
						<< std::endl;
				//Set IP to OLDIP to skip updating DNS Zone
				IP = OLDIP;
				ipv4 = false;
			}

			bool ipv6 = true;
			if (!ddnsd::is_ipv6_address(IP6)) {
				IP6 = OLDIP6;
				ipv6 = false;
				if (!ipv4) {
					std::cerr << "ERROR: Failed to get valid IPv6 Address"
							<< std::endl;
				}
			}

			if (IP != OLDIP || IP6 != OLDIP6) {
				if (use_puckdns == "true") {
					int type = atoi(puckdns_type.c_str());
					std::cout << "Starting PDNS updater (Mode: " << type << ")"
							<< std::endl;
					if (type == 0) {
						if (ipv6) {
							type = 2;
						} else {
							type = 1;
						}
					}

					std::string masterip;

					if (type == 1) {
						masterip = IP;
					} else {
						masterip = IP6;
					}

					CURL* curl = www::login(
							"https://puck.nether.net/dns/login?username="
									+ puckdns_username + "&password="
									+ puckdns_password);

					if (www::get_status(curl) == 302) {
						std::cout << "PDNS: Setting Master-IPs of domains to "
								<< masterip << " :" << std::endl;

						for (std::string domain : puckdns_domains) {
							if (www::access_logged_in(
									"https://puck.nether.net/dns/dnsinfo/edit/"
											+ domain + "?domainname=" + domain
											+ "&masterip=" + masterip
											+ "&aa=Y&submit=Submit", curl)
									== 302) {
								std::cout
										<< "PDNS updater: Successfully updated "
										<< domain << std::endl;
							} else {
								std::cout << "PDNS updater: FAILED UPDATING "
										<< domain << std::endl;
							}
						}

						curl_easy_cleanup(curl);

					} else {
						std::cout << "PDNS updater: LOGIN FAILED!" << std::endl;
					}

				}

				if (IP != OLDIP) {
					for (std::string tmpStr : zones) {
						ddnsd::updateip(tmpStr, OLDIP, IP, false);
					}
				}

				if (IP6 != OLDIP6) {
					for (std::string tmpStr : zones) {
						ddnsd::updateip(tmpStr, OLDIP6, IP6, true);
					}
				}

				// Execute Post-update cmds
				for (std::string tmpStr : cmds) {
					char cmd[sizeof(tmpStr)];
					strncpy(cmd, tmpStr.c_str(), sizeof(tmpStr));
					system(cmd);
				}
			}
		}
	}
}
