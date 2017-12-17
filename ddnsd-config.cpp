#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <boost/algorithm/string/replace.hpp>

int main(int argc, char** argv) {
	int dont_showhelp = 0;
	if (argc > 1) {
		if (std::string(argv[1]) == "-h") {
			std::cout << argv[0] << " -h -- Shows the help." << std::endl;
			std::cout << argv[0] << " -ge -- Shows if the service is enabled (1) or disabled (0)." << std::endl;
			std::cout << argv[0] << " -se 1|0 -- Enables (1) or disables (0) the service." << std::endl; 
			std::cout << argv[0] << " -gs -- Shows the DNS Zone Serial." << std::endl;
			std::cout << argv[0] << " -ss Serial -- Sets the DNS Zone Serial (Format: YYYYMMDDVV)." << std::endl;
			std::cout << argv[0] << " -gu -- Shows the IP-Adress update frequency." << std::endl;
			std::cout << argv[0] << " -su Time -- Sets the IP-Adress update frequency (in seconds)." << std::endl;
			std::cout << argv[0] << " -gzn -- Shows the DNS Zone Name." << std::endl;
			std::cout << argv[0] << " -szn Zonenname -- Sets the DNS Zone Name." << std::endl;
			std::cout << argv[0] << " -gzp -- Shows the path to DNS Zone file." << std::endl;
			std::cout << argv[0] << " -szp /path/to/dnszone -- Sets the path to DNS Zone file." << std::endl;
			std::cout << argv[0] << " -gp -- Shows commands executed after DNS update." << std::endl;
			std::cout << argv[0] << " -sp -- Opens a prompt to enter commands executed after DNS update." << std::endl; 
			std::cout << argv[0] << " -v -- Shows DDNSD-Config Version." << std::endl;
			dont_showhelp = 1;
		}
		if (std::string(argv[1]) == "-ge") {
			std::fstream enabled_read;
			enabled_read.open("/etc/ddns/enabled", std::fstream::in);
			std::string enabled;
			getline( enabled_read, enabled, '\0');
			boost::replace_all(enabled, "\n", "");
			std::cout << "Aktiviert (1)/Deaktiviert (0): " << enabled << std::endl;
			dont_showhelp = 1;
		}
                if (std::string(argv[1]) == "-se") {
                        if (argc < 3) {
                                std::cerr << "ERROR: You have to enter 1 or 0!" << std::endl;
                                exit(1);
                        }
                        std::fstream enabled_write;
                        enabled_write.open("/etc/ddns/enabled", std::ios::out);
                        std::string enabled = std::string(argv[2]);
                        enabled_write << enabled;
                        enabled_write.close();
                        std::cout << "Set enabled/disabled to " << enabled << " ." << std::endl;
                        dont_showhelp = 1;
                }
		if (std::string(argv[1]) == "-gs") {
			std::fstream zeitstempel_read;
			zeitstempel_read.open("/etc/ddns/.serial_old.ddns", std::fstream::in);
			std::string serial;
			getline( zeitstempel_read, serial, '\0');
			boost::replace_all(serial, "\n", "");
			std::cout << "Serial: " << serial << std::endl;
			dont_showhelp = 1;
		}
		if (std::string(argv[1]) == "-ss") {
			if (argc < 3) {
				std::cerr << "ERROR: You have to enter a serial!" << std::endl;
				exit(1);
			}
			std::fstream zeitstempel_write;
			zeitstempel_write.open("/etc/ddns/.serial_old.ddns", std::ios::out);
			std::string serial = std::string(argv[2]);
			zeitstempel_write << serial;
			zeitstempel_write.close();
			std::string date = serial.substr(0, 8);
			zeitstempel_write.open("/etc/ddns/.date_old.ddns", std::ios::out);
			zeitstempel_write << date;
			zeitstempel_write.close();
			std::cout << "Set serial to " << serial << " ." << std::endl;
			dont_showhelp = 1;
		}
		if (std::string(argv[1]) == "-gu") {
                        std::fstream update_freq_read;
                        update_freq_read.open("/etc/ddns/update_freq", std::fstream::in);
                        std::string update_freq;
                        getline( update_freq_read, update_freq, '\0');
			boost::replace_all(update_freq, "\n", "");
                        std::cout << "IP-Adress update frequency: " << update_freq << std::endl;
                        dont_showhelp = 1;
                }
                if (std::string(argv[1]) == "-su") {
                        if (argc < 3) {
                                std::cerr << "ERROR: You have to enter an update frequency!" << std::endl;
                                exit(1);
                        }
                        std::fstream update_freq_write;
                        update_freq_write.open("/etc/ddns/update_freq", std::ios::out);
                        std::string update_freq = std::string(argv[2]);
                        update_freq_write << update_freq;
                        update_freq_write.close();
                        std::cout << "Set IP-Adress update frequency to " << update_freq << " ." << std::endl;
                        dont_showhelp = 1;
                }
                if (std::string(argv[1]) == "-gzn") {
                        std::fstream zone_name_read;
                        zone_name_read.open("/etc/ddns/zone_name", std::fstream::in);
                        std::string zone_name;
                        getline( zone_name_read, zone_name, '\0');
			boost::replace_all(zone_name, "\n", "");
                        std::cout << "DNS Zone Name: " << zone_name << std::endl;
                        dont_showhelp = 1;
                }
                if (std::string(argv[1]) == "-szn") {
                        if (argc < 3) {
                                std::cerr << "ERROR: You have to enter a DNS Zone Name!" << std::endl;
                                exit(1);
                        }
                        std::fstream zone_name_write;
                        zone_name_write.open("/etc/ddns/zone_name", std::ios::out);
                        std::string zone_name = std::string(argv[2]);
                        zone_name_write << zone_name;
                        zone_name_write.close();
                        std::cout << "Set DNS Zone Name to " << zone_name << " ." << std::endl;
                        dont_showhelp = 1;
                }
                if (std::string(argv[1]) == "-gzp") {
                        std::fstream zone_path_read;
                        zone_path_read.open("/etc/ddns/zone_path", std::fstream::in);
                        std::string zone_path;
                        getline( zone_path_read, zone_path, '\0');
			boost::replace_all(zone_path, "\n", "");
                        std::cout << "Path to DNS Zone file: " << zone_path << std::endl;
                        dont_showhelp = 1;
                }
                if (std::string(argv[1]) == "-szp") {
                        if (argc < 3) {
                                std::cerr << "ERROR: You have to enter a file path!" << std::endl;
                                exit(1);
                        }
                        std::fstream zone_path_write;
                        zone_path_write.open("/etc/ddns/zone_path", std::ios::out);
                        std::string zone_path = std::string(argv[2]);
                        zone_path_write << zone_path;
                        zone_path_write.close();
                        std::cout << "Set path to DNS Zone file to " << zone_path << " ." << std::endl;
                        dont_showhelp = 1;
                }
                if (std::string(argv[1]) == "-gp") {
                        std::fstream post_update_cmds_read;
                        post_update_cmds_read.open("/etc/ddns/post_update_cmds", std::fstream::in);
                        std::string post_update_cmds;
                        getline( post_update_cmds_read, post_update_cmds, '\0');
			boost::replace_all(post_update_cmds, "\n", "");
                        std::cout << "Commands that are executed after DNS Update: " << post_update_cmds << std::endl;
                        dont_showhelp = 1;
                }
                if (std::string(argv[1]) == "-sp") {
                        std::fstream post_update_cmds_write;
                        post_update_cmds_write.open("/etc/ddns/post_update_cmds", std::ios::out);
                        char post_update_cmds_char[1024];
			std::cout << "Enter commands to execute after DNS Update separted by && (Befehl1 && Befehl2 && ..) (Maximum 1024 characters!) :" << std::endl;
			std::cin.getline(post_update_cmds_char, sizeof(post_update_cmds_char));
                        post_update_cmds_write << std::string(post_update_cmds_char);
                        post_update_cmds_write.close();
                        std::cout << "Set commands to execute after DNS Update to " << post_update_cmds_char << " ." << std::endl;
                        dont_showhelp = 1;
                }
		if (std::string(argv[1]) == "-v") {
			std::cout << "DDNSD-Config v4.4.2 (15.12.2017)" << std::endl;
			dont_showhelp = 1;
			}
		else {
			if (dont_showhelp != 1) {
				std::cout << argv[0] << " -h -- Shows the help." << std::endl;
			}
		}
	}
	else {
		std::cout << argv[0] << " -h -- Shows the help." << std::endl;
	}
}
