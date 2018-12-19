#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <fstream>
#include <memory>
#include <iomanip>

namespace util {

inline bool replace(std::string &source, const std::string& from,
		const std::string& to) {
	if (!from.empty()) {
		size_t pos = source.find(from);

		if (pos != std::string::npos) {
			source.replace(pos, from.length(), to);
		}

	}
	return false;
}

void replace_all(std::string& source, const std::string& from,
		const std::string& to) {
	if (!from.empty()) {
		std::string new_string;
		new_string.reserve(source.length());  // avoids a few memory allocations

		std::string::size_type lastPos = 0;
		std::string::size_type findPos;

		while (std::string::npos != (findPos = source.find(from, lastPos))) {
			new_string.append(source, lastPos, findPos - lastPos);
			new_string += to;
			lastPos = findPos + from.length();
		}

		// Care for the rest after last occurrence
		new_string += source.substr(lastPos);

		source.swap(new_string);
	}
}

std::vector<std::string> split(std::string str, char token) {
	std::istringstream ss(str);
	std::vector<std::string> broken;
	std::string tmp;

	while (std::getline(ss, tmp, token)) {
		broken.push_back(tmp);
	}

	return broken;
}

bool bool_from_string(std::string str) {
	if (str == "1" || str == "true") {
		return true;
	} else {
		return false;
	}
}

inline std::string time(time_t rawtime, const std::string format) {
	std::stringstream buffer;
	struct std::tm * ptm = localtime(&rawtime);
	buffer << std::put_time(ptm, format.c_str());
	return buffer.str();
}

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
			replace(config_value, config_key, "");
			result = config_value;
			break;
		}
	}

	f.close();
	return result;
}

}
