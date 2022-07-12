use rand::prelude::*;

use std::{
    sync::mpsc,
    time::{SystemTime, UNIX_EPOCH},
};

#[cxx::bridge]
pub mod ffi {
    #[derive(Copy, Clone, Debug)]
    enum FixMessageType {
        Admin,
        App,
    }

    #[derive(Debug)]
    struct QuickFixMessage {
        content: UniquePtr<CxxString>,
        from: FixMessageType,
    }

    #[namespace = "FIX"]
    unsafe extern "C++" {
        include!("quickfix-rs/vendor/quickfix-cpp/src/C++/Values.h");

        type SessionID;
    }

    unsafe extern "C++" {
        include!("quickfix-rs/cxx/include/Tradeclient.h");

        type TradeClient;

        fn create_client(
            filepath: &CxxString,
            app_ctx: Box<ApplicationContext>,
            inbound_callback: fn(
                message: QuickFixMessage,
                session_id: &SessionID,
                app_ctx: &Box<ApplicationContext>,
            ),
        ) -> UniquePtr<TradeClient>;

        fn start(&self);
        fn stop(&self);

        fn put_order(
            self: &TradeClient,
            order_id: &CxxString,
            quote_id: &CxxString,
            symbol: &CxxString,
            currency: &CxxString,
            side: u32,
            quantity: u32,
            price: u32,
            time_in_force: u32,
        );
    }

    extern "Rust" {
        type ApplicationContext;

        fn inbound(&self, message: QuickFixMessage, session_id: &SessionID);
    }
}

pub fn generate_order_id(account_id: &str) -> String {
    let account_id = account_id.split('-').last().expect("Must have number part");
    let timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("Time went backwards")
        .as_millis();
    let random_number = random::<u16>() % 1000;

    format!("{}{:.13}{:03}", account_id, timestamp, random_number)
}

pub struct ApplicationContext(mpsc::Sender<ffi::QuickFixMessage>);

impl ApplicationContext {
    pub fn new() -> (Self, mpsc::Receiver<ffi::QuickFixMessage>) {
        let (tx, rx) = mpsc::channel();
        (Self(tx), rx)
    }

    pub fn inbound(&self, message: ffi::QuickFixMessage, session_id: &ffi::SessionID) {
        self.0.send(message).unwrap();
    }
}
