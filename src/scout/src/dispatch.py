# Copyright 2022 Marzuk Rashid

'''
Main executable for the scout which takes screenshots and sends them to the lab
for processing
'''

import json

from PIL import ImageGrab
import requests

from utils import pil_to_png_str, png_str_to_pil
from settings import LAB_URL


def main():
  screenshot = ImageGrab.grab()
  screenshot_str = pil_to_png_str(screenshot)
  lab_result = requests.post(
    f'{LAB_URL}/process_hand_start',
    data=json.dumps({
      'png_str': screenshot_str
    }),
    params={
      'poker_app': 'bovada_mac',
    },
    timeout=30,
  ).json()
  result_ss_pil = png_str_to_pil(lab_result['result'])
  result_ss_pil.save('/Users/marzukrashid/Desktop/scout.png')

if __name__ == '__main__':
  main()
