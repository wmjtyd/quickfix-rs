use std::path::Path;

const BASE_FILES: [&str; 45] = [
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

const SSL_SUPPORT_FILES: [&str; 7] = [
    "SSLSocketAcceptor.cpp",
    "SSLSocketConnection.cpp",
    "SSLSocketInitiator.cpp",
    "ThreadedSSLSocketAcceptor.cpp",
    "ThreadedSSLSocketConnection.cpp",
    "ThreadedSSLSocketInitiator.cpp",
    "UtilitySSL.cpp",
];

fn main() {
    let cpp_root_path = Path::new("vendor/quickfix-cpp");
    let cpp_header_path = cpp_root_path.join("include/");
    let cpp_source_path = cpp_root_path.join("src/C++/");

    cxx_build::bridge("src/lib.rs")
        .include(cpp_header_path)
        .include(&cpp_source_path)
        .include("cxx/")
        .include(cpp_root_path.join("examples/tradeclient-apifiny/inc/"))
        .file("cxx/Application.cpp")
        .file("cxx/Tradeclient.cpp")
        .files(BASE_FILES.iter().map(|x| cpp_source_path.join(x)))
        .files(SSL_SUPPORT_FILES.iter().map(|x| cpp_source_path.join(x)))
        .compile("quickfix-cpp");

    println!("cargo:rerun-if-changed=src/");
    println!("cargo:rerun-if-changed=cxx/");
    println!("cargo:rerun-if-changed=vendor/quickfix-cpp");

    println!("cargo:rustc-link-lib=ssl");
    println!("cargo:rustc-link-lib=crypto");
}
