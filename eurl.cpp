#include "eurl.h"


namespace EURL {
    bool eurl::Download(const char* url, const char* savePath, const char* proxy) {
        if (!_isInitialized()) {
            Initialize();
        }
        std::ofstream outfile(savePath, std::ios::binary);
        auto pCurl = curl_easy_init();
        curl_easy_setopt(pCurl, CURLOPT_URL, url);
        curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_to_file);
        curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &outfile);
        curl_easy_setopt(pCurl, CURLOPT_PROXY, proxy);
        if ( DEBUGLOGING) {
            curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
        }
        CURLcode res = curl_easy_perform(pCurl);
        curl_easy_cleanup(pCurl);
        outfile.close();
        if (res != CURLE_OK) {
            std::cerr << "Download failed: " << curl_easy_strerror(res) << std::endl;
        }
        return res == CURLE_OK;
    }

    bool eurl::Get(const char* url, std::string&response, const char* proxy) {
        if (!_isInitialized()) {
            Initialize();
        }
        std::cout << "Submit Get request to: " << url << std::endl;
        auto pCurl = curl_easy_init();
        curl_easy_setopt(pCurl, CURLOPT_URL, url);
        curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_to_str);
        curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(pCurl, CURLOPT_PROXY, proxy);
        if ( DEBUGLOGING) {
            curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
        }
        CURLcode res = curl_easy_perform(pCurl);
        curl_easy_cleanup(pCurl);

        if (res != CURLE_OK) {
            std::cerr << "Get failed: " << curl_easy_strerror(res) << std::endl;
        }
        std::cout << "Submit Get request to: " << url << " success" << std::endl;

        return res == CURLE_OK;
    }

    bool eurl::Post(const char* url, std::string data, json&response, RequestHeader header,
                    WriteCallback callback, const char* proxy) {
        if (!_isInitialized()) {
            Initialize();
        }
        auto pCurl = curl_easy_init();
        curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, header.ContentType);
        headers = curl_slist_append(headers, header.Accept);
        headers = curl_slist_append(headers, header.Connection);
        if (header.UserAgent != "")
            headers = curl_slist_append(headers, header.UserAgent);
        if (header.Date != "")
            headers = curl_slist_append(headers, header.Date);
        if (header.Host != "")
            headers = curl_slist_append(headers, header.Host);
        if (header.CacheControl != "")
            headers = curl_slist_append(headers, header.CacheControl);
        if (header.Authorization != "")
            headers = curl_slist_append(headers, header.Authorization);
        if (header.Referer != "")
            headers = curl_slist_append(headers, header.Referer);
        curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(pCurl, CURLOPT_URL, url);
        curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, data.size());
        if (callback != nullptr)
            curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, callback);
        else
            curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_to_json);
        curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(pCurl, CURLOPT_PROXY, proxy);
        if ( DEBUGLOGING) {
            curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
        }
        CURLcode res = curl_easy_perform(pCurl);
        curl_easy_cleanup(pCurl);
        curl_slist_free_all(headers);
        if (res != CURLE_OK) {
            std::cerr << "Post failed: " << curl_easy_strerror(res) << std::endl;
        }
        return res == CURLE_OK;
    }

    void eurl::MultiThreadedDownload(const std::string&url, const std::string&savePath,
                                     size_t numThreads) {
        std::vector<std::thread> threads;
        std::vector<std::string> outputFiles(numThreads);
        size_t fileSize = DownloadTask::GetFileSize(url);
        if (fileSize == 0) {
            std::cerr << "Failed to get file size" << std::endl;
            Download(url.c_str(), savePath.c_str());
            return;
        }
        size_t chunkSize = fileSize / numThreads;
        for (size_t i = 0; i < numThreads; ++i) {
            size_t start = i * chunkSize;
            size_t end = (i == numThreads - 1) ? fileSize - 1 : (i + 1) * chunkSize - 1;
            outputFiles[i] = savePath + ".part" + std::to_string(i);
            threads.emplace_back(DownloadTask::Run, url, outputFiles[i], start, end);
        }

        while (totalDownloaded < fileSize) {
            std::cout << "Downloaded: " << totalDownloaded << " / " << fileSize << " bytes" << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        for (auto&thread: threads) {
            thread.join();
        }

        std::ofstream finalFile(savePath, std::ios::binary);
        if (!finalFile) {
            std::cerr << "Failed to open final file: " << savePath << std::endl;
            return;
        }

        for (const auto&partFile: outputFiles) {
            std::ifstream part(partFile, std::ios::binary);
            if (!part) {
                std::cerr << "Failed to open part file: " << partFile << std::endl;
                continue;
            }

            finalFile << part.rdbuf();
            part.close();
            remove(partFile.c_str());
        }

        finalFile.close();
    }

    bool eurl::_isInitialized() {
        return instance != nullptr;
    }

    bool eurl::Initialize() {
        if (!_isInitialized()) {
            instance = new eurl();
            curl_global_init(CURL_GLOBAL_DEFAULT);

            return true;
        }
        return false;
    }
}
