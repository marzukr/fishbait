# Copyright 2022 Marzuk Rashid

"""Pydantic models for configs that describe poker apps"""

import json

from pydantic import BaseModel
from PIL import Image

from utils import input_with_indent, pil_to_png_str, print_with_indent


# Some constants
CONFIG_DIR = 'config/data'
CONFIG_LIST_FILE: str = f'{CONFIG_DIR}/list.json'


# Config models
class ConfigList(BaseModel):
  '''The list of all available poker app configs'''

  configs: list[str]

  def add_config(self, config: str):
    if config not in self.configs:
      self.configs.append(config)

  def __init__(self) -> None:
    with open(CONFIG_LIST_FILE, 'r', encoding='utf-8') as f:
      data = json.load(f)
      super().__init__(**data)

  def save(self):
    file_contents = json.dumps(self.dict(), indent=2)
    with open(CONFIG_LIST_FILE, 'w', encoding='utf-8') as f:
      f.write(file_contents)


class Box(BaseModel):
  width: int
  height: int

  @classmethod
  def from_user_input(cls, indent_level=0):
    width = int(input_with_indent('width: ', indent_level))
    height = int(input_with_indent('height: ', indent_level))
    return cls(width=width, height=height)


class Config(BaseModel):
  '''A poker app config'''

  upper_left_img: str
  window_size: Box

  @staticmethod
  def upper_left_img_from_user_input(indent_level=0):
    upper_left_img_loc = input_with_indent(
      'Location of upper left image: ',
      indent_level=indent_level
    )
    pil_img = Image.open(upper_left_img_loc)
    return pil_to_png_str(pil_img)

  @classmethod
  def from_user_input(cls, indent_level=0, to_edit: BaseModel | None = None):
    user_input_processors = {
      'upper_left_img': cls.upper_left_img_from_user_input,
      'window_size': Box.from_user_input
    }

    inputs = {}
    if to_edit is not None:
      inputs.update(to_edit.dict())

    for field in cls.__fields__:
      if to_edit is not None:
        edit_field = input_with_indent(
          f'do you want to override {field}? [y/n]: ',
          indent_level=indent_level
        )
        if edit_field != 'y':
          continue

      print_with_indent(f'configuring {field}:', indent_level)
      inputs[field] = user_input_processors[field](indent_level + 1)

    return cls(**inputs)

  @classmethod
  def open(cls, name: str):
    return cls.parse_file(f'{CONFIG_DIR}/{name}.json')

  def save(self, name: str):
    file_contents = json.dumps(self.dict(), indent=2)
    with open(f'{CONFIG_DIR}/{name}.json', 'w', encoding='utf-8') as f:
      f.write(file_contents)
