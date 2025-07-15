use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use rhai::Engine;

#[no_mangle]
pub extern "C" fn eval_expr(expr: *const c_char) -> *mut c_char {
    unsafe {
        assert!(!expr.is_null());
        let c_str = CStr::from_ptr(expr);

        let expr_str = match c_str.to_str() {
            Ok(s) => s,
            Err(_) => return std::ptr::null_mut(),
        };

        let engine = Engine::new();
        let result = match engine.eval::<rhai::Dynamic>(expr_str) {
            Ok(res) => format!("{}", res),
            Err(err) => format!("Error: {}", err),
        };

        CString::new(result).unwrap().into_raw()
    }
}

#[no_mangle]
pub extern "C" fn free_string(ptr: *mut c_char) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        drop(CString::from_raw(ptr));
    }
}
#[no_mangle]
pub extern "C" fn is_expr(s: *const c_char) -> u8 {
    if s.is_null() {
        return 0;
    }
    let c_str = unsafe { CStr::from_ptr(s) };
    let s_str = match c_str.to_str() {
        Ok(v) => v,
        Err(_) => return 0,
    };
    let operators = ['+', '-', '*', '/', '%', '^', '(', ')'];
    for c in s_str.chars() {
        if operators.contains(&c) {
            return 1;
        }
    }
    0
}