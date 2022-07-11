fn main() {
    cxx_build::bridge("src/lib.rs")
        .file("cxx/Application.cxx")
        .compile("quickfix-cpp");
}
