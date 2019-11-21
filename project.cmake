ADD_CUSTOM_TARGET(mbed-cli-flash
        COMMAND ${CMAKE_COMMAND} -E echo "mbed compile --flash"
        COMMAND mbed compile --flash
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        SOURCES ${SOURCE_FILES} ${SYS_SOURCE_FILES})
