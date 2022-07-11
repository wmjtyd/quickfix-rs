#[cxx::bridge]
mod ffi {
    #[namespace = "FIX"]
    unsafe extern "C++" {
        include!("quickfix-rs/vendor/quickfix-cpp/include/quickfix/Values.h");

        type SessionID;
        type Message;

        fn toStringFrozen(self: &SessionID) -> &CxxString;
    }

    unsafe extern "C++" {
        include!("quickfix-rs/cxx/include/Application.h");

        type Application;

        fn new_application() -> UniquePtr<Application>;
    }

    extern "Rust" {
        type ApplicationImpl;

        fn new_application_impl() -> Box<ApplicationImpl>;
        fn on_create(self: &ApplicationImpl, session_id: &SessionID);
    }
}

mod application;
use application::{new_application_impl, ApplicationImpl};

#[cfg(test)]
mod tests {
    use super::ffi;

    #[test]
    fn test_nothing() {
        let app = ffi::new_application();
    }
}
