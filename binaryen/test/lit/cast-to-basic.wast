;; NOTE: Assertions have been generated by update_lit_checks.py and should not be edited.

;; Test that casts to basic types round trip properly.

;; RUN: wasm-opt %s -all --roundtrip -S -o - | filecheck %s

(module
  ;; CHECK:      (func $test (type $1) (param $x structref) (result i32)
  ;; CHECK-NEXT:  (ref.test (ref struct)
  ;; CHECK-NEXT:   (local.get $x)
  ;; CHECK-NEXT:  )
  ;; CHECK-NEXT: )
  (func $test (param $x (ref null struct)) (result i32)
    (ref.test (ref struct)
      (local.get $x)
    )
  )

  ;; CHECK:      (func $cast (type $2) (param $x structref)
  ;; CHECK-NEXT:  (drop
  ;; CHECK-NEXT:   (ref.cast structref
  ;; CHECK-NEXT:    (local.get $x)
  ;; CHECK-NEXT:   )
  ;; CHECK-NEXT:  )
  ;; CHECK-NEXT: )
  (func $cast (param $x (ref null struct))
    (drop
      (ref.cast structref
        (local.get $x)
      )
    )
  )

  ;; CHECK:      (func $br (type $0) (param $anyref anyref)
  ;; CHECK-NEXT:  (drop
  ;; CHECK-NEXT:   (block $block (result structref)
  ;; CHECK-NEXT:    (drop
  ;; CHECK-NEXT:     (br_on_cast $block anyref (ref struct)
  ;; CHECK-NEXT:      (local.get $anyref)
  ;; CHECK-NEXT:     )
  ;; CHECK-NEXT:    )
  ;; CHECK-NEXT:    (ref.null none)
  ;; CHECK-NEXT:   )
  ;; CHECK-NEXT:  )
  ;; CHECK-NEXT: )
  (func $br (param $anyref anyref)
    (drop
      (block $l (result structref)
        (drop
          (br_on_cast $l anyref (ref struct)
            (local.get $anyref)
          )
        )
        (ref.null none)
      )
    )
  )

  ;; CHECK:      (func $br-null (type $0) (param $anyref anyref)
  ;; CHECK-NEXT:  (drop
  ;; CHECK-NEXT:   (block $block (result structref)
  ;; CHECK-NEXT:    (drop
  ;; CHECK-NEXT:     (br_on_cast $block anyref structref
  ;; CHECK-NEXT:      (local.get $anyref)
  ;; CHECK-NEXT:     )
  ;; CHECK-NEXT:    )
  ;; CHECK-NEXT:    (ref.null none)
  ;; CHECK-NEXT:   )
  ;; CHECK-NEXT:  )
  ;; CHECK-NEXT: )
  (func $br-null (param $anyref anyref)
    (drop
      (block $l (result structref)
        (drop
          (br_on_cast $l anyref structref
            (local.get $anyref)
          )
        )
        (ref.null none)
      )
    )
  )

  ;; CHECK:      (func $br-fail-null (type $0) (param $anyref anyref)
  ;; CHECK-NEXT:  (drop
  ;; CHECK-NEXT:   (block $block (result anyref)
  ;; CHECK-NEXT:    (drop
  ;; CHECK-NEXT:     (br_on_cast_fail $block anyref structref
  ;; CHECK-NEXT:      (local.get $anyref)
  ;; CHECK-NEXT:     )
  ;; CHECK-NEXT:    )
  ;; CHECK-NEXT:    (ref.null none)
  ;; CHECK-NEXT:   )
  ;; CHECK-NEXT:  )
  ;; CHECK-NEXT: )
  (func $br-fail-null (param $anyref anyref)
    (drop
      (block $l (result anyref)
        (drop
          (br_on_cast_fail $l anyref structref
            (local.get $anyref)
          )
        )
        (ref.null none)
      )
    )
  )
)
