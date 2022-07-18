mod trade_client;

pub use trade_client::{TradeClient, TradeClientContext};

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

        fn start(&self);
        fn stop(&self);

        fn put_order(
            self: &ITradeClient,
            quote_id: &CxxString,
            symbol: &CxxString,
            currency: &CxxString,
            side: u32,
            quantity: u32,
            price: u32,
            time_in_force: u32,
        ) -> UniquePtr<CxxString>;
    }

    extern "Rust" {
        type TradeClientContext;

        fn inbound(&self, message: QuickFixMessage, session_id: &SessionID);
    }
}
