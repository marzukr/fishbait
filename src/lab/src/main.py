# Copyright 2022 Marzuk Rashid

'''
Where all specimens enter to get processed by the lab
'''

import os

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

from utils import png_str_to_pil, pil_to_png_str
from config.models import Config, ConfigList
from scientist import crop_screenshot_with_topleft_corner


lab_env = os.getenv('LAB_ENV', None)
if lab_env == 'dev':
  app = FastAPI()
else:
  app = FastAPI(docs_url=None, redoc_url=None, openapi_url=None)


class Screenshot(BaseModel):
  png_str: str

class ProcessedHandStart(BaseModel):
  result: str | None

@app.post('/process_hand_start')
def process_hand_start(screenshot: Screenshot, poker_app: str):
  if poker_app not in ConfigList().configs:
    raise HTTPException(status_code=404, detail=f'{poker_app} not found')
  app_config = Config.open(poker_app)
  screenshot_pil = png_str_to_pil(screenshot.png_str)
  topleft_pil = png_str_to_pil(app_config.upper_left_img)
  cropped_screenshot = crop_screenshot_with_topleft_corner(
    original_screenshot=screenshot_pil,
    topleft_corner=topleft_pil,
    crop_size=app_config.window_size,
  )
  cropped_str_img = cropped_screenshot and pil_to_png_str(cropped_screenshot)
  return ProcessedHandStart(result=cropped_str_img)
