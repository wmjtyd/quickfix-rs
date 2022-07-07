#[cxx::bridge]
mod ffi {
    extern "Rust" {
        // type MultiBuf;

        // fn next_chunk(buf: &mut MultiBuf) -> &[u8];
    }

    unsafe extern "C++" {
        include!("Entrypoint.h");

        fn create_client(test: fn() -> i32);
    }
}

pub fn create_client(test: fn() -> i32) {
    unsafe { ffi::create_client(test); }
}
