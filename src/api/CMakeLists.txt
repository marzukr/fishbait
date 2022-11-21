# Copyright 2021 Marzuk Rashid

# setup venv if it doesn't exist
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/venv")
  message(STATUS "Creating python virtual environment")
  find_package(Python3 3.10 REQUIRED)
  execute_process(
    COMMAND ${Python3_EXECUTABLE} "-m" "venv" "venv" "--copies"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RESULTS_VARIABLE venv_status
    )
  execute_process(
    COMMAND touch -t 200001010000 venv
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RESULTS_VARIABLE touch_status
    )
  if(venv_status)
    message(FATAL_ERROR "Failed to create python virtual enviornment: "
            ${venv_status}
            )
  elseif(touch_status)
    message(FATAL_ERROR "Failed to touch python virtual enviornment: "
            ${touch_status}
            )
  endif()
endif()

# update venv packages
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/venv
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/pip install -r
          ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
  COMMAND touch -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
          ${CMAKE_CURRENT_BINARY_DIR}/venv
  COMMENT "Updating python virtual environment packages"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
  )

add_custom_target(api ALL
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/venv
                  )

# create api run script
file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/api.sh
     "#!/bin/sh\n"
     "cd ${CMAKE_CURRENT_SOURCE_DIR}/src\n"
     "${CMAKE_CURRENT_BINARY_DIR}/venv/bin/flask --app app.py --debug run &\n"
     "${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python depot.py &\n"
     "trap 'trap - SIGTERM && kill -- -$$' SIGINT SIGTERM EXIT\n"
     "wait\n"
     )
file(CHMOD ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/api.sh
     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
                 WORLD_READ WORLD_EXECUTE
     )
