use std::{
    collections::HashMap,
    os::{raw::c_char, unix::prelude::OsStrExt},
    path::Path,
    sync::{Arc, Mutex},
};

use crossbeam::channel;
use cxx::{let_cxx_string, UniquePtr};
use fefix::{
    definitions::fix42,
    tagvalue::{Config, Decoder, FieldAccess},
    Dictionary,
};
use tracing::{debug, error, instrument, warn};

use crate::ffi;

pub struct TradeClientContext(channel::Sender<ffi::QuickFixMessage>);

impl TradeClientContext {
    pub fn new() -> (Self, channel::Receiver<ffi::QuickFixMessage>) {
        let (tx, rx) = channel::unbounded();
        (Self(tx), rx)
    }

    pub fn inbound(&self, message: ffi::QuickFixMessage) {
        self.0.send(message).unwrap();
    }
}

#[derive(Clone)]
pub struct TradeClient {
    inner: Arc<TradeClientInner>,
}

impl TradeClient {
    pub fn new(client_type: ffi::TradeClientType, file_path: &Path) -> Self {
        let inner = TradeClientInner::new(client_type, file_path);
        Self {
            inner: Arc::new(inner),
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
    ) -> (
        cxx::UniquePtr<cxx::CxxString>,
        cxx::UniquePtr<ffi::SessionID>,
    ) {
        self.inner
            .put_order(symbol, side, quantity, price, time_in_force)
    }

    pub fn cancel_order(
        &self,
        order_id: &cxx::CxxString,
        symbol: &cxx::CxxString,
        side: c_char,
        session_id: &ffi::SessionID,
    ) -> (
        cxx::UniquePtr<cxx::CxxString>,
        cxx::UniquePtr<ffi::SessionID>,
    ) {
        self.inner.cancel_order(order_id, symbol, side, session_id)
    }

    pub fn poll_response(&self) {
        self.inner.poll_response();
    }
}

struct TradeClientInner {
    cxx_inner: UniquePtr<ffi::ITradeClient>,
    pending_requests: Mutex<
        HashMap<
            String,
            channel::Sender<(
                cxx::UniquePtr<cxx::CxxString>,
                cxx::UniquePtr<ffi::SessionID>,
            )>,
        >,
    >,

    rx: channel::Receiver<ffi::QuickFixMessage>,
}

impl TradeClientInner {
    pub fn new(client_type: ffi::TradeClientType, file_path: &Path) -> Self {
        let_cxx_string!(file_path = file_path.as_os_str().as_bytes());
        let (ctx, rx) = TradeClientContext::new();
        let cxx_inner =
            ffi::create_client(client_type, &file_path, Box::new(ctx), |message, ctx| {
                ctx.inbound(message)
            });

        Self {
            cxx_inner,
            pending_requests: Mutex::default(),
            rx,
        }
    }

    pub fn start(&self) {
        self.cxx_inner.start();
    }

    pub fn stop(&self) {
        self.cxx_inner.stop();
    }

    #[instrument(skip(self))]
    pub fn put_order(
        &self,
        symbol: &cxx::CxxString,
        side: c_char,
        quantity: f64,
        price: f64,
        time_in_force: c_char,
    ) -> (
        cxx::UniquePtr<cxx::CxxString>,
        cxx::UniquePtr<ffi::SessionID>,
    ) {
        let order_id = self
            .cxx_inner
            .put_order(symbol, side, quantity, price, time_in_force);

        let order_id = order_id.to_string_lossy().into_owned();
        let (tx, rx) = channel::bounded(1);

        if self
            .pending_requests
            .lock()
            .unwrap()
            .insert(order_id, tx)
            .is_some()
        {
            error!("Overwrite exists order id");
        }

        rx.recv().unwrap()
    }

    #[instrument(skip(self, session_id))]
    pub fn cancel_order(
        &self,
        order_id: &cxx::CxxString,
        symbol: &cxx::CxxString,
        side: c_char,
        session_id: &ffi::SessionID,
    ) -> (
        cxx::UniquePtr<cxx::CxxString>,
        cxx::UniquePtr<ffi::SessionID>,
    ) {
        self.cxx_inner
            .cancel_order(order_id, symbol, side, session_id);

        let order_id = order_id.to_string_lossy().into_owned();
        let (tx, rx) = channel::bounded(1);

        if self
            .pending_requests
            .lock()
            .unwrap()
            .insert(order_id, tx)
            .is_some()
        {
            error!("Overwrite exists order id");
        }

        rx.recv().unwrap()
    }

    pub fn poll_response(&self) {
        let fix_dictionary = Dictionary::fix42();
        let mut fix_decoder = Decoder::<Config>::new(fix_dictionary);

        for ffi::QuickFixMessage {
            content,
            session_id,
            from,
        } in &self.rx
        {
            debug!(session_id = %session_id.to_string_frozen(), from = ?from, %content);

            let message = fix_decoder
                .decode(content.as_bytes())
                .expect("Invalid FIX message");

            if let Ok(message_type) = message.fv(fix42::MSG_TYPE) {
                match message_type {
                    fix42::MsgType::ExecutionReport | fix42::MsgType::OrderCancelReject => {
                        let order_id = message.fv::<&str>(fix42::CL_ORD_ID).unwrap();
                        match self.pending_requests.lock().unwrap().remove(order_id) {
                            Some(tx) => {
                                if let Err(_) = tx.send((content, session_id)) {
                                    error!("rx droppped");
                                }
                            }
                            None => warn!("Order {} is not in pending requests", order_id),
                        }
                    }
                    fix42::MsgType::Heartbeat => debug!("Heartbeat"),
                    fix42::MsgType::Logon => debug!("Logon"),
                    fix42::MsgType::Logout => debug!("Logout"),
                    _ => todo!("{:?}", message_type),
                }
            }
        }
    }
}

impl Drop for TradeClientInner {
    fn drop(&mut self) {
        // TODO: Stop multiple times
        self.stop();
    }
}
