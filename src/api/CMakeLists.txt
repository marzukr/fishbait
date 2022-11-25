# Copyright 2021 Marzuk Rashid

create_or_update_venv()

add_custom_target(api ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/venv)

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
