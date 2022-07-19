use std::{
    os::{raw::c_char, unix::prelude::OsStrExt},
    path::Path,
    sync::mpsc,
};

use cxx::{let_cxx_string, UniquePtr};

use crate::ffi;

pub struct TradeClientContext(mpsc::Sender<ffi::QuickFixMessage>);

impl TradeClientContext {
    pub fn new() -> (Self, mpsc::Receiver<ffi::QuickFixMessage>) {
        let (tx, rx) = mpsc::channel();
        (Self(tx), rx)
    }

    pub fn inbound(&self, message: ffi::QuickFixMessage, _session_id: &ffi::SessionID) {
        self.0.send(message).unwrap();
    }
}

pub struct TradeClient {
    inner: UniquePtr<ffi::ITradeClient>,
    rx: mpsc::Receiver<ffi::QuickFixMessage>,
}

impl TradeClient {
    pub fn new(client_type: u32, file_path: &Path) -> Self {
        let_cxx_string!(file_path = file_path.as_os_str().as_bytes());
        let (ctx, rx) = TradeClientContext::new();

        let inner = ffi::create_client(
            client_type,
            &file_path,
            Box::new(ctx),
            |message, session_id, ctx| ctx.inbound(message, session_id),
        );

        Self { inner, rx }
    }

    pub fn start(&self) {
        self.inner.start();
    }

    pub fn stop(&self) {
        self.inner.stop();
    }

    #[allow(clippy::too_many_arguments)]
    pub fn put_order(
        &self,
        quote_id: &str,
        symbol: &str,
        currency: &str,
        side: c_char,
        quantity: u32,
        price: u32,
        time_in_force: c_char,
    ) -> String {
        let_cxx_string!(quote_id = quote_id);
        let_cxx_string!(symbol = symbol);
        let_cxx_string!(currency = currency);

        let order_id = self.inner.put_order(
            &quote_id,
            &symbol,
            &currency,
            side,
            quantity,
            price,
            time_in_force,
        );

        order_id.to_string()
    }

    pub fn poll_response(&self) {
        for message in &self.rx {
            dbg!(message);
        }
    }
}

impl Drop for TradeClient {
    fn drop(&mut self) {
        self.stop();
    }
}
