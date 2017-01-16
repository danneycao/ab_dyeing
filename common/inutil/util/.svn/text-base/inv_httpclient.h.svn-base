#ifndef __HTTP_CURL_H__
#define __HTTP_CURL_H__

#include <string>

namespace inv
{

/**
    g++ demo.cpp  -I/usr/local/inutil/ libinvutil.a  /usr/local/lib/libcurl.a  -lrt -lssl
    demo.cpp:
    string response;
    int ret = INV_HttpClient::get("http://www.baidu.com/img/bdlogo.gif", response);
    std::cout << ret << ":" << response.length() << endl;
    ofstream out("bdlogo.gif");
    out.write(response.c_str(), response.length());
    out.close();
 */
class INV_HttpClient
{

public:

    static int post(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int timeout=3);  
    static int post_binary(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int timeout=3);
   
    static int get(const std::string & strUrl, std::string & strResponse, int timeout=3);
};

}

#endif
