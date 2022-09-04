if (HAVE_SSL AND WIN32)
set(applink_SOURCE ${OPENSSL_INCLUDE_DIR}/openssl/applink.c)
endif()

if(NOT DEFINED CLI11_DIR)
    set (CLI11_DIR ${CMAKE_SOURCE_DIR}/vendor/CLI11/include)
endif()

find_package(OpenSSL REQUIRED)
message("-- OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
message("-- OPENSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")
message("-- OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")


# LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/vendor/quickfix-cpp/src/C++)
# link_libraries(libssl.a libcrypto.a)
link_libraries(ssl crypto)

add_executable(tradeclient-ccapi ../Tradeclient.cpp ApplicationCCApi.cpp TradeclientCCApi.cpp main.cpp ccapi_wrapper.cpp ${applink_SOURCE})
TARGET_LINK_LIBRARIES(tradeclient-ccapi dl pthread)
#link_directories(${CMAKE_SOURCE_DIR}/examples/tradeclient/lib/openssl/lib/MinGW)
#link_directories(${CMAKE_SOURCE_DIR}/examples/tradeclient/lib/openssl/lib/MinGW)
#target_link_libraries(libcommon.lib openssl.lib libssl.lib libssl_static.lib libcrypto.lib)

#${CMAKE_SOURCE_DIR}/examples/tradeclient/lib/openssl/include
message("--  CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}"  )
message("--  PROJECT_NAME ${PROJECT_NAME}" )
target_include_directories(tradeclient-ccapi PRIVATE ${CMAKE_SOURCE_DIR}/vendor/quickfix-cpp/include ${CMAKE_SOURCE_DIR}/vendor/quickfix-cpp/include ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/cxx/tradeclient-apifiny/inc ${CMAKE_SOURCE_DIR}/cxx ${CMAKE_SOURCE_DIR}/cxx/tradeclient-apifiny
        ${CMAKE_SOURCE_DIR}/src/C++/ ${CMAKE_SOURCE_DIR}/vendor/ccapi/include
        ${CMAKE_SOURCE_DIR}/vendor/ccapi/dependency/boost
        ${CMAKE_SOURCE_DIR}/vendor/ccapi/dependency/rapidjson/include
        ${CMAKE_SOURCE_DIR}/vendor/ccapi/dependency/websocketpp
        ${CMAKE_SOURCE_DIR}/vendor/CLI11/include)
  

# target_link_libraries(tradeclient-ccapi ${PROJECT_NAME})

if (NOT WIN32)
ADD_CUSTOM_TARGET(tradeclient-ccapi_target ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:tradeclient-apifiny> ${CMAKE_SOURCE_DIR}/bin/tradeclient-apifiny)
else()
set_target_properties(tradeclient-ccapi PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/tradeclient-apifiny/
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/tradeclient-apifiny/
                      RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/release/tradeclient-apifiny/)
endif()    

