#![allow(non_upper_case_globals, non_camel_case_types, non_snake_case)]

mod trade_client;

pub use trade_client::{TradeClient, TradeClientContext};

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

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
        include!("quickfix-rs/cxx/ITradeclient.h");

        type ITradeClient;

        #[allow(clippy::borrowed_box)]
        fn create_client(
            client_type: u32,
            file_path: &CxxString,
            ctx: Box<TradeClientContext>,
            inbound_callback: fn(
                message: QuickFixMessage,
                session_id: &SessionID,
                ctx: &Box<TradeClientContext>,
            ),
        ) -> UniquePtr<ITradeClient>;

        fn start(self: &ITradeClient);
        fn stop(self: &ITradeClient);

        fn put_order(
            self: &ITradeClient,
            symbol: &CxxString,
            side: c_char,
            quantity: u32,
            price: u32,
            time_in_force: c_char,
        ) -> UniquePtr<CxxString>;
    }

    extern "Rust" {
        type TradeClientContext;

        fn inbound(&self, message: QuickFixMessage, session_id: &SessionID);
    }
}
