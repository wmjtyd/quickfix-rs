use std::{
    os::{raw::c_char, unix::prelude::OsStrExt},
    path::Path,
};

use cxx::{let_cxx_string, UniquePtr};
use tokio::sync::{mpsc, Mutex};
use tracing::{instrument, warn};

use crate::ffi;

#[derive(Clone, Debug)]
pub enum OrderMessage {
    NewOrder {
        symbol: String,
        side: c_char,
        quantity: f64,
        price: f64,
        stop_price: f64,
        order_type: c_char,
        time_in_force: c_char,
    },
    CancelOrder {
        symbol: String,
        order_id: String,
    },
}

pub struct TradingClientContext(mpsc::UnboundedSender<ffi::RustExecutionReport>);

impl TradingClientContext {
    pub fn new(order_report_tx: mpsc::UnboundedSender<ffi::RustExecutionReport>) -> Self {
        Self(order_report_tx)
    }

    pub fn inbound(&self, message: ffi::RustExecutionReport) {
        self.0.send(message).unwrap();
    }
}

pub struct TradingClient {
    cxx_inner: UniquePtr<ffi::ITradeClient>,
    order_rx: Mutex<mpsc::UnboundedReceiver<OrderMessage>>,
}

impl TradingClient {
    pub fn new(
        exchange_name: &str,
        client_type: ffi::TradingClientType,
        file_path: &Path,
        order_rx: mpsc::UnboundedReceiver<OrderMessage>,
        order_report_tx: mpsc::UnboundedSender<ffi::RustExecutionReport>,
    ) -> Self {
        let_cxx_string!(file_path = file_path.as_os_str().as_bytes());
        let_cxx_string!(exchange_name = exchange_name);
        let ctx = TradingClientContext::new(order_report_tx);
        let cxx_inner =
            ffi::create_client(&exchange_name, client_type, &file_path, Box::new(ctx), |message, ctx| {
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
        self: &TradingClient,
        symbol: &str,
        side: c_char,
        quantity: f64,
        price: f64,
        stop_price: f64,
        order_type: c_char,
        time_in_force: c_char,
    ) -> cxx::UniquePtr<cxx::CxxString> {
        let_cxx_string!(symbol = symbol);
        self.cxx_inner.put_order(
            &symbol,
            side,
            quantity,
            price,
            stop_price,
            order_type,
            time_in_force,
        )
    }

    #[instrument(skip(self))]
    pub fn cancel_order(&self, symbol: &str, order_id: &str) {
        let_cxx_string!(symbol = symbol);
        let_cxx_string!(order_id = order_id);
        self.cxx_inner.cancel_order(&symbol, &order_id);
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
                    stop_price,
                    order_type,
                    time_in_force,
                } => {
                    let _ = self.new_order(
                        &symbol,
                        side,
                        quantity,
                        price,
                        stop_price,
                        order_type,
                        time_in_force,
                    );
                }
                OrderMessage::CancelOrder { symbol, order_id } => {
                    self.cancel_order(&symbol, &order_id);
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
