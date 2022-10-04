if (HAVE_SSL AND WIN32)
set(applink_SOURCE ${OPENSSL_INCLUDE_DIR}/openssl/applink.c)
endif()

if(NOT DEFINED CLI11_DIR)
    set (CLI11_DIR ${CMAKE_SOURCE_DIR}/vendor/CLI11/include)
endif()
message("-- CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
#set(CMAKE_BUILD_TYPE Debug) 默认就是Debug
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    message("Debug mode:${CMAKE_C_FLAGS_DEBUG}")
    message("-- CMAKE_BUILD_TYPE2: ${CMAKE_BUILD_TYPE}")
#    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0")
    #这里是增加调试信息，可以打印出行号
    add_compile_definitions(DEBUG_VERSION)
    set(CMAKE_CXX_FLAGS   "-no-pie")
    set(CMAKE_C_FLAGS   "-no-pie")
    set(BOOST_STACKTRACE_USE_ADDR2LINE   "1")
#    add_compile_definitions(BOOST_STACKTRACE_USE_ADDR2LINE)
    add_definitions("-Wall -g")
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
#    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
    message("Release mode:${CMAKE_C_FLAGS_RELEASE}")
else()
    message("else:${CMAKE_BUILD_TYPE}")
    message("else:${CMAKE_C_FLAGS_RELEASE}")
endif()






# add_compile_definitions(CCAPI_USE_SINGLE_THREAD)
add_compile_definitions(CCAPI_ENABLE_LOG_FATAL)
add_compile_definitions(CCAPI_ENABLE_LOG_ERROR)
add_compile_definitions(CCAPI_ENABLE_LOG_WARN)
add_compile_definitions(CCAPI_ENABLE_LOG_INFO)
add_compile_definitions(CCAPI_ENABLE_LOG_DEBUG)
add_compile_definitions(CCAPI_ENABLE_LOG_TRACE)


add_compile_definitions(CCAPI_ENABLE_SERVICE_EXECUTION_MANAGEMENT)
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE)
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE_US)
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE_USDS_FUTURES)
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE_COIN_FUTURES)

# 测试网base url配置
add_compile_definitions(CCAPI_BINANCE_URL_REST_BASE="https://testnet.binance.vision/api")
add_compile_definitions(CCAPI_BINANCE_URL_WS_BASE="wss://testnet.binance.vision")

# 期货测试网
add_compile_definitions(CCAPI_BINANCE_USDS_FUTURES_URL_REST_BASE="https://testnet.binancefuture.com")
add_compile_definitions(CCAPI_BINANCE_USDS_FUTURES_URL_WS_BASE="wss://stream.binancefuture.com")


message("-- CCAPI_BINANCE_URL_REST_BASE: ${CCAPI_BINANCE_URL_REST_BASE}")
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

