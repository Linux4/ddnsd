#include <iostream>
#include <string>
#include <curl/curl.h>

namespace www {

	static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	std::string get_content(std::string url) {
		CURL *curl;
		std::string read_buffer;
		curl = curl_easy_init();
		if(curl) {
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
}
