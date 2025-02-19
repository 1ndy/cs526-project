;; NOTE: Assertions have been generated by update_lit_checks.py and should not be edited.

;; Test that we roundtrip br_if with a reference value properly if GC is not
;; enabled. We emit a ref.cast in such cases when GC is used, but if only
;; reference types are enabled (and not GC) then we do not need to emit a cast
;; at all (without GC, there are no situations that require a cast anyhow). If
;; we did emit a cast we would error here on GC not being enabled.

;; RUN: wasm-opt %s --enable-reference-types --roundtrip -S -o - | filecheck %s

(module
  ;; CHECK:      (func $test (param $x i32) (result funcref)
  ;; CHECK-NEXT:  (block $block (result funcref)
  ;; CHECK-NEXT:   (br_if $block
  ;; CHECK-NEXT:    (ref.func $test)
  ;; CHECK-NEXT:    (local.get $x)
  ;; CHECK-NEXT:   )
  ;; CHECK-NEXT:  )
  ;; CHECK-NEXT: )
  (func $test (param $x i32) (result funcref)
    (block $out (result funcref)
      (br_if $out
        ;; This has non-nullable type, which is more refined than the block, so
        ;; it looks like we need to emit a cast after the br_if.
        (ref.func $test)
        (local.get $x)
      )
    )
  )
)
