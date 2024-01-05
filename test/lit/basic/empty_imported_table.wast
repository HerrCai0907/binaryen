;; NOTE: Assertions have been generated by update_lit_checks.py --all-items and should not be edited.

;; RUN: wasm-opt %s -all -o %t.text.wast -g -S
;; RUN: wasm-as %s -all -g -o %t.wasm
;; RUN: wasm-dis %t.wasm -all -o %t.bin.wast
;; RUN: wasm-as %s -all -o %t.nodebug.wasm
;; RUN: wasm-dis %t.nodebug.wasm -all -o %t.bin.nodebug.wast
;; RUN: cat %t.text.wast | filecheck %s --check-prefix=CHECK-TEXT
;; RUN: cat %t.bin.wast | filecheck %s --check-prefix=CHECK-BIN
;; RUN: cat %t.bin.nodebug.wast | filecheck %s --check-prefix=CHECK-BIN-NODEBUG

(module
  (import "env" "table" (table 0 0 funcref))
  ;; CHECK-TEXT:      (import "env" "table" (table $timport$0 0 0 funcref))

  ;; CHECK-TEXT:      (memory $0 0)
  ;; CHECK-BIN:      (import "env" "table" (table $timport$0 0 0 funcref))

  ;; CHECK-BIN:      (memory $0 0)
  ;; CHECK-BIN-NODEBUG:      (import "env" "table" (table $timport$0 0 0 funcref))

  ;; CHECK-BIN-NODEBUG:      (memory $0 0)
  (memory $0 0)
)