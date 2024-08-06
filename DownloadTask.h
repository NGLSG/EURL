#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H
#include <fstream>
#include <iostream>
#include <string>
#include <curl/curl.h>

namespace EURL {
    inline std::atomic<size_t> totalDownloaded(0);

    inline size_t write_to_file(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t newLength = size * nmemb;
        std::ofstream* outfile = static_cast<std::ofstream *>(userp);
        outfile->write(static_cast<char *>(contents), newLength);
        totalDownloaded += newLength;
        return newLength;
    }

    class DownloadTask {
    public:
        inline static size_t GetFileSize(const std::string&url) {
            CURL* curl = curl_easy_init();
            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

                CURLcode res = curl_easy_perform(curl);
                if (res != CURLE_OK) {
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                    curl_easy_cleanup(curl);
                    return 0;
                }

                double fileSize;
                curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &fileSize);
                curl_easy_cleanup(curl);
                if (fileSize == -1)
                    return 0;
                return static_cast<size_t>(fileSize);
            }
            return 0;
        }

        inline static void Run(const std::string&url, const std::string&outputFile, size_t start, size_t end) {
            CURL* curl = curl_easy_init();
            if (curl) {
                std::ofstream outfile(outputFile, std::ios::binary);
                if (!outfile) {
                    std::cerr << "Failed to open file: " << outputFile << std::endl;
                    return;
                }

                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_file);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);
                curl_easy_setopt(curl, CURLOPT_RANGE, std::to_string(start) + "-" + std::to_string(end));

                CURLcode res = curl_easy_perform(curl);
                if (res != CURLE_OK) {
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                }

                outfile.close();
                curl_easy_cleanup(curl);
            }
        }
    };
}
#endif //DOWNLOADTASK_H
