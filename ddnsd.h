#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <thread>
#include <locale>
#include <string>
#include <ctime>
#include <sstream>
#include <unistd.h>
#include <boost/algorithm/string/replace.hpp>
#include <array>
#include <cstdio>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <arpa/inet.h>
#include "utils.h"

namespace ddnsd {
	bool is_ipv4_address(const std::string& str) {
		struct sockaddr_in sa;
		return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)) != 0;
	}

	bool is_ipv6_address(const std::string& str) {
		struct sockaddr_in6 sa;
		return inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr)) != 0;
	}

	void updateip(std::string zone, std::string OLDIP, std::string IP, bool ipv6) {
		std::vector<std::string> zone_name_path;
		boost::split(zone_name_path, zone, boost::is_any_of(":"));
		int zonesize = zone_name_path.size();
		std::string zone_path;
		std::string zone_name;
		if(zonesize == 2) {
			zone_name = zone_name_path.at(0);
			zone_path = zone_name_path.at(1);
		} else {
			zone_path = zone;
			zone_name = zone;
			boost::replace_all(zone_name, "/etc/bind/db.", "");
		}
		//Check if given DNS Zone file exists
		if(!(utils::file_exists(zone_path))) {
	                std::cerr << "ERROR: The given DNS Zone file (" << zone_path << ") does not exist!" << std::endl;
	                return;
	        }
		std::fstream f;
		//Get actual serial + zone version
		f.open("/etc/ddns/.serial_old.ddns", std::ios::out);
		std::string serial_dig = utils::shell_exec("dig +short @localhost "+zone_name+" SOA | awk '{print $3}'");
		boost::replace_all(serial_dig, "\n", "");
		boost::replace_all(serial_dig, "\r", "");
	        f << serial_dig;
	        f.close();
	        std::string date_dig = serial_dig.substr(0, serial_dig.length() - 2);
		date_dig = date_dig + "0";
	        f.open("/etc/ddns/.date_old.ddns", std::ios::out);
	        f << date_dig;
	        f.close();
		std::string version_dig = serial_dig;
		boost::replace_all(version_dig, date_dig, "");
		if(version_dig.substr(0, 1) == "0") {
			boost::replace_all(version_dig, "0", "");
		}
		f.open("/etc/ddns/.version.ddns", std::ios::out);
		f << version_dig;
		f.close();
		//Get last DNS Update
		f.open("/etc/ddns/.date_old.ddns", std::fstream::in );
		std::string date_old;
		getline( f, date_old, '\0');
		f.close();
		//Write actual time
		std::time_t tt = time(0);
		f.open("/etc/ddns/.date.ddns", std::ios::out);
		f << utils::Time( tt, "%Y%m%d0" );
		f.close();
		//Read actual time
		f.open("/etc/ddns/.date.ddns", std::fstream::in );
		std::string date;
		getline( f, date, '\0');
		f.close();
		//Check if DNS Zone already was updated at the same day
		if (date_old != date) {
			//If not set DNS Zone Version to 0
			f.open("/etc/ddns/.date_old.ddns", std::ios::out);
			f << utils::Time( tt, "%Y%m%d0" );
			f.close();
			int zeit = 0;
			f.open("/etc/ddns/.version.ddns", std::ios::out);
			f << zeit;
			f.close();
	   	}
		//Read DNS Zone Version
		f.open("/etc/ddns/.version.ddns", std::fstream::in );
		std::string version_str;
		getline( f, version_str, '\0');
		f.close();
		//Convert DNS Zone Version to Integer, add 1 and convert it back to string
		int version_int = atoi(version_str.c_str());
		version_int = version_int + 1;
		std::stringstream versionss;
		versionss << version_int;
		std::string version = versionss.str();
		f.open("/etc/ddns/.version.ddns", std::ios::out);
		f << version;
		f.close();
		//Check if there were 10 or more updates at the same day
		if (version_int >= 10) {
			//If yes generate date without 0 as placeholder at the end
			std::time_t tt = time(0);
			f.open("/etc/ddns/.10-date.ddns", std::ios::out);
			f << utils::Time( tt, "%Y%m%d" );
			f.close();
			f.open("/etc/ddns/.10-date.ddns", std::fstream::in );
			std::string date;
			getline( f, date, '\0');
			f.close();
		}
		//Add Version to date to create DNS Zone Serial
		std::string serial = date + version;
		//Read actual DNS Zone Serial
		f.open("/etc/ddns/.serial_old.ddns", std::fstream::in );
		std::string serial_old;
		getline( f, serial_old, '\0');
		f.close();
		//Write new Serial to file
		f.open("/etc/ddns/.serial_old.ddns", std::ios::out);
		f << serial;
		f.close();
		//Write new IP to file
		if(!ipv6) {
			f.open("/etc/ddns/.oldip.ddns", std::ios::out);
		} else {
			f.open("/etc/ddns/.oldip6.ddns", std::ios::out);
		}
		f << IP;
		f.close();
		//Read DNS Zone file
		f.open(zone_path, std::fstream::in );
		std::string dnszone;
		getline( f, dnszone, '\0');
		f.close();
		//Replace in DNS Zone: OLDIP with IP and serial_old with serial
		std::string n = "\n";
		boost::replace_all(IP, "\n", "");
		boost::replace_all(IP, "\r", "");
		boost::replace_all(OLDIP, "\n", "");
		boost::replace_all(dnszone, OLDIP, IP);
		boost::replace_all(serial_old, "\n", "");
		boost::replace_all(serial, "\n", "");
		boost::replace_all(dnszone, serial_old, serial);
		//Write DNS Zone to file
		f.open(zone_path, std::ios::out);
		f << dnszone;
		f.close();
		//Run post update commands
		//system(cmds);
		std::cout << "DNS Zone " << zone_name <<  " was updated to Serial " << serial << " !" << std::endl;
	}
}

