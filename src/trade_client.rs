use std::{
    os::{raw::c_char, unix::prelude::OsStrExt},
    path::Path,
};

use cxx::{let_cxx_string, UniquePtr};
use tokio::sync::{mpsc, oneshot, Mutex};
use tracing::{debug, instrument, warn};

use crate::ffi;

pub enum OrderMessage {
    NewOrder {
        symbol: String,
        side: c_char,
        quantity: f64,
        price: f64,
        time_in_force: c_char,
        tx: oneshot::Sender<cxx::UniquePtr<cxx::CxxString>>,
    },
    CancelOrder {
        order_id: cxx::CxxString,
        session_id: ffi::SessionID,
    },
}

pub struct TradingClientContext(mpsc::UnboundedSender<ffi::QuickFixMessage>);

impl TradingClientContext {
    pub fn new(order_report_tx: mpsc::UnboundedSender<ffi::QuickFixMessage>) -> Self {
        Self(order_report_tx)
    }

    pub fn inbound(&self, message: ffi::QuickFixMessage) {
        self.0.send(message).unwrap();
    }
}

pub struct TradingClient {
    cxx_inner: UniquePtr<ffi::ITradeClient>,
    order_rx: Mutex<mpsc::UnboundedReceiver<OrderMessage>>,
}

impl TradingClient {
    pub fn new(
        client_type: ffi::TradingClientType,
        file_path: &Path,
        order_rx: mpsc::UnboundedReceiver<OrderMessage>,
        order_report_tx: mpsc::UnboundedSender<ffi::QuickFixMessage>,
    ) -> Self {
        let_cxx_string!(file_path = file_path.as_os_str().as_bytes());
        let ctx = TradingClientContext::new(order_report_tx);
        let cxx_inner =
            ffi::create_client(client_type, &file_path, Box::new(ctx), |message, ctx| {
                ctx.inbound(message)
            });

        Self {
            cxx_inner,
            order_rx: Mutex::new(order_rx),
        }
    }

    pub fn start(&self) {
        self.cxx_inner.start();
    }

    pub fn stop(&self) {
        self.cxx_inner.stop();
    }

    #[instrument(skip(self))]
    pub fn new_order(
        &self,
        symbol: &cxx::CxxString,
        side: c_char,
        quantity: f64,
        price: f64,
        time_in_force: c_char,
    ) -> cxx::UniquePtr<cxx::CxxString> {
        self.cxx_inner
            .put_order(symbol, side, quantity, price, time_in_force)
    }

    #[instrument(skip(self, session_id))]
    pub fn cancel_order(&self, order_id: &cxx::CxxString, session_id: &ffi::SessionID) {
        self.cxx_inner.cancel_order(order_id, session_id);
    }

    #[instrument(skip(self))]
    pub async fn poll_order(&self) {
        let mut order_rx = self.order_rx.lock().await;
        while let Some(order) = order_rx.recv().await {
            match order {
                OrderMessage::NewOrder {
                    symbol,
                    side,
                    quantity,
                    price,
                    time_in_force,
                    tx,
                } => {
                    let_cxx_string!(symbol = symbol);
                    let order_id = self.new_order(&symbol, side, quantity, price, time_in_force);
                    tx.send(order_id).unwrap();
                }
                OrderMessage::CancelOrder {
                    order_id,
                    session_id,
                } => {
                    self.cancel_order(&order_id, &session_id);
                }
            }
        }
    }
}

impl Drop for TradingClient {
    fn drop(&mut self) {
        // TODO: Stop multiple times
        self.stop();
    }
}
