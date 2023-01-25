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
                  )

# create react dev server run script
file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/interface.sh
     "#!/bin/sh\n"
     "cd ${CMAKE_CURRENT_SOURCE_DIR}\n"
     "yarn start\n"
     )
file(CHMOD ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/interface.sh
     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
                 WORLD_READ WORLD_EXECUTE
     )
