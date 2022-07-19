use std::{
    cell::RefCell,
    collections::HashSet,
    os::{raw::c_char, unix::prelude::OsStrExt},
    path::Path,
    sync::mpsc,
};

use cxx::{let_cxx_string, UniquePtr};
use fefix::{
    definitions::fix42,
    tagvalue::{Config, Decoder, FieldAccess},
    Dictionary,
};
use tracing::{debug, info, warn};

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
    fix_decoder: Decoder,
    inner: UniquePtr<ffi::ITradeClient>,
    // TODO: 如果写如已经存在的 order id 怎么办
    pending_requests: RefCell<HashSet<String>>,

    rx: mpsc::Receiver<ffi::QuickFixMessage>,
}

impl TradeClient {
    pub fn new(client_type: ffi::TradeClientType, file_path: &Path) -> Self {
        let fix_dictionary = Dictionary::fix42();
        let fix_decoder = Decoder::<Config>::new(fix_dictionary);

        let_cxx_string!(file_path = file_path.as_os_str().as_bytes());
        let (ctx, rx) = TradeClientContext::new();
        let inner = ffi::create_client(client_type, &file_path, Box::new(ctx), |message, ctx| {
            ctx.inbound(message)
        });

        Self {
            fix_decoder,
            inner,
            pending_requests: RefCell::default(),
            rx,
        }
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
        quantity: f64,
        price: f64,
        time_in_force: c_char,
    ) {
        let order_id = self
            .inner
            .put_order(symbol, side, quantity, price, time_in_force);

        let order_id = order_id.to_string_lossy().into_owned();
        let _ = self.pending_requests.borrow_mut().insert(order_id);
    }

    pub fn cancel_order(
        &self,
        order_id: &cxx::CxxString,
        symbol: &cxx::CxxString,
        side: c_char,
        session_id: &ffi::SessionID,
    ) {
        self.inner.cancel_order(order_id, symbol, side, session_id);

        let order_id = order_id.to_string_lossy().into_owned();
        let _ = self.pending_requests.borrow_mut().insert(order_id);
    }

    pub fn poll_response(&mut self) {
        for ffi::QuickFixMessage {
            content,
            session_id,
            from,
        } in &self.rx
        {
            debug!(%content, session_id = %session_id.to_string_frozen(), from = ?from);

            let message = self
                .fix_decoder
                .decode(content.as_bytes())
                .expect("Invalid FIX message");
            if let Ok(message_type) = message.fv(fix42::MSG_TYPE) {
                match message_type {
                    fix42::MsgType::ExecutionReport => {
                        let order_id = message.fv::<&str>(fix42::CL_ORD_ID).unwrap();
                        if !self.pending_requests.borrow_mut().remove(order_id) {
                            warn!("order {} is not in pending requests", order_id);
                        }

                        let exec_type = message.fv(fix42::EXEC_TYPE).unwrap();
                        match exec_type {
                            fix42::ExecType::New => {
                                let_cxx_string!(order_id = order_id);
                                let_cxx_string!(symbol = "BTCUSDT");

                                self.cancel_order(
                                    &order_id,
                                    &symbol,
                                    super::FIX_Side_SELL,
                                    &session_id,
                                );
                            }
                            fix42::ExecType::Canceled => {
                                info!("Order {} canceled", order_id);
                            }
                            fix42::ExecType::Rejected => {
                                let reason = message.fv::<&str>(fix42::TEXT).unwrap();
                                info!("Faild to put new order {}: {}", order_id, reason);
                            }
                            _ => todo!(),
                        }
                    }
                    fix42::MsgType::OrderCancelReject => {
                        let order_id = message.fv::<&str>(fix42::CL_ORD_ID).unwrap();
                        if !self.pending_requests.borrow_mut().remove(order_id) {
                            warn!("order {} is not in pending requests", order_id);
                        }

                        let reason = message.fv::<&str>(fix42::TEXT).unwrap();
                        info!("Faild to cancel order {}: {}", order_id, reason);
                    }
                    fix42::MsgType::Heartbeat => {
                        info!("Heartbeat");
                    }
                    fix42::MsgType::Logon => {
                        info!("Logon");
                    }
                    fix42::MsgType::Logout => {
                        info!("Logout");
                    }
                    _ => todo!("{:?}", message_type),
                }
            }
        }
    }
}

impl Drop for TradeClient {
    fn drop(&mut self) {
        // TODO: stop multiple times
        self.stop();
    }
}
