#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <boost/algorithm/string/replace.hpp>
#include <fstream>
#include <memory>

namespace util {
struct Time {
	Time(std::time_t tm, const std::string format) :
			m_tm(tm), m_format(format) {
	}
	friend std::ostream& operator<<(std::ostream& out, const Time& t) {
		typedef std::ostreambuf_iterator<char> out_type;
		typedef std::time_put<char, out_type> time_put_facet_type;
		const time_put_facet_type& fac = std::use_facet<time_put_facet_type>(
				out.getloc());
		const char* pattern = t.m_format.c_str();
		fac.put(out_type(out), out, out.fill(), std::localtime(&t.m_tm),
				pattern, pattern + t.m_format.size());
		return out;
	}
private:
	std::time_t m_tm;
	std::string m_format;
};

inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
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

std::string read_config(std::string file_path, std::string config_key) {
	std::fstream f;
	std::string result;
	std::string config_value;
	f.open(file_path, std::fstream::in);
	int length = config_key.length();
	while (getline(f, config_value)) {
		if (config_value.substr(0, length) == config_key) {
			boost::replace_all(config_value, config_key, "");
			result = config_value;
			break;
		}
	}
	f.close();
	return result;
}
}
