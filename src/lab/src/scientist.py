# Copyright 2022 Marzuk Rashid

'''
Performs analysis on specimens in the lab
'''

import cv2
import numpy as np
from PIL.Image import Image as ImageT

from settings import TEMPLATE_MATCH_THRESHOLD
from config.models import Box

def find_template_on_screen(template: np.ndarray, screenshot: np.ndarray):
  res = cv2.matchTemplate(screenshot, template, cv2.TM_SQDIFF_NORMED)
  loc = np.where(res <= TEMPLATE_MATCH_THRESHOLD)
  min_val, _, min_loc, _ = cv2.minMaxLoc(res)
  best_fit = min_loc
  count = 0
  points = []
  for pt in zip(*loc[::-1]):
    count += 1
    points.append(pt)
  return count, points, best_fit, min_val

def crop_screenshot_with_topleft_corner(
  original_screenshot: ImageT,
  topleft_corner: ImageT,
  crop_size: Box,
):
  img = cv2.cvtColor(np.array(original_screenshot), cv2.COLOR_BGR2RGB)
  corner_img = cv2.cvtColor(np.array(topleft_corner), cv2.COLOR_BGR2RGB)
  count, _, tlc, _ = find_template_on_screen(corner_img, img)
  if count == 1:
    cropped_screenshot = original_screenshot.crop((
      tlc[0],
      tlc[1],
      tlc[0] + crop_size.width,
      tlc[1] + crop_size.height,
    ))
    return cropped_screenshot
  elif count > 1:
    return None
  else:
    return None
