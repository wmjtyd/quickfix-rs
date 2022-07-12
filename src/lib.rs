use rand::prelude::*;

use std::time::{SystemTime, UNIX_EPOCH};

#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        include!("quickfix-rs/cxx/include/Tradeclient.h");

        type TradeClient;

        fn create_client(filepath: &CxxString) -> UniquePtr<TradeClient>;

        fn start(&self);
        fn stop(&self);

        fn put_order(
            self: &TradeClient,
            order_id: &CxxString,
            quote_id: &CxxString,
            symbol: &CxxString,
            currency: &CxxString,
            side: i32,
            quantity: i32,
            price: i32,
            time_in_force: i32,
        );
    }
}

pub fn generate_order_id(account_id: &str) -> String {
    let account_id = account_id.split('-').last().expect("Must have number part");
    let timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("Time went backwards")
        .as_millis();
    let random_number = random::<u16>() / 1000;

    format!("{}{:.13}{:03}", account_id, timestamp, random_number)
}
