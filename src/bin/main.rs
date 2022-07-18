use std::{path::Path, thread, time::Duration};

use cxx::let_cxx_string;

use quickfix_rs::{ffi, TradeClient};

fn main() {
    let trade_client = TradeClient::new(1, Path::new("cxx/tradeclient-apifiny/tradeclient.cfg"));
    trade_client.start();

    // thread::sleep(Duration::from_secs(3));
    // let account_id = "STA-01251839";
    // let_cxx_string!(order_id = generate_order_id(account_id));
    // let_cxx_string!(quote_id = "");
    // let_cxx_string!(symbol = "BTCUSDT");
    // let_cxx_string!(currency = "");
    // trade_client.put_order(
    //     &order_id, &quote_id, &symbol, &currency, '1' as u32, 1, 1, '3' as u32,
    // );

    trade_client.poll_response();
}
