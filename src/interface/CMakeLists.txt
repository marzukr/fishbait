# setup venv if it doesn't exist
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/venv")
  message(STATUS "Creating python virtual environment")
  find_package(Python3)
  execute_process(
    COMMAND ${Python3_EXECUTABLE} "-m" "venv" "venv"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RESULTS_VARIABLE venv_status
    )
  if(venv_status)
    message(FATAL_ERROR "Failed to create python virtual environment")
  endif()
endif()

# update venv packages
add_custom_target(venv-update ALL
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/pip install -r
            ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    COMMENT "Updating python virtual environment packages"
    )

# create server run script
file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/interface.sh
     "FLASK_ENV=development ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python "
     "${CMAKE_CURRENT_SOURCE_DIR}/app.py"
     )
file(CHMOD ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/interface.sh
     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
                 WORLD_READ WORLD_EXECUTE
     )
