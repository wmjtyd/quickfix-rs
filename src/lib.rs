#![allow(non_upper_case_globals, non_camel_case_types, non_snake_case)]

mod trade_client;
pub use trade_client::{OrderMessage, TradingClient, TradingClientContext};

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cxx::bridge]
pub mod ffi {

    #[derive(Debug)]
    struct RustExecutionReport {
        exec_type: c_char,
        symbol: String,
        order_id: String,
        client_ord_id: String,
        exec_id: String,

        cum_qty: f64,  //Total quantity (e.g. number of shares) filled. //https://www.onixs.biz/fix-dictionary/4.4/tagNum_14.html
        order_qty: f64, // TODO 这个后续核对fix对应字段的名称
        ord_status: c_char,
        side: c_char,
        asset_free: f64,
        asset_locked: f64,


        error_message: String,
    }


    #[derive(Copy, Clone, Debug, Eq, PartialEq, Hash)]
    enum TradingClientType {
        Apifiny = 1,
        Wintmute,
        CCApi,
    }

    #[derive(Copy, Clone, Debug, Eq, PartialEq, Hash)]
    pub enum OrderType {
        Market = 1,
        Limit,
        Stop,
        StopLimit,
    }

    #[derive(Copy, Clone, Debug, Eq, PartialEq, Hash)]
    enum FixMessageType {
        Admin,
        App,
    }

    #[derive(Debug)]
    struct QuickFixMessage {
        content: UniquePtr<CxxString>,
        from: FixMessageType,
    }

    unsafe extern "C++" {
        include!("quickfix-rs/cxx/ITradeclient.h");

        type ITradeClient;
        type OrderCancelRequest;
        type ExecutionReport;

        #[allow(clippy::borrowed_box)]
        fn create_client(
            exchange_name: &CxxString,
            client_type: TradingClientType,
            file_path: &CxxString,
            ctx: Box<TradingClientContext>,
            inbound_callback: fn(message: RustExecutionReport, ctx: &Box<TradingClientContext>),
        ) -> UniquePtr<ITradeClient>;

        fn start(self: &ITradeClient);
        fn stop(self: &ITradeClient);

        fn put_order(
            self: &ITradeClient,
            symbol: &CxxString,
            side: c_char,
            quantity: f64,
            price: f64,
            stop_price: f64,
            order_type: c_char,
            time_in_force: c_char,
        ) -> UniquePtr<CxxString>;

        fn cancel_order(self: &ITradeClient, symbol: &CxxString, order_id: &CxxString);
        fn cancel_open_orders(self: &ITradeClient, symbol: &CxxString);

        fn get_account_postions(self: &ITradeClient);

        fn get_order(self: &ITradeClient, symbol: &CxxString, order_id: &CxxString);
        fn get_open_orders(self: &ITradeClient, symbol: &CxxString);

        fn get_recent_trades(self: &ITradeClient, symbol: &CxxString);

        fn get_account_balances(self: &ITradeClient);

        fn get_accounts(self: &ITradeClient);
    }

    extern "Rust" {
        type TradingClientContext;

        fn inbound(&self, message: RustExecutionReport);
    }
}

unsafe impl Send for ffi::ITradeClient {}
unsafe impl Sync for ffi::ITradeClient {}
