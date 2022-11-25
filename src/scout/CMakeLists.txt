# Copyright 2022 Marzuk Rashid

create_or_update_venv()

add_custom_target(scout ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/venv)

# create script to run the scout
file(
  WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scout.sh
  "#!/bin/sh\n"
  "cd ${CMAKE_CURRENT_SOURCE_DIR}/src\n"
  "export PYTHON_EXC=${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python\n"
  "eval \"$PYTHON_EXC -m $1\"\n"
)
file(
  CHMOD ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scout.sh PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)
