#ifndef HEADER_H
#define HEADER_H

namespace EURL {
    struct GeneralHeader {
        const char* CacheControl = "Cache-Control: no-cache";
        const char* Connection = "Connection: keep-alive";
        const char* Date="";
    };

    struct RequestHeader : public GeneralHeader {
        const char* ContentType = "Content-Type: application/json; charset=UTF-8";
        const char* UserAgent="";
        const char* Authorization = "Authorization: Bearer KEY";
        const char* Accept = "Accept:application/json, text/javascript, */*; q=0.01";
        const char* Host="";
        const char* Referer="";
    };

    struct ResponseHeader : public GeneralHeader {
        const char* ContentType="";
        const char* ContentLength="";
        const char* Server="";
        const char* TransferEncoding="";
        const char* Connection="";
        const char* Date="";
        const char* Expires="";
        const char* LastModified="";
        const char* Pragma="";
        const char* SetCookie="";
        const char* XPoweredBy="";
        const char* XRequestId="";
    };
    struct EntityHeader {
        const char* ContentType="Content-Type: application/json";
        const char* ContentLength="";
        const char* ContentEncoding="";
    };
}
#endif //HEADER_H
