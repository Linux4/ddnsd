#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sstream>
#include <boost/algorithm/string/erase.hpp>

namespace dns {

std::string get_serial(std::string domain) {
	unsigned char nsbuffer[NS_MAXDNAME];
	int msg_length;
	msg_length = res_query(domain.c_str(), ns_c_any, ns_t_soa, nsbuffer,
			sizeof(nsbuffer));
	if (msg_length < 0) {
		return "0";
	} else {
		ns_msg msg;
		ns_initparse(nsbuffer, msg_length, &msg);
		ns_rr rr;
		ns_parserr(&msg, ns_s_an, 0, &rr);
		char dispbuffer[NS_MAXDNAME];
		ns_sprintrr(&msg, &rr, NULL, NULL, dispbuffer, sizeof(dispbuffer));
		std::string full_record(dispbuffer);
		std::istringstream f(full_record);
		for (int i = 0; i < 2; i++) {
			std::string line;
			getline(f, line);
			if (i == 1) {
				boost::erase_all(line, " ");
				boost::erase_all(line, "\t");
				boost::erase_all(line, ";serial");
				return line;
			}
		}
		return "0";
	}
}
}
