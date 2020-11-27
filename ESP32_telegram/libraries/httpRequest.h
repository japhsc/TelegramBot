#ifndef HTTP_REQUEST_H
#define  HTTP_REQUEST_H

#include <HTTPClient.h>
#include "base64.h"

const String A[] = {"%",   "!",   "\"",  "#",   "$",   "&",
                    "'",   "(",   ")",   "*",   "+",   ",",
                    "-",   ".",   "/",   ":",   ";",   "<",
                    "=",   ">",   "?",   "@",   "[",   "\\",
                    "]",   "{",   "|",   "}",   " "};

const String B[] = {"%25", "%21", "%22", "%23", "%24", "%26",
                    "%27", "%28", "%29", "%2A", "%2B", "%2C",
                    "%2D", "%2E", "%2F", "%3A", "%3B", "%3C",
                    "%3D", "%3E", "%3F", "%40", "%5B", "%5C",
                    "%5D", "%7B", "%7C", "%7D", "+"};           //" " == "%20"

String base64_url_enc(const String &str) {
    BYTE *data;
    size_t data_len;
    base64_encode((unsigned char*) str.c_str(), str.length(), &data, &data_len);
    String data_str((char*) data);
    free(data);
    return data_str;
};

String base64_url_dec(const String &str) {
    BYTE *data;
    size_t data_len;
    base64_decode((unsigned char*) str.c_str(), str.length(), &data, &data_len);
    String data_str((char*) data);
    free(data);
    return data_str;
};

String urlsafe_enc(String text) {
    for (int i=0; i<29; ++i) {
        text.replace(A[i], B[i]);
    }
    return text;
};

String urlsafe_dec(String text) {
    for (int i=0; i<29; ++i) {
        text.replace(B[i], A[i]);
    }
    return text;
};

void byte_print(unsigned char* payload, const size_t &payload_size) {
    for (int i=0; i<payload_size; ++i) {
        if (i%60==0) Serial.println();
        if (payload[i]<17) {
            Serial.print("\\x0" + String(payload[i],HEX));
        } else if (payload[i]<32 || payload[i]>126)
            Serial.print("\\x" + String(payload[i],HEX));
        else
            Serial.print((char)payload[i]);
    }
    Serial.println();
};


class ByteStream : public Stream {
  private:
      const uint8_t* _data;
      const size_t _len;
      size_t _pos;
      int err = 0;
      
  public:
      ByteStream(const uint8_t* data, const size_t size) : 
                  _data(data), _len(size), _pos(0) { }
  
      int available() { return _len - _pos; };
      int read() { return _pos < _len ? _data[_pos++] : -1; };
      int peek() { return _pos < _len ? _data[_pos] : -1; };
      
      size_t readBytes(uint8_t* buff, size_t len) {
          int left = (_len - _pos);
          if (left < 0) return 0;
          size_t size = (left > len) ? len : left;
          /*
          Serial.print(_pos);
          Serial.print("/");
          Serial.print(_len);
          Serial.print(" left ");
          Serial.print(left);
          Serial.print(" -> ");
          Serial.print(len);
          Serial.print(" give ");
          Serial.println(size);
          */
          memcpy(buff, _data + _pos, size);
          _pos += size;
          return size;
      };
      
      void flush(){};
  
      size_t write(uint8_t data) {
          /*if (_pos < _len) {
              _data[_pos++] = data;
              return 1;
          } else*/
          err = 1;
          return 0;
      };
      /*
      size_t write(const uint8_t *buffer, size_t size) override;
      size_t write(uint8_t data) override;
      */
      int getWriteError() {
          return err;
      }
  
      void clearWriteError() {
          err = 0;
      }
};

template <class T>
int request(const String url, const char *httpType, 
            const String contentType, 
            T* payload, const size_t &size, 
            String &result)
{
    HTTPClient http;
    http.begin(url);
    if (contentType.length()) {
        http.addHeader("Content-Type", contentType);
    }
    int httpCode = http.sendRequest(httpType, payload, size);
    if (httpCode > 0) {
        result = http.getString();
    }
    http.end();
    return httpCode;
};

int request_stream(const String url, const char *httpType, 
            const String contentType, 
            const unsigned char* payload, const size_t size, 
            String &result)
{
    ByteStream stream(payload, size);
    return request(url, httpType, contentType, &stream, size, result);
};

int get(const String url, String &data) {
    return request(url, "GET", "", (unsigned char*) NULL, 0, data);
};

int post(const String url, const String contentType, String &data) {
    return request( url, "POST", contentType, (unsigned char*) data.c_str(), data.length(), data);
};

String hex_random(size_t n) {
    n *= 2;
    unsigned char hex[n+1];
    unsigned char x;
    randomSeed(analogRead(0));
    for (int i=0; i<n; i+=2) {
        x = random(0, 256);
        hex[i] = (x >> 4) + 0x30;
        if (hex[i] > 0x39) hex[i] += 39;

        hex[i+1] = (x & 0x0f) + 0x30;
        if (hex[i+1] > 0x39) hex[i+1] += 39;
    }
    hex[n] = 0;
    return String((const char*) hex);
}

#endif
