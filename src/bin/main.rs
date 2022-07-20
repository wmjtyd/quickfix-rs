use std::{path::Path, thread, time::Duration};

use cxx::let_cxx_string;
use fefix::{
    definitions::fix42,
    tagvalue::{Config, Decoder, FieldAccess},
    Dictionary,
};
use tracing::info;

use quickfix_rs::{
    ffi::TradeClientType, FIX_Side_BUY, FIX_Side_SELL, FIX_TimeInForce_AT_THE_OPENING, TradeClient,
};

fn main() {
    tracing_subscriber::fmt::init();

    let trade_client = TradeClient::new(
        TradeClientType::Apifiny,
        Path::new("cxx/tradeclient-apifiny/tradeclient.cfg"),
    );
    trade_client.start();

    let trade_client_clone = trade_client.clone();
    thread::spawn(move || {
        thread::sleep(Duration::from_secs(3));

        let_cxx_string!(symbol = "BTCUSDT");
        let (content, session_id) = trade_client_clone.put_order(
            &symbol,
            FIX_Side_BUY,
            0.0001,
            100000.0,
            FIX_TimeInForce_AT_THE_OPENING,
        );

        let fix_dictionary = Dictionary::fix42();
        let mut fix_decoder = Decoder::<Config>::new(fix_dictionary);
        let order_id = handle_message(&mut fix_decoder, content);

        let_cxx_string!(order_id = order_id);
        let (content, _) =
            trade_client_clone.cancel_order(&order_id, &symbol, FIX_Side_SELL, &session_id);
        let _ = handle_message(&mut fix_decoder, content);
    });

    trade_client.poll_response();
}

fn handle_message(fix_decoder: &mut Decoder, content: cxx::UniquePtr<cxx::CxxString>) -> String {
    let message = fix_decoder.decode(content.as_bytes()).unwrap();
    let order_id = message.fv::<&str>(fix42::CL_ORD_ID).unwrap();
    match message.fv(fix42::MSG_TYPE).unwrap() {
        fix42::MsgType::ExecutionReport => {
            let exec_type = message.fv(fix42::EXEC_TYPE).unwrap();
            match exec_type {
                fix42::ExecType::New => {
                    info!("Order new placed {}", order_id);
                }
                fix42::ExecType::Canceled => {
                    info!("Order canceled {}", order_id);
                }
                fix42::ExecType::Rejected => {
                    let reason = message.fv::<&str>(fix42::TEXT).unwrap();
                    info!("Faild to put new order {}: {}", order_id, reason);
                }
                _ => todo!(),
            }
        }
        fix42::MsgType::OrderCancelReject => {
            let reason = message.fv::<&str>(fix42::TEXT).unwrap();
            info!("Faild to cancel order {}: {}", order_id, reason);
        }
        _ => unreachable!(),
    }

    order_id.to_owned()
}
