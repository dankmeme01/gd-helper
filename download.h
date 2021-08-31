#pragma once
#define CURL_STATICLIB
#include <curl/curl.h>
#include <fstream>
#include "files.h"
#include "errors.h"
#include <iostream>
CURL* gCurl;

void SetCurl(CURL* curl) {
	gCurl = curl;
}

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void DownloadFile(std::string url, std::string name, fs::path savePath = fs::temp_directory_path()) {
	if (!fs::exists(savePath)) throw FileNotFoundError(savePath.string());
	auto save = savePath / name;
	std::cout << "Downloading " << url << " to " << save.string() << std::endl;

	if (gCurl) {
		FILE* fp;
		fopen_s(&fp, save.string().c_str(), "wb");
		curl_easy_setopt(gCurl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(gCurl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(gCurl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(gCurl, CURLOPT_WRITEDATA, fp);
		CURLcode res = curl_easy_perform(gCurl);
		curl_easy_cleanup(gCurl);
		if(fp) fclose(fp);

		if (res != CURLE_OK) throw DownloadError(curl_easy_strerror(res));
	} else {
		throw DownloadError("cURL instance is undefined");
	}
}