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

    pub fn inbound(&self, message: ffi::QuickFixMessage) {
        self.0.send(message).unwrap();
    }
}

pub struct TradeClient {
    inner: UniquePtr<ffi::ITradeClient>,
    rx: mpsc::Receiver<ffi::QuickFixMessage>,
}

impl TradeClient {
    pub fn new(client_type: ffi::TradeClientType, file_path: &Path) -> Self {
        let_cxx_string!(file_path = file_path.as_os_str().as_bytes());
        let (ctx, rx) = TradeClientContext::new();

        let inner = ffi::create_client(client_type, &file_path, Box::new(ctx), |message, ctx| {
            ctx.inbound(message)
        });

        Self { inner, rx }
    }

    pub fn start(&self) {
        self.inner.start();
    }

    pub fn stop(&self) {
        self.inner.stop();
    }

    pub fn put_order(
        &self,
        symbol: &cxx::CxxString,
        side: c_char,
        quantity: u32,
        price: u32,
        time_in_force: c_char,
    ) -> cxx::UniquePtr<cxx::CxxString> {
        self.inner
            .put_order(symbol, side, quantity, price, time_in_force)
    }

    pub fn cancel_order(
        &self,
        order_id: &cxx::CxxString,
        symbol: &cxx::CxxString,
        side: c_char,
        session_id: &ffi::SessionID,
    ) {
        self.inner.cancel_order(order_id, symbol, side, session_id);
    }

    pub fn recv(&self) -> ffi::QuickFixMessage {
        self.rx.recv().unwrap()
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
