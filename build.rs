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
    "Utility.cpp"
];

const SSL_SUPPORT_FILES: [&str; 7] = [
    "SSLSocketAcceptor.cpp",
    "SSLSocketConnection.cpp",
    "SSLSocketInitiator.cpp",
    "ThreadedSSLSocketAcceptor.cpp",
    "ThreadedSSLSocketConnection.cpp",
    "ThreadedSSLSocketInitiator.cpp",
    "UtilitySSL.cpp"
];

fn main() {
    let cpp_lib_src = Path::new("vendor/quickfix-cpp/src/C++");
    let srcjoin = |&d| cpp_lib_src.join(d);

    cxx_build::bridge("src/lib.rs")
        .include(Path::new("vendor/quickfix-cpp"))
        .include(Path::new("vendor/quickfix-cpp/src"))
        .include(cpp_lib_src)
        .files(BASE_FILES.iter().map(srcjoin))
        .files(SSL_SUPPORT_FILES.iter().map(srcjoin))
        .flag_if_supported("-std=c++14")
        // Make the compilation of quickfix quiet :)
        .flag_if_supported("-w")
        .compile("quickfix-cpp");
    
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=vendor/quickfix-cpp");
}
