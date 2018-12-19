#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <thread>
#include <locale>
#include <string>
#include <ctime>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <array>
#include <cstdio>
#include <vector>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "util.h"
#include "dns.h"
#include "www.h"

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
	zone_name_path = util::split(zone, ':');
	int zonesize = zone_name_path.size();
	std::string zone_path;
	std::string zone_name;

	if (zonesize == 2) {
		zone_name = zone_name_path.at(0);
		zone_path = zone_name_path.at(1);
	} else {
		zone_path = zone;
		zone_name = zone;
		util::replace(zone_name, "/etc/bind/db.", "");
	}

	//Check if given DNS Zone file exists
	if (!(util::file_exists(zone_path))) {
		std::cerr << "ERROR: The given DNS Zone file (" << zone_path
				<< ") does not exist!" << std::endl;
		return;
	}

	std::fstream f;
	//Get actual serial + zone version
	std::string serial_dig = dns::get_serial(zone_name);
	std::string date_dig = serial_dig.substr(0, serial_dig.length() - 2);
	date_dig = date_dig + "0";
	std::string version_dig = serial_dig;
	util::replace(version_dig, date_dig, "");
	int version = atoi(version_dig.c_str());

	//Get actual time
	std::string date = util::time(time(0), "%Y%m%d0");

	//Check if DNS Zone already was updated at the same day
	if (date_dig != date) {
		//If not set DNS Zone Version to 0
		version = 0;
	}

	//Add 1 to DNS Zone Versiong
	version++;

	//Check if there were 10 or more updates at the same day
	if (version >= 10) {
		//If yes remove 0 at the end of date
		date.substr(0, date.length() - 1);
	}

	//Add Version to date to create DNS Zone Serial
	std::stringstream ss;
	ss << version;
	std::string serial = date + ss.str();

	//Read DNS Zone file
	f.open(zone_path, std::fstream::in);
	std::string dnszone;
	getline(f, dnszone, '\0');
	f.close();

	//Replace in DNS Zone: OLDIP with IP and serial_old with serial
	util::replace_all(dnszone, OLDIP, IP);
	util::replace(dnszone, serial_dig, serial);

	//Write DNS Zone to file
	f.open(zone_path, std::ios::out);
	f << dnszone;
	f.close();

	//Write new IP to file
	if (!ipv6) {
		f.open("/etc/ddns/.oldip.ddns", std::ios::out);
	} else {
		f.open("/etc/ddns/.oldip6.ddns", std::ios::out);
	}

	f << IP;
	f.close();

	std::cout << "DNS Zone " << zone_name << " was updated to Serial " << serial
			<< " !" << std::endl;
}
}

