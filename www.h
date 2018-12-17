#include <iostream>
#include <string>
#include <curl/curl.h>

namespace www {

static size_t write_callback(void *contents, size_t size, size_t nmemb,
		void *userp) {
	((std::string*) userp)->append((char*) contents, size * nmemb);
	return size * nmemb;
}

std::string get_content(std::string url) {
	CURL *curl;
	std::string read_buffer;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, www::write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return read_buffer;
	}
	return "";
}

CURL* login(std::string url) {
	CURL *curl;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	std::string read_buffer;
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/.cookies.ddns");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/tmp/.cookies.ddns");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, www::write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
		curl_easy_perform(curl);
	}
	return curl;
}

long get_status(CURL* curl) {
	long res;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
	return res;
}

long access_logged_in(std::string url, CURL* curl) {
	std::string read_buffer;
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/.cookies.ddns");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/tmp/.cookies.ddns");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, www::write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
		curl_easy_perform(curl);
		long res;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
		return res;
	}
	return -1;
}

}
