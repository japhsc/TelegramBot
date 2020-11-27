token = 'YOUR_TOKEN'

from telegram.ext import Updater
from telegram.ext import CommandHandler
from telegram.ext import MessageHandler, Filters

import os
from datetime import datetime
from capture import capture_image, capture_video, capture_audio

import logging
logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                     level=logging.INFO)

updater = Updater(token=token, use_context=True)
dispatcher = updater.dispatcher

def start(update, context):
    text = "I'm a bot, please talk to me!"
    context.bot.send_message(chat_id=update.effective_chat.id, text=text)

def image(update, context):
    #image = open('image.jpg', 'rb')
    logging.info('take picture')
    image = capture_image()
    if image is None:
        context.bot.send_message(chat_id=update.effective_chat.id, text="ERROR")
    else:
        caption = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        context.bot.send_photo(chat_id=update.effective_chat.id, photo=image, caption=caption)

def video(update, context):
    filename = './temp.mp4'
    logging.info('start video recording')
    image = capture_video(filename, duration=2.)
    logging.info('stop video recording')
    if image is None:
        context.bot.send_message(chat_id=update.effective_chat.id, text="ERROR")
    else:
        caption = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        context.bot.send_video(chat_id=update.effective_chat.id, video=image, caption=caption)
        os.remove(filename)
    
def audio(update, context):
    logging.info('start audio recording')
    image = capture_audio(duration=2.)
    logging.info('stop audio recording')
    caption = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    context.bot.send_audio(chat_id=update.effective_chat.id, audio=image, caption=caption)


def echo(update, context):
    context.bot.send_message(chat_id=update.effective_chat.id, text=update.message.text)


dispatcher.add_handler(CommandHandler('start', start))
dispatcher.add_handler(CommandHandler('image', image))
dispatcher.add_handler(CommandHandler('video', video))
dispatcher.add_handler(CommandHandler('audio', audio))

echo_handler = MessageHandler(Filters.text & (~Filters.command), echo)
dispatcher.add_handler(echo_handler)

updater.start_polling()

