use std::{thread, time::Duration};

use cxx::let_cxx_string;
use quickfix_rs::ffi;

fn main() {
    let_cxx_string!(filepath = "tradeclient.cfg");
    let_cxx_string!(quote_id = "quote_id");
    let_cxx_string!(symbol = "BTCUSDT");
    let_cxx_string!(currency = "currency");

    let trade_client = ffi::create_client(&filepath);
    trade_client.start();

    loop {
        trade_client.put_order(&quote_id, &symbol, &currency, 1, 1, 1, 1);
        thread::sleep(Duration::from_secs(10));
    }
}
