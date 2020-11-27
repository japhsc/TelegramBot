# Telegram bot examples in Python and for ESP32

* Please add: 
    your token, chat id in ``ESP32_telegram/libraries/httpTelegram.h``
    and (for esp32) your wifi credentials in ``ESP32_telegram/ESP32_telegram.ino``

* To create ``ESP32_telegram/libraries/data.h`` place a ``voice.ogg`` and ``image.jpg`` in ``ESP32_telegram/parser/`` and run ``parser.py``

# How to start a telegram bot...

* [Bot API Documentation](https://core.telegram.org/bots/api)

* Find this description [here](https://gist.github.com/dideler/85de4d64f66c1966788c1b2304b9caf1)

    1. [Create a bot](https://core.telegram.org/bots#6-botfather)
    2. Get the bot's API token from [@BotFather] (https://telegram.me/BotFather)
    3. Add your bot to the chat you'll be sending messages to and send ``/start``
    4. Get the ID of the chat
        ```
        curl https://api.telegram.org/bot$TELEGRAM_BOT_TOKEN/getUpdates | jq .message.chat.id
        ```
    5. Send a message using the HTTP API: https://core.telegram.org/bots/api#sendmessage
        ```
        curl -X POST \
         -H 'Content-Type: application/json' \
         -d '{"chat_id": "123456789", ' \
            '"text": "This is a test from curl", ' \
            '"disable_notification": true}' \
            https://api.telegram.org/bot$TELEGRAM_BOT_TOKEN/sendMessage
        ```

```
TOKEN="YOUR_TOKEN" 
ID="YOUR_ID"
URL="https://api.telegram.org/bot$TOKEN/sendMessage"

curl -s -X POST $URL -d chat_id=$ID -d text="Hello World"
```

