# Copyright 2022 Marzuk Rashid

"""Utility to generate poker table config files"""

import sys

from config.models import ConfigList, Config

def main():
  config_name = input('Name for this config: ')
  existing_list = ConfigList()

  if config_name == 'list':
    print('Config name cannot be \'list\'')
    sys.exit(1)

  to_edit = None
  if config_name in existing_list.configs:
    override = input(
      f'{config_name} already exists. Do you want to override? [y/n]: '
    )
    if override != 'y':
      sys.exit()
    else:
      to_edit = Config.open(config_name)

  new_config = Config.from_user_input(to_edit=to_edit)
  new_config.save(config_name)
  existing_list.add_config(config_name)
  existing_list.save()

if __name__ == '__main__':
  main()
