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
    let cpp_lib_src = "vendor/quickfix-cpp/src/C++";

    cxx_build::bridge("src/lib.rs")
        .include(cpp_lib_src)
        .file("cxx/tradeclient.cpp")
        .file("cxx/application.cpp")
        .files(BASE_FILES.iter().map(|x| format!("{}/{}", cpp_lib_src, x)))
        .files(
            SSL_SUPPORT_FILES
                .iter()
                .map(|x| format!("{}/{}", cpp_lib_src, x)),
        )
        .compile("quickfix-cpp");
}
