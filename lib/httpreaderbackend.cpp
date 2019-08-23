#include "httpreaderbackend.h"
#include "parts_definitions.h"
#include <memory>
#include <curl/curl.h>
#include <cstring>
#include <fmt/format.h>
#include "logger_internal.h"
#include <boost/endian/conversion.hpp>

using std::string;
using std::vector;
using std::uint8_t;

using namespace parts;

namespace {

//==========================================================================================================================================
struct WriteDataStruct
{
    size_t size;
    uint8_t * data;
    int overflow = 0;
};

//==========================================================================================================================================
size_t write_data(void *ptr, size_t size, size_t nmemb, void *data_struct)
{
  auto& out = * static_cast<WriteDataStruct*>(data_struct);
  size_t copyc = std::min(size * nmemb, out.size);
  if (size * nmemb > out.size)
  {
      out.overflow += size * nmemb - out.size;
  }
  std::memcpy(out.data,ptr, copyc);
  out.size -= copyc;
  out.data +=copyc;
  //This might not be true, but in case of a file not found error,
  //we want to throw a file not found exception, and not die with a write error.
  return size * nmemb;

}

//==========================================================================================================================================
void getFilePart(CURL *curl_handle , int start, WriteDataStruct* wds)
{
    LOG_DEBUG("Requesting part of file. Start {}, Size {}",start, wds->size);
    vector<uint8_t> res;
    res.clear();
    //setting requested range
    curl_easy_setopt(curl_handle, CURLOPT_RANGE, fmt::format("{}-{}",start,start + wds->size - 1).c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    /* write the page body to this file handle */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, wds);

    char error_buf[CURL_ERROR_SIZE];
    error_buf[0] = '\0';
    curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, error_buf);
    long http_code = 0; //In case of 404 (not found) curl_easy_performs return code is CURLE_OK.
    CURLcode ec = curl_easy_perform(curl_handle);
    curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
    if ( (http_code != 200 && http_code != 206 /* partial content */ && http_code != 0) || ec != CURLE_OK)
    {
        throw PartsException(fmt::format("Error during http request. http code: {} Curl code: {}, curl error buffer: {}"
                                         ,http_code, (int)ec,error_buf));
    }
    if (wds->overflow)
    {
        throw PartsException(
                    fmt::format("Too much data in received http response. Remaining buffer size: {}. Received data size: {}"
                                , wds->size, wds->overflow ));
    }
    if (wds->size != 0)
    {
        throw PartsException("Not enought data in received http response");
    }
}

} // anonimous namespace


//==========================================================================================================================================
HttpReaderBackend::HttpReaderBackend(const string& file_url) :m_fileUrl(file_url)
{
    struct CurlInit
    {
        CurlInit() { curl_global_init(CURL_GLOBAL_ALL); }
        ~CurlInit() { curl_global_cleanup(); }
    };
    static CurlInit __curl_init;
    m_curlHandle = curl_easy_init();
    curl_easy_setopt(m_curlHandle, CURLOPT_URL, file_url.c_str());
    curl_easy_setopt(m_curlHandle, CURLOPT_HTTPHEADER, NULL);
    m_valid = true;
}

//==========================================================================================================================================
HttpReaderBackend::~HttpReaderBackend() noexcept
{
    curl_easy_cleanup(m_curlHandle);
}


//==========================================================================================================================================
void HttpReaderBackend::read(std::vector<uint8_t> &data)
{
    read(data.data(), data.size());
}

//==========================================================================================================================================
void HttpReaderBackend::read(InputBuffer& data, size_t size)
{
    size_t old_size = data.size();
    data.append(size);
    read(&(*(data.begin() + old_size)), size);
}

//==========================================================================================================================================
void HttpReaderBackend::read(uint8_t *data, size_t size)
{
    WriteDataStruct wd { size,(uint8_t*)data};
    getFilePart(m_curlHandle,m_currentPos, &wd);
    m_currentPos += size;
    ++m_sentRequests;
    m_readBytes += size;
}

//==========================================================================================================================================
void HttpReaderBackend::seek(const uint64_t &position)
{
    m_currentPos = position;
}
