use std::{path::Path, thread, time::Duration};

use cxx::let_cxx_string;

use quickfix_rs::{
    ffi::TradeClientType, FIX_Side_BUY, FIX_TimeInForce_AT_THE_OPENING, TradeClient,
};

fn main() {
    tracing_subscriber::fmt::init();

    let mut trade_client = TradeClient::new(
        TradeClientType::Apifiny,
        Path::new("cxx/tradeclient-apifiny/tradeclient.cfg"),
    );

    // 阻塞直到登陆
    // 中途短线怎么办
    trade_client.start();
    thread::sleep(Duration::from_secs(3));

    let_cxx_string!(symbol = "BTCUSDT");
    trade_client.put_order(
        &symbol,
        FIX_Side_BUY,
        0.0001,
        100000.0,
        FIX_TimeInForce_AT_THE_OPENING,
    );

    trade_client.poll_response();
}
