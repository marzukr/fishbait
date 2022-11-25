# Copyright 2022 Marzuk Rashid

create_or_update_venv()

add_custom_target(lab ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/venv)

# create script to generate a lab config file
file(
  WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generate_lab_config.sh
  "#!/bin/sh\n"
  "cd ${CMAKE_CURRENT_SOURCE_DIR}/src\n"
  "export PYTHON_EXC=${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python\n"
  "eval \"$PYTHON_EXC -m config.generate\"\n"
)
file(
  CHMOD ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generate_lab_config.sh PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

# create script to run the lab
file(
  WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lab.sh
  "#!/bin/sh\n"
  "cd ${CMAKE_CURRENT_SOURCE_DIR}/src\n"
  "export VENV_BIN=${CMAKE_CURRENT_BINARY_DIR}/venv/bin\n"
  "export LAB_ENV=dev\n"
  "$VENV_BIN/uvicorn main:app --reload\n"
)
file(
  CHMOD ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lab.sh PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)
