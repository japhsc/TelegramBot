#ifndef HTTP_TELEGRAM_H
#define  HTTP_TELEGRAM_H

#include "httpRequest.h"

const String    token     = "TELEGRAM_BOT_TOKEN";
const long int  bot_chat  = TELEGRAM_CHAT_ID;

const String    bot_token = "bot" + token;
const String    url_api   = "https://api.telegram.org/";
const String    url_tkn   = url_api + bot_token + "/";

bool getMessage(String &data, const long int &update_id) {
    data = "{\"offset\": " + String(update_id) + "}";
    return (post(url_tkn+"getUpdates", "application/json", data)>0);
};

bool sendMessage(   String &data, 
                    const long int &chat_id, 
                    const String &text, 
                    const bool &notify = true) {
    data = "{\"chat_id\": \"" + String(chat_id) 
         + "\", \"text\": \"" + text + "\"";
    if (!notify)
        data += ", \"disable_notification\": " +  String(notify?"true":"false") + 
    data += "}";
    
    return (post(url_tkn+"sendMessage", "application/json", data)>0);
};

bool sendData(  String &result, const String uri, 
                const String type, const String name,
                const unsigned char* data, size_t size)
{
    //Content-Type
    const String boundary = hex_random(16);
    const String contentHead = "Content-Disposition: form-data; name=\""
                             + type + "\"; filename=\""
                             + name + "\"\r\n\r\n";
    const String contentType = "multipart/form-data; boundary="+boundary;
    const String head = "--" + boundary + "\r\n" + contentHead;
    const String tail = "\r\n--" + boundary + "--\r\n";

    const size_t payload_size = head.length()+size+tail.length();
    unsigned char* payload = (unsigned char*) malloc(payload_size*sizeof(char));
    if (!payload) return false;
    /*
    Serial.println(uri);
    Serial.println(payload_size);
    Serial.println(contentType);
    */
    memcpy(payload, head.c_str(), head.length());
    memcpy(payload+head.length(), data, size);
    memcpy(payload+head.length()+size, tail.c_str(), tail.length());

    int ret = request_stream(uri, "POST", contentType, payload, payload_size, result);
    free(payload);
    /*
    Serial.print("ret ");
    Serial.println(ret);
    Serial.print("\"\n");
    Serial.print(result);
    Serial.println("\n\"");
    //byte_print(payload, payload_size);
    */
    return (ret>0);
}

String make_uri(const String &path, 
                const long int &chat_id,
                const String &caption="",
                const bool &notify=true) {
    String uri = url_tkn+path+"?chat_id="+ String(chat_id);
    if (caption.length()>0) uri += "&caption=" + urlsafe_enc(caption);
    if (!notify)            uri += "&disable_notification=true";
    return uri;
}

/*
    sendPhoto (image/jpeg, image/png, image/bmp, image/svg+xml)
    "image/jpg"
*/
bool sendPhoto( String &result, const long int &chat_id,
                const unsigned char* data, const size_t &size,
                const String &caption="", 
                const String &filename = "image.jpg",
                const bool &notify=true)
{
    return sendData(result, 
                    make_uri("sendPhoto", chat_id, caption, notify),
                    "photo", filename, data, size);
};

/*
    sendVoice (audio/ogg) -> .OGG file encoded with OPUS
    sendAudio (audio/mpeg) -> .MP3 or .M4A format
    "audio/ogg"
*/
bool sendVoice( String &result, const long int &chat_id, 
                const unsigned char* data, const size_t &size, 
                const String &caption = "", 
                const String &filename = "voice.ogg",
                const bool &notify = true)
{
    return sendData(result, 
                    make_uri("sendVoice", chat_id, caption, notify),
                    "voice", filename, data, size);
};

bool sendDocument( String &result, const long int &chat_id, 
                const unsigned char* data, const size_t &size, 
                const String &caption = "", 
                const String filename = "audio.wav",
                const bool &notify = true)
{
    return sendData(result, 
                    make_uri("sendDocument", chat_id, caption, notify),
                    "document", filename, data, size);
};

/*
    POST "application/json" {"file_id": "file_id"}
    https://api.telegram.org/bot<bot_token>/getFile?file_id=the_file_id
*/
bool getFile(String &result, const String &file_id) {
    const String uri = url_tkn+"getFile?file_id="+file_id;
    return (get(uri,result)>0);
};

bool dlFile(String &result, const String &file_path) {
    const String uri = url_api+"file/"+bot_token+"/"+file_path;
    return get(uri, result);
};

#endif
