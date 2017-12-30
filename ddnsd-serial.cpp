#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <boost/algorithm/string/replace.hpp>

int main(int argc, char** argv) {
	std::fstream f;
	int dont_showhelp = 0;
	if (argc > 1) {
		if (std::string(argv[1]) == "-h") {
			std::cout << argv[0] << " -h -- Shows the help." << std::endl;
			std::cout << argv[0] << " -gs -- Shows the DNS Zone Serial." << std::endl;
			std::cout << argv[0] << " -ss Serial -- Sets the DNS Zone Serial (Format: YYYYMMDDVV)." << std::endl;
			std::cout << argv[0] << " -v -- Shows DDNSD-Config Version." << std::endl;
			dont_showhelp = 1;
		}
		if (std::string(argv[1]) == "-gs") {
			f.open("/etc/ddns/.serial_old.ddns", std::fstream::in);
			std::string serial;
			getline(f, serial, '\0');
			f.close();
			boost::replace_all(serial, "\n", "");
			std::cout << "Current serial: " << serial << std::endl;
			dont_showhelp = 1;
		}
		if (std::string(argv[1]) == "-ss") {
			if (argc < 3) {
				std::cerr << "ERROR: You have to enter a serial!" << std::endl;
				exit(1);
			}
			f.open("/etc/ddns/.serial_old.ddns", std::ios::out);
			std::string serial = std::string(argv[2]);
			f << serial;
			f.close();
			std::string date = serial.substr(0, (serial.length() -2));
			f.open("/etc/ddns/.date_old.ddns", std::ios::out);
			f << date;
			f.close();
			std::cout << "New serial: " << serial << "." << std::endl;
			dont_showhelp = 1;
		}
		if (std::string(argv[1]) == "-v") {
			std::cout << "DDNSD-Config v1.0 (30.12.2017)" << std::endl;
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
