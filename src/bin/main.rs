use cxx::let_cxx_string;
use quickfix_rs::ffi;

fn main() {
    let_cxx_string!(filepath = "tradeclient.cfg");
    let mut trade_client = ffi::create_client(&filepath);

    trade_client.pin_mut().run();
}
