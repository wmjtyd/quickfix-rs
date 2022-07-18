use std::{path::Path, thread, time::Duration};

use quickfix_rs::TradeClient;

fn main() {
    let trade_client = TradeClient::new(1, Path::new("cxx/tradeclient-apifiny/tradeclient.cfg"));

    trade_client.start();
    thread::sleep(Duration::from_secs(3));
    let order_id = trade_client.put_order("", "BTCUSDT", "", '1' as u32, 1, 1, '3' as u32);
    dbg!(order_id);

    trade_client.poll_response();
}
