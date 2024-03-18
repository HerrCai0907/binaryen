;; NOTE: Assertions have been generated by update_lit_checks.py and should not be edited.

;; RUN: wasm-opt %s --precompute --fuzz-exec -all -S -o - | filecheck %s

(module
 ;; CHECK:      (func $eq-no (type $0) (result i32)
 ;; CHECK-NEXT:  (i32.const 0)
 ;; CHECK-NEXT: )
 (func $eq-no (export "eq-no") (result i32)
  (string.eq
   (string.const "ab")
   (string.const "cdefg")
  )
 )

 ;; CHECK:      (func $eq-yes (type $0) (result i32)
 ;; CHECK-NEXT:  (i32.const 1)
 ;; CHECK-NEXT: )
 (func $eq-yes (export "eq-yes") (result i32)
  (string.eq
   (string.const "ab")
   (string.const "ab")
  )
 )

 ;; CHECK:      (func $concat (type $0) (result i32)
 ;; CHECK-NEXT:  (i32.const 1)
 ;; CHECK-NEXT: )
 (func $concat (export "concat") (result i32)
  (string.eq
   (string.concat (string.const "a") (string.const "b"))
   (string.const "ab")
  )
 )
)
