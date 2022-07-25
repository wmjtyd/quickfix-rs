use std::path::Path;

use tokio::{sync::mpsc, task, time};
use tracing::{error, info};

use quickfix_rs::{
    ffi::TradingClientType, FIX_Side_SELL, FIX_TimeInForce_AT_THE_OPENING, OrderMessage,
    TradingClient,
};

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt::init();

    let (order_tx, order_rx) = mpsc::unbounded_channel();
    let (order_report_tx, mut order_report_rx) = mpsc::unbounded_channel();
    let trading_client = TradingClient::new(
        TradingClientType::Apifiny,
        Path::new("cxx/tradeclient-apifiny/tradeclient.cfg"),
        order_rx,
        order_report_tx,
    );
    trading_client.start();

    let _handle = task::spawn(async move {
        time::sleep(time::Duration::from_secs(3)).await;

        let new_order_message = OrderMessage::NewOrder {
            symbol: "BTCUSDT".to_owned(),
            side: FIX_Side_SELL,
            quantity: 0.00000001,
            price: 100000.0,
            time_in_force: FIX_TimeInForce_AT_THE_OPENING,
        };
        if order_tx.send(new_order_message).is_err() {
            error!("Order receiver closed");
        }

        while let Some(order_report) = order_report_rx.recv().await {
            info!(?order_report);

            // let cancel_order_message = OrderMessage::CancelOrder {};
        }
    });

    trading_client.poll_order().await;
}
