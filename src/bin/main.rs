use std::{thread, time::Duration};

use cxx::let_cxx_string;

use quickfix_rs::{ffi, generate_order_id};

fn main() {
    let account_id = "STA-01251839";

    let_cxx_string!(order_id = generate_order_id(account_id));
    let_cxx_string!(filepath = "tradeclient.cfg");
    let_cxx_string!(quote_id = "");
    let_cxx_string!(symbol = "BTCUSDT");
    let_cxx_string!(currency = "");

    let trade_client = ffi::create_client(&filepath);
    trade_client.start();

    loop {
        thread::sleep(Duration::from_secs(10));
        trade_client.put_order(
            &order_id, &quote_id, &symbol, &currency, '1' as i32, 1, 1, '3' as i32,
        );
    }

    trade_client.stop();
}
