#[cxx::bridge]
mod ffi {
    extern "Rust" {
        // type MultiBuf;

        // fn next_chunk(buf: &mut MultiBuf) -> &[u8];
    }

    unsafe extern "C++" {
        include!("Session.h");

        // type BlobstoreClient;

        // fn new_blobstore_client() -> UniquePtr<BlobstoreClient>;
        // fn put(&self, parts: &mut MultiBuf) -> u64;
    }
}
