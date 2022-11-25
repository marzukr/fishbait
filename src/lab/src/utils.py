# Copyright 2022 Marzuk Rashid

"""Utility functions for the scout"""

import base64
from io import BytesIO

from PIL.Image import Image as ImageT
from PIL import Image

def indent_string(string: str, indent_level: int):
  return string.rjust(len(string) + 2 * indent_level)

def print_with_indent(string: str, indent_level: int):
  print(indent_string(string, indent_level))

def input_with_indent(prompt: str, indent_level: int):
  return input(indent_string(prompt, indent_level))

def pil_to_png_str(img: ImageT):
  buffered = BytesIO()
  img.save(buffered, format='png')
  return base64.b64encode(buffered.getvalue()).decode('utf-8')

def png_str_to_pil(png: str):
  bytes_img = png.encode('utf-8')
  decode_img = base64.decodebytes(bytes_img)
  return Image.open(BytesIO(decode_img))
