#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>
#include <chrono>
#include <curl/curl.h>

// Struct to hold the website content and any issues
struct WebsiteData {
	std::string content;
	std::string issue;
};

// Callback function to receive website data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, WebsiteData* data) {
	size_t totalSize = size * nmemb;
	data->content.append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

// Function to check if a string contains a keyword
bool containsKeyword(const std::string& text, const std::unordered_set<std::string>& keywords) {
	for (const std::string& keyword : keywords) {
		if (std::regex_search(text, std::regex("\\b" + keyword + "\\b", std::regex_constants::icase))) {
			return true;
		}
	}
	return false;
}

// Function to fetch website content and handle issues
WebsiteData fetchWebsiteContent(const std::string& website) {
		WebsiteData websiteData;
		CURL* curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, website.c_str());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &websiteData);

			CURLcode res = curl_easy_perform(curl);

			if (res != CURLE_OK) {
				websiteData.issue = curl_easy_strerror(res);
			}

			curl_easy_cleanup(curl);
		}
		else {
			websiteData.issue = "Failed to initialize CURL.";
		}
		return websiteData;
}			

int main() {
	std::string line;

	// Load keywords from CSV files
	std::unordered_set<std::string> techOfferings;
	std::ifstream techOfferingsFile("Tech_Offerings.csv");
	while (std::getline(techOfferingsFile, line)) {
		techOfferings.insert(line);
	}
	techOfferingsFile.close();

	std::unordered_set<std::string> sectors;
	std::ifstream sectorsFile("Sectors.csv");
	while (std::getline(sectorsFile, line)) {
		sectors.insert(line);
	}
	sectorsFile.close();

	std::unordered_set<std::string> useCases;
	std::ifstream useCasesFile("Use_Cases.csv");
	while (std::getline(useCasesFile, line)) {
		useCases.insert(line);
	}
	useCasesFile.close();

	// Open the input CSV file
	std::ifstream inputFile("Listt.csv");
	std::ofstream outputFile("Result.csv");
	std::string name, web;
	int count = 0;

	// Process each company in the input file
	while (std::getline(inputFile, line)) {
		std::istringstream iss(line);
		if (std::getline(iss, name, ',') && std::getline(iss, web, ',')) {
			// Fetch webiste content and measure time spent
			auto start = std::chrono::steady_clock::now();
			WebsiteData websiteData = fetchWebsiteContent(web);
			auto end = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			// Check for issues
			if (!websiteData.issue.empty()) {
				outputFile << name << "," << web << ",," << websiteData.issue << "\n";
				continue;
			}

			// Check if the website content contains any keyword from the CSV files
			if (containsKeyword(websiteData.content, techOfferings) ||
					containsKeyword(websiteData.content, sectors) ||
					containsKeyword(websiteData.content, useCases)) {

				// Save the result to the output CSV file
				outputFile << name << "," << web << "\n";
				++count;
			}
		}
	}

	// Close the files
	inputFile.close();
	outputFile.close();

	std::cout << "Matching results saved to Result.csv Total mataches: " << count << std::endl;

	return 0;
}
	
