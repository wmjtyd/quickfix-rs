use std::{time::UNIX_EPOCH, sync::Mutex};

use jsonwebtoken::{Header, Algorithm, encode, EncodingKey};
use once_cell::sync::OnceCell;
use rand::prelude::*;
use sha2::{Sha256, Digest};

const ACCOUNT_ID: &str = "";
const SECRET_KEY_ID: &str = "";
const SECRET_KEY: &str = "";
const SYMBOL: &str = "BTCUSDT";
const VENUE: &str = "FIX";  // choose exchange you want to trade
const SERVER: &str = "fix.api.apifiny.com:1443";  // use the right endpoint for each exchange
const SENDER_COMP_ID: &str = ACCOUNT_ID;
const TARGET_COMP_ID: &str = "APIFINY";

#[derive(Debug, Clone)]
struct SignaturePayload {
    pub method: String,
    pub account_id: String,
    pub secret_key_id: String,
    pub params: String,
    pub secret_key: String,
}

#[derive(serde::Serialize, serde::Deserialize)]
struct JwtClaims {
    pub exp: u64,

    #[serde(rename = "accountId")]
    pub account_id: String,
    #[serde(rename = "secretKeyId")]
    pub secret_key_id: String,
    pub method: String,
    pub digest: String,
}

fn get_signature(payload: SignaturePayload) -> anyhow::Result<String> {
    log::debug!("received signature payload: {:#?}", payload);
    let SignaturePayload {
        method,
        account_id,
        secret_key_id,
        params,
        secret_key,
    } = payload;

    let header = Header::new(Algorithm::HS256);
    let digest = sha256(params.as_bytes());
    let exp = std::time::SystemTime::now() + std::time::Duration::from_secs(300);

    let claim = JwtClaims {
        exp: exp.duration_since(UNIX_EPOCH)?.as_secs(),
        account_id,
        secret_key_id,
        method,
        digest,
    };

    Ok(encode(&header, &claim, &EncodingKey::from_base64_secret(&secret_key)?)?)
}

fn sha256(raw: &[u8]) -> String {
    static SHA256_HASHER: OnceCell<Mutex<Sha256>> = OnceCell::new();

    let hasher = SHA256_HASHER.get_or_init(|| {
        Mutex::new(Sha256::new())
    });

    let result = {
        let mut hasher = hasher.lock().unwrap();
        hasher.update(raw);
        hasher.finalize_reset()
    };

    format!("{:x}", result)
}

fn generate_order_id(account_id: &str) -> anyhow::Result<String> {
    let mut split_acc_id = account_id.split('-');

    let current_time = std::time::SystemTime::now().duration_since(UNIX_EPOCH)?.as_secs();
    let account_id = split_acc_id.nth(1).ok_or_else(|| anyhow::anyhow!("no index 1"))?;
    let random_digit = rand::thread_rng().gen_range(100..1000);
    
    log::debug!("current time: {current_time}");
    log::debug!("account id: {account_id}");
    log::debug!("random digit: {random_digit}");

    // orderId if requested, should be composed of:
    //     account number + random number + 13 digit timestamp + 3 digit random number. 
    // Up to 64 digits.
    let order_id = format!("{account_id}{current_time}{random_digit}");
    log::debug!("=> order_id: {random_digit}");

    Ok(order_id)
}

fn main() {

}
