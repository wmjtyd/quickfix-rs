#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("quickfix-rs/cxx/include/tradeclient.h");

        type TradeClient;

        fn create_client(filepath: &CxxString) -> UniquePtr<TradeClient>;
        fn run(self: Pin<&mut TradeClient>) -> i32;
        fn put_order(
            self: Pin<&mut TradeClient>,
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

#[cfg(test)]
mod tests {
    use cxx::let_cxx_string;

    use super::ffi;

    #[test]
    fn test_nothing() {
        let_cxx_string!(filepath = "fix42.xml");
        let mut trade_client = ffi::create_client(&filepath);

        trade_client.pin_mut().run();
    }
}
