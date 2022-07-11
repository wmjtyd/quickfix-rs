use crate::ffi::{Message, SessionID};

pub struct ApplicationImpl {}

pub fn new_application_impl() -> Box<ApplicationImpl> {
    Box::new(ApplicationImpl {})
}

impl ApplicationImpl {
    pub fn on_create(&self, session_id: &SessionID) {
        println!("on_create {}", session_id.toStringFrozen());
    }

    pub fn on_logon(&self, session_id: &SessionID) {
        println!("on_logon {}", session_id.toStringFrozen());
    }

    pub fn on_logout(&self, session_id: &SessionID) {
        println!("on_logout {}", session_id.toStringFrozen());
    }

    pub fn to_admin(&self, message: &mut Message, session_id: &SessionID) {
        todo!()
    }

    pub fn from_admin(&self, message: &Message, session_id: &SessionID) {
        todo!()
    }

    pub fn to_app(&self, message: &mut Message, session_id: &SessionID) {
        todo!()
    }

    pub fn from_app(&self, message: &Message, session_id: &SessionID) {
        todo!()
    }
}
