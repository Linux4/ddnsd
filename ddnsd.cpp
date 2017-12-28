#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <arpa/inet.h>
#include <locale>
#include <string>
#include <ctime>
#include <sstream>
#include <unistd.h>
#include <boost/algorithm/string/replace.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>

bool is_ipv4_address(const std::string& str) {
	struct sockaddr_in sa;
	return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr))!=0;
}

struct Time
{
	Time( std::time_t tm, const std::string format )
	: m_tm( tm ), m_format( format )
	{}
	friend std::ostream& operator<<( std::ostream& out, const Time& t )
	{
		typedef std::ostreambuf_iterator< char > out_type;
		typedef std::time_put< char, out_type > time_put_facet_type;
		const time_put_facet_type& fac = std::use_facet< time_put_facet_type >( out.getloc() );
		const char* pattern = t.m_format.c_str();
		fac.put( out_type( out ), out, out.fill(), std::localtime( &t.m_tm ), pattern, pattern + t.m_format.size() );
		return out;
	}
private:
	std::time_t m_tm;
	std::string m_format;
};

inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

std::string shell_exec(std::string cmd) {
	std::array<char, 1024> buffer;
	std::string result;
	std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
	if (!pipe) {
		std::cerr << "ERROR executing " << cmd << std::endl;
		exit(1);
	}
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), 1024, pipe.get()) != nullptr) {
			result += buffer.data();
		}
	}
	return result;
}

int main(int argc, char** argv) {
	std::string version = "4.4.2";
        std::string remote_version = shell_exec("curl --silent https://raw.githubusercontent.com/Schmorzel/ddnsd/master/.version");
        boost::replace_all(remote_version, "\n", "");
        boost::replace_all(remote_version, "\r", "");
        if (remote_version != version) {
		if (remote_version.length() == 0) {
			std::cout << "Could not check for updates!" std::endl;
		}
		else {
                	std::cout << "Update to version " << remote_version << " available!" << std::endl;
                	std::cout << "To update clone the following Git Repository: https://github.com/Schmorzel/ddnsd" << std::endl;
		}
        }

	if (argc > 1) {
		if (std::string(argv[1]) == "-version" || std::string(argv[1]) == "--version") {
			std::cout << "DDNSD v4.4.2 (15.12.2017)" << std::endl;
			exit(0);
		}
	}
	//Read config
	std::fstream f;
	f.open("/etc/ddns/enabled", std::fstream::in );
	std::string enabled;
	getline( f, enabled, '\0');
	f.close();
	boost::replace_all(enabled, "\n", "");
	if (enabled == "0") {
		std::cout << "/etc/ddns/enabled is set to 0... Stopping service..." << std::endl;
		exit(0);
	}
	f.open("/etc/ddns/update_freq", std::fstream::in );
	std::string update_freq_string;
	getline( f, update_freq_string, '\0');
	f.close();
	boost::replace_all(update_freq_string, "\n", "");
	f.open("/etc/ddns/zone_name", std::fstream::in );
	std::string zone_name;
	getline( f, zone_name, '\0');
	f.close();
	boost::replace_all(zone_name, "\n", "");
	f.open("/etc/ddns/zone_path", std::fstream::in );
	std::string zone_path;
	getline( f, zone_path, '\0');
	f.close();
	boost::replace_all(zone_path, "\n", "");
	f.open("/etc/ddns/post_update_cmds");
	std::string cmds_string;
	getline ( f, cmds_string, '\0');
	f.close();
	char cmds[1024];
	strncpy(cmds, cmds_string.c_str(), sizeof(cmds));
	f.open("/etc/ddns/.config_created.ddns", std::fstream::in );
	std::string config_created;
	getline( f, config_created, '\0');
	f.close();
	f.open("/run/ddnsd.pid", std::ios::out );
	f << getpid();
	f << "\n";
	f.close();
	//Check if config exists, if not create config files
	if (config_created != "1") {
		std::cout << "It looks like the service is started first time, creating configuration files..." << std::endl;
		system("mkdir -p /etc/ddns && echo 1 >/etc/ddns/enabled && echo 60 >/etc/ddns/update_freq && echo Example.com >/etc/ddns/zone_name && echo /etc/bind/db.example.com >/etc/ddns/zone_path && echo 'service bind9 restart' >/etc/ddns/post_update_cmds");
		system("curl --silent https://v4.ident.me >/etc/ddns/.oldip.ddns");
		std::cout << "Config files created." << std::endl;
		std::cout << "Use \"ddnsd-config\" for configuration:" << std::endl;
		std::cout << "Help for DDNSD-Config: \"ddnsd-config -h\"" << std::endl;
		std::cout << "Stopping service, after configuration type \"service ddnsd start\" to start the service." << std::endl;
		f.open("/etc/ddns/.config_created.ddns", std::ios::out );
		f << "1";
		f.close();
		exit(1);
	}
	//Check if given DNS Zone file exists
	if(!(file_exists(zone_path))) {
                std::cerr << "ERROR: The given DNS Zone file (" << zone_path << ") does not exist!" << std::endl;
                exit(1);
        }
        int update_freq = atoi(update_freq_string.c_str());
	//Check if update frequency is a number and greater 0
        if(update_freq == 0) {
                std::cerr << "ERROR: The given update frequency (" << update_freq_string << ") is not a valid number!" << std::endl;
                std::cerr << "The number has to be greater than 0!" << std::endl;
                exit(1);
        }
	while(true) {
		//Wait as long as update_freq
		std::this_thread::sleep_for (std::chrono::seconds(update_freq));
		//Read last known IP-Adress
		f.open("/etc/ddns/.oldip.ddns", std::fstream::in );
		std::string OLDIP;
		getline( f, OLDIP, '\0');
		f.close();
		//Get actual IP-Adress
		std::string IP = shell_exec("curl --silent https://v4.ident.me/");
		//Check if IP is a valid IP-Adress
		//e.g if no internet connection is available
		if (!is_ipv4_address(IP)) {
			std::cerr << "ERROR: Failed to get valid IPv4-Adress" << std::endl;
			//Set IP to OLDIP to skip updating DNS Zone
			IP = OLDIP;
		}
		if (IP != OLDIP) {
			//Get last DNS Update
			f.open("/etc/ddns/.serial_old.ddns", std::fstream::in );
			std::string date_old;
			getline( f, date_old, '\0');
			f.close();
			//Write actual time
			std::time_t tt = time(0);
			f.open("/etc/ddns/.date.ddns", std::ios::out);
			f << Time( tt, "%Y%m%d0" );
			f.close();
			//Read actual time
			f.open("/etc/ddns/.date.ddns", std::fstream::in );
			std::string date;
			getline( f, date, '\0');
			f.close();
			//Check if DNS Zone already was updated at the same day
			if (date_old.substr(0, 8) != date.substr(0, 8)) {
				//If not set DNS Zone Version to 0
				f.open("/etc/ddns/.date_old.ddns", std::ios::out);
				f << Time( tt, "%Y%m%d0" );
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
				f << Time( tt, "%Y%m%d" );
				f.close();
				f.open("/etc/ddns/.10-date.ddns", std::fstream::in );
				std::string date;
				getline( f, date, '\0');
				f.close();
			}
			//Add Version to date to create DNS Zone Serial
			std::string serial = date+version;
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
			f.open("/etc/ddns/.oldip.ddns", std::ios::out);
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
			system(cmds);
			std::cout << "DNS Zone " << zone_name <<  " was updated to Serial " << serial << " !" << std::endl;
		}
	}
}
