if (HAVE_SSL AND WIN32)
set(applink_SOURCE ${OPENSSL_INCLUDE_DIR}/openssl/applink.c)
endif()

add_executable(tradeclient-wintmute Application.cpp tradeclient.cpp ${applink_SOURCE})
#add_library(tradeclient-wintmute STATIC Application.cpp tradeclient.cpp ${applink_SOURCE})

target_include_directories(tradeclient-wintmute PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/quickfix ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/examples/tradeclient-wintmute/lib/snowflake-cpp)


target_link_libraries(tradeclient-wintmute ${PROJECT_NAME})

if (NOT WIN32)
ADD_CUSTOM_TARGET(tradeclient-wintmute_target ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:tradeclient-wintmute> ${CMAKE_SOURCE_DIR}/bin/tradeclient-wintmute)
else()
set_target_properties(tradeclient-wintmute PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/tradeclient-wintmute/
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/tradeclient-wintmute/
                      RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/release/tradeclient-wintmute/)
endif()    

