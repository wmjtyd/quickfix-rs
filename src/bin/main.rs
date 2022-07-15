use std::{thread, time::Duration};

use cxx::let_cxx_string;

use quickfix_rs::{ffi, generate_order_id, ApplicationContext};

fn main() {
    let account_id = "STA-01251839";

    let_cxx_string!(order_id = generate_order_id(account_id));
    let_cxx_string!(filepath = "tradeclient.cfg");
    let_cxx_string!(quote_id = "");
    let_cxx_string!(symbol = "BTCUSDT");
    let_cxx_string!(currency = "");

    let (app_ctx, rx) = ApplicationContext::new();
    let trade_client = ffi::create_client(
        &filepath,
        Box::new(app_ctx),
        |message, session_id, app_ctx| app_ctx.inbound(message, session_id),
    );
    trade_client.start();

    thread::sleep(Duration::from_secs(3));
    trade_client.put_order(
        &order_id, &quote_id, &symbol, &currency, '1' as u32, 1, 1, '3' as u32,
    );

    for message in rx {
        dbg!(message);
    }

    trade_client.stop();
}
