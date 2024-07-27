#ifndef EURL_H
#define EURL_H

#include <curl/curl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

#include "Header.h"
#include <nlohmann/json.hpp>

#include "DownloadTask.h"
using nlohmann::json;

#define Debug true
#ifdef NDEBUG
#define Debug false
#endif

namespace EURL {
    using WriteCallback = size_t(*)(void* contents, size_t size, size_t nmemb, void* userp);


    inline size_t write_to_str(void* contents, size_t size, size_t nmemb, std::string* s) {
        size_t newLength = size * nmemb;
        try {
            s->append((char *)contents, newLength);
        }
        catch (std::bad_alloc&e) {
            return 0;
        }
        return newLength;
    }

    inline size_t write_to_json(void* contents, size_t size, size_t nmemb, json* s) {
        size_t newLength = size * nmemb;
        try {
            // 将接收到的数据块解析为 JSON 对象
            *s = json::parse(static_cast<char *>(contents), static_cast<char *>(contents) + newLength);
        }
        catch (std::bad_alloc&e) {
            return 0;
        } catch (json::parse_error&e) {
            // 处理解析错误
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            return 0;
        }
        return newLength;
    }

    class eurl {
    public:
        inline static eurl* instance;

        static bool Download(const char* url, const char* savePath, const char* proxy = "");

        static bool Get(const char* url, std::string&response, const char* proxy = "");

        static bool Post(const char* url, std::string data, json&response, RequestHeader header,
                         WriteCallback callback = nullptr,
                         const char* proxy = "");

        static void MultiThreadedDownload(const std::string&url, const std::string&savePath,
                                          size_t numThreads = 16);

    private :
        static bool _isInitialized();

        static bool Initialize();
    };
}


#endif //EURL_H
