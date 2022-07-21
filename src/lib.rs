#![allow(non_upper_case_globals, non_camel_case_types, non_snake_case)]

mod trade_client;

use std::fmt;

pub use trade_client::{TradeClient, TradeClientContext};

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cxx::bridge]
pub mod ffi {
    #[derive(Copy, Clone, Debug, Eq, PartialEq, Hash)]
    enum TradeClientType {
        Apifiny = 1,
        Wintmute,
    }

    #[derive(Copy, Clone, Debug, Eq, PartialEq, Hash)]
    enum FixMessageType {
        Admin,
        App,
    }

    struct QuickFixMessage {
        content: UniquePtr<CxxString>,
        session_id: UniquePtr<SessionID>,
        from: FixMessageType,
    }

    #[namespace = "FIX"]
    unsafe extern "C++" {
        include!("quickfix-rs/vendor/quickfix-cpp/src/C++/SessionID.h");

        type SessionID;

        #[rust_name = "to_string_frozen"]
        fn toStringFrozen(self: &SessionID) -> &CxxString;
    }

    unsafe extern "C++" {
        include!("quickfix-rs/cxx/ITradeclient.h");

        type ITradeClient;

        #[allow(clippy::borrowed_box)]
        fn create_client(
            client_type: TradeClientType,
            file_path: &CxxString,
            ctx: Box<TradeClientContext>,
            inbound_callback: fn(message: QuickFixMessage, ctx: &Box<TradeClientContext>),
        ) -> UniquePtr<ITradeClient>;

        fn start(self: &ITradeClient);
        fn stop(self: &ITradeClient);

        fn put_order(
            self: &ITradeClient,
            symbol: &CxxString,
            side: c_char,
            quantity: f64,
            price: f64,
            time_in_force: c_char,
        ) -> UniquePtr<CxxString>;
        fn cancel_order(self: &ITradeClient, order_id: &CxxString, session_id: &SessionID);
    }

    extern "Rust" {
        type TradeClientContext;

        fn inbound(&self, message: QuickFixMessage);
    }
}

impl fmt::Debug for ffi::QuickFixMessage {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("QuickFixMessage")
            .field("content", &self.content)
            .field("session_id", self.session_id.to_string_frozen())
            .field("from", &self.from)
            .finish()
    }
}

unsafe impl Send for ffi::SessionID {}
unsafe impl Sync for ffi::SessionID {}
unsafe impl Send for ffi::ITradeClient {}
unsafe impl Sync for ffi::ITradeClient {}
