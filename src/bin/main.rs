use std::{path::Path, thread, time::Duration};

use cxx::let_cxx_string;

use quickfix_rs::{
    ffi::{FixMessageType, TradeClientType},
    FIX_Side_BUY, FIX_Side_SELL, FIX_TimeInForce_AT_THE_OPENING, TradeClient,
};

fn main() {
    let trade_client = TradeClient::new(
        TradeClientType::Apifiny,
        Path::new("cxx/tradeclient-apifiny/tradeclient.cfg"),
    );

    trade_client.start();
    thread::sleep(Duration::from_secs(3));

    let_cxx_string!(symbol = "BTCUSDT");
    let order_id =
        trade_client.put_order(&symbol, FIX_Side_BUY, 1, 1, FIX_TimeInForce_AT_THE_OPENING);
    dbg!(&order_id);

    loop {
        let response = trade_client.recv();
        if response.from == FixMessageType::App {
            trade_client.cancel_order(&order_id, &symbol, FIX_Side_SELL, &response.session_id);
            break;
        }
    }

    trade_client.poll_response();
}
