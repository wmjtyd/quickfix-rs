#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        include!("quickfix-rs/cxx/include/Tradeclient.h");

        type TradeClient;

        fn create_client(filepath: &CxxString) -> UniquePtr<TradeClient>;
        fn run(self: Pin<&mut TradeClient>) -> bool;
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
