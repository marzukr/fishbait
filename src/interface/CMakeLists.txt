# Copyright 2021 Marzuk Rashid

# setup venv if it doesn't exist
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/venv")
  message(STATUS "Creating python virtual environment")
  find_package(Python3)
  execute_process(
    COMMAND ${Python3_EXECUTABLE} "-m" "venv" "venv"
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
          ${CMAKE_CURRENT_SOURCE_DIR}/api/requirements.txt
  COMMAND touch -r ${CMAKE_CURRENT_SOURCE_DIR}/api/requirements.txt
          ${CMAKE_CURRENT_BINARY_DIR}/venv
  COMMENT "Updating python virtual environment packages"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/api/requirements.txt
  )

# update npm packages
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/node_modules
  COMMAND yarn install
  COMMAND touch -r package.json ${CMAKE_CURRENT_SOURCE_DIR}/node_modules
  COMMENT "Updating npm packages"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/package.json
  )

# build the react app and update the list of files it depends on
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build
  COMMAND ${CMAKE_COMMAND} -E echo_append "${CMAKE_CURRENT_BINARY_DIR}/build "
          "${CMAKE_CURRENT_BINARY_DIR}/filelist.d : " >
          ${CMAKE_CURRENT_BINARY_DIR}/filelist.d
  COMMAND find ${CMAKE_CURRENT_SOURCE_DIR}
          -path ${CMAKE_CURRENT_SOURCE_DIR}/node_modules -prune -o -print |
          tr '\\n' ' ' >> ${CMAKE_CURRENT_BINARY_DIR}/filelist.d
  COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_BINARY_DIR}/build
  COMMAND yarn build
  COMMAND ${CMAKE_COMMAND} -E rename build ${CMAKE_CURRENT_BINARY_DIR}/build
  COMMAND touch -r ${CMAKE_CURRENT_BINARY_DIR}/build
          ${CMAKE_CURRENT_SOURCE_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "building interface frontend"
  DEPFILE ${CMAKE_CURRENT_BINARY_DIR}/filelist.d
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/node_modules
  )

add_custom_target(interface ALL
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/build
                          ${CMAKE_CURRENT_BINARY_DIR}/venv
                  )

# create server run script
file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/interface.sh
     "cd ${CMAKE_CURRENT_SOURCE_DIR}\n"
     "VENV_PY=${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python yarn start"
     )
file(CHMOD ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/interface.sh
     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
                 WORLD_READ WORLD_EXECUTE
     )
