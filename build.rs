use std::{
    env,
    path::{Path, PathBuf},
};

const QUICKFIX_BASE_FILES: [&str; 45] = [
    "Acceptor.cpp",
    "DataDictionary.cpp",
    "DataDictionaryProvider.cpp",
    "Dictionary.cpp",
    "FieldConvertors.cpp",
    "FieldMap.cpp",
    "FieldTypes.cpp",
    "FileLog.cpp",
    "FileStore.cpp",
    "Group.cpp",
    "HttpConnection.cpp",
    "HttpMessage.cpp",
    "HttpParser.cpp",
    "HttpServer.cpp",
    "Initiator.cpp",
    "Log.cpp",
    "Message.cpp",
    "MessageSorters.cpp",
    "MessageStore.cpp",
    "MySQLLog.cpp",
    "MySQLStore.cpp",
    "NullStore.cpp",
    "OdbcLog.cpp",
    "OdbcStore.cpp",
    "Parser.cpp",
    "PostgreSQLLog.cpp",
    "PostgreSQLStore.cpp",
    "pugixml.cpp",
    "PUGIXML_DOMDocument.cpp",
    "Session.cpp",
    "SessionFactory.cpp",
    "SessionSettings.cpp",
    "Settings.cpp",
    "SocketAcceptor.cpp",
    "SocketConnection.cpp",
    "SocketConnector.cpp",
    "SocketInitiator.cpp",
    "SocketMonitor.cpp",
    "SocketServer.cpp",
    "stdafx.cpp",
    "ThreadedSocketAcceptor.cpp",
    "ThreadedSocketConnection.cpp",
    "ThreadedSocketInitiator.cpp",
    "TimeRange.cpp",
    "Utility.cpp",
];

const QUICKFIX_SSL_SUPPORT_FILES: [&str; 7] = [
    "SSLSocketAcceptor.cpp",
    "SSLSocketConnection.cpp",
    "SSLSocketInitiator.cpp",
    "ThreadedSSLSocketAcceptor.cpp",
    "ThreadedSSLSocketConnection.cpp",
    "ThreadedSSLSocketInitiator.cpp",
    "UtilitySSL.cpp",
];

const APIFINY_FILES: [&str; 2] = ["Application.cpp", "Tradeclient.cpp"];
const CCAPI_FILES: [&str; 3] = ["Application.cpp", "Tradeclient.cpp", "ccapi_wrapper.cpp"];

fn main() {
    let bindings = bindgen::Builder::default()
        .clang_args(["-x", "c++"])
        .header("cxx/wrapper.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .opaque_type("std::.*")
        .allowlist_var("FIX.*")
        .generate()
        .expect("Unable to generate bindings");
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");

    let cpp_root_path = Path::new("vendor/quickfix-cpp/");
    let cpp_header_path = cpp_root_path.join("include/");
    let cpp_source_path = cpp_root_path.join("src/C++/");
    let ccapi_header_path = Path::new("vendor/ccapi/include/");
    let ccapi_depandency_rapidjson_path = Path::new("vendor/ccapi/dependency/rapidjson/include");
    let ccapi_depandency_websocket_path = Path::new("vendor/ccapi/dependency/websocketpp");
    let ccapi_depandency_boost_path = Path::new("vendor/ccapi/dependency/boost");
    

    cxx_build::bridge("src/lib.rs")
        .include(&cpp_header_path)
        .include(&cpp_source_path)
        .include("cxx/")
        .include(cpp_root_path.join("examples/tradeclient-apifiny/inc/"))
        .include(&ccapi_header_path)
        .include (&ccapi_depandency_rapidjson_path)
        .include (&ccapi_depandency_websocket_path)
        .include (&ccapi_depandency_boost_path)
        .files(
            APIFINY_FILES
                .iter()
                .map(|x| Path::new("cxx/tradeclient-apifiny").join(x)),
        )
        .files(
            CCAPI_FILES
                .iter()
                .map(|x| Path::new("cxx/tradeclient-ccapi").join(x)),
        )
        .files(
            QUICKFIX_BASE_FILES
                .iter()
                .chain(&QUICKFIX_SSL_SUPPORT_FILES)
                .map(|x| cpp_source_path.join(x)),
        )
        .compile("quickfix-cpp");

    println!("cargo:rerun-if-changed=src/");
    println!("cargo:rerun-if-changed=cxx/");
    println!("cargo:rerun-if-changed=vendor/quickfix-cpp/");
    println!("cargo:rerun-if-changed=vendor/ccapi/");

    println!("cargo:rustc-link-lib=ssl");
    println!("cargo:rustc-link-lib=crypto");
}
