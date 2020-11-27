//#define ARDUINOJSON_USE_LONG_LONG 1

#include <WiFi.h>
#include "ArduinoJson.h"

#include "libraries/httpTelegram.h"
#include "libraries/data.h" // create with parser/parser.py

#define SSID    "YOUR_SSID"
#define PASS    "YOUR_PASS"

bool parse_json(DynamicJsonDocument &json_data, const String &str_data) {
    auto error = deserializeJson(json_data, str_data);
    if (error) {
        Serial.print(F("Error in deserializeJson(): "));
        Serial.println(error.c_str());
        return false;
    }
    return true;
}

void commands(long int id, const JsonObject &data) {
    unsigned long int date = data["date"];
    Serial.print(id);
    Serial.print(": ");
    Serial.println(date);
    /*
    serializeJsonPretty(data, Serial);
    Serial.println();
    */
    for (JsonPair kv : data) {
        Serial.print("\t");
        Serial.println(kv.key().c_str());
    }
    if (data.containsKey("text")) {
        String text = data["text"];
        Serial.print("\t");
        Serial.println(text);
        
        String msg;
        if (text == "news") {
            msg = "Running since " + String(millis());
        } else if ((text == "?")) {
            msg = "What?";
        } else if ((text == "ok")) {
            msg = "Ok!";
        } else if ((text == "hallo")) {
            msg = "Hallo!";
        } else if ((text == "pic")) {
            // use chat id from json data
            const String caption = "Image!";
            bool ret = sendPhoto(msg, bot_chat, jpg_image, sizeof(jpg_image), caption);
            if (!ret) {
                Serial.println(F("Error on HTTP request!"));
            }
            Serial.println(msg);
            return;
        } else if ((text == "mic")) {
            const String caption = "Voice!";
            bool ret = sendVoice(msg, bot_chat, ogg_voice, sizeof(ogg_voice), caption);
            if (!ret) {
                Serial.println(F("Error on HTTP request!"));
            }
            Serial.println(msg);
        } else {
            msg = "I don't know!";
        }
        if (msg.length() && !sendMessage(msg, bot_chat, msg, true)) {
            Serial.println(F("Error on HTTP request!"));
            Serial.println(msg);
        }
    }
    
    if (data.containsKey("photo")) {
        size_t idx = 0; // low:0 mid:1 high:2
        const JsonObject &photo(data["photo"][idx]);
        String result;
        String file_id = photo["file_id"];
        if (getFile(result, file_id)) {
            DynamicJsonDocument ret(32768);
            if (parse_json(ret, result)) {
                Serial.print(ret["result"]["file_unique_id"].as<String>());
                Serial.print(" (");
                Serial.print(ret["result"]["file_size"].as<int>());
                Serial.print(") ");
                Serial.print(photo["width"].as<int>());
                Serial.print("x");
                Serial.println(photo["height"].as<int>());
                Serial.println(ret["result"]["file_path"].as<String>());
                String file_path = ret["result"]["file_path"];
                dlFile(result, file_path);
                Serial.println(result.length());
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(4000);
    WiFi.begin(SSID, PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println(F("Connecting to WiFi.."));
    }

    Serial.println(F("Connected to the WiFi network"));
}

String resp;
long int id, lastid = 0;

void loop() {
    if ((WiFi.status() == WL_CONNECTED)) {
        if (getMessage(resp, lastid+1)) {
            DynamicJsonDocument ret(65536);
            if (parse_json(ret, resp)) {
                if (ret["ok"]) {
                    for (int i=0; i<ret["result"].size(); ++i) {
                        id = ret["result"][i]["update_id"];
                        if (id>lastid) {
                            commands(id, ret["result"][i]["message"]);
                            lastid = id;
                        }
                    }
                }
            }
        } else {
            Serial.println("Error in getMessage!");
            Serial.println(resp);
        }
    } else {
        Serial.println("No Wifi!");
    }
    delay(5000);
}
