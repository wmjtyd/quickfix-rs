if (HAVE_SSL AND WIN32)
set(applink_SOURCE ${OPENSSL_INCLUDE_DIR}/openssl/applink.c)
endif()

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/vendor/quickfix-cpp/src/C++)
link_libraries(libssl.a libcrypto.a)
add_executable(tradeclient-apifiny Application.cpp Tradeclient.cpp main.cpp ${applink_SOURCE})
TARGET_LINK_LIBRARIES(tradeclient-apifiny dl pthread)
#link_directories(${CMAKE_SOURCE_DIR}/examples/tradeclient/lib/openssl/lib/MinGW)
#link_directories(${CMAKE_SOURCE_DIR}/examples/tradeclient/lib/openssl/lib/MinGW)
#target_link_libraries(libcommon.lib openssl.lib libssl.lib libssl_static.lib libcrypto.lib)

#${CMAKE_SOURCE_DIR}/examples/tradeclient/lib/openssl/include
message("--  CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}"  )
message("--  PROJECT_NAME ${PROJECT_NAME}" )
target_include_directories(tradeclient-apifiny PRIVATE ${CMAKE_SOURCE_DIR}/vendor/quickfix-cpp/include ${CMAKE_SOURCE_DIR}/vendor/quickfix-cpp/include/quickfix ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/cxx/tradeclient-apifiny/inc ${CMAKE_SOURCE_DIR}/cxx ${CMAKE_SOURCE_DIR}/cxx/tradeclient-apifiny
        ${CMAKE_SOURCE_DIR}/src/C++/ ${CMAKE_SOURCE_DIR}/target/cxxbridge/ ${CMAKE_SOURCE_DIR})


target_link_libraries(tradeclient-apifiny ${PROJECT_NAME})

if (NOT WIN32)
ADD_CUSTOM_TARGET(tradeclient-apifiny_target ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:tradeclient-apifiny> ${CMAKE_SOURCE_DIR}/bin/tradeclient-apifiny)
else()
set_target_properties(tradeclient-apifiny PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/tradeclient-apifiny/
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/tradeclient-apifiny/
                      RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/release/tradeclient-apifiny/)
endif()    

