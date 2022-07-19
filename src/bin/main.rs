use std::{path::Path, thread, time::Duration};

use quickfix_rs::{FIX_Side_BUY, FIX_TimeInForce_AT_THE_OPENING, TradeClient};

fn main() {
    let trade_client = TradeClient::new(1, Path::new("cxx/tradeclient-apifiny/tradeclient.cfg"));

    trade_client.start();
    thread::sleep(Duration::from_secs(3));
    let order_id = trade_client.put_order(
        "",
        "BTCUSDT",
        "",
        FIX_Side_BUY,
        1,
        1,
        FIX_TimeInForce_AT_THE_OPENING,
    );
    dbg!(order_id);

    trade_client.poll_response();
}
