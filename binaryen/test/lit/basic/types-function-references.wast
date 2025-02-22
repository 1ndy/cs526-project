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
  ;; CHECK-TEXT:      (type $mixed_results (func (result anyref f32 anyref f32)))

  ;; CHECK-TEXT:      (type $i32-i32 (func (param i32) (result i32)))

  ;; CHECK-TEXT:      (type $void (func))
  ;; CHECK-BIN:      (type $mixed_results (func (result anyref f32 anyref f32)))

  ;; CHECK-BIN:      (type $i32-i32 (func (param i32) (result i32)))

  ;; CHECK-BIN:      (type $void (func))
  (type $void (func))
  ;; inline ref type in result
  ;; CHECK-TEXT:      (type $3 (func (result i32 (ref null $mixed_results) f64)))

  ;; CHECK-TEXT:      (type $_=>_eqref (func (result eqref)))
  ;; CHECK-BIN:      (type $3 (func (result i32 (ref null $mixed_results) f64)))

  ;; CHECK-BIN:      (type $_=>_eqref (func (result eqref)))
  (type $_=>_eqref (func (result eqref)))

  ;; CHECK-TEXT:      (type $5 (func (param (ref $i32-i32)) (result i32)))

  ;; CHECK-TEXT:      (type $6 (func (param (ref null $i32-i32)) (result i32)))

  ;; CHECK-TEXT:      (type $7 (func (result i32)))

  ;; CHECK-TEXT:      (type $f64_=>_ref_null<_->_eqref> (func (param f64) (result (ref null $_=>_eqref))))
  ;; CHECK-BIN:      (type $5 (func (param (ref $i32-i32)) (result i32)))

  ;; CHECK-BIN:      (type $6 (func (param (ref null $i32-i32)) (result i32)))

  ;; CHECK-BIN:      (type $7 (func (result i32)))

  ;; CHECK-BIN:      (type $f64_=>_ref_null<_->_eqref> (func (param f64) (result (ref null $_=>_eqref))))
  (type $f64_=>_ref_null<_->_eqref> (func (param f64) (result (ref null $_=>_eqref))))
  (type $=>eqref (func (result eqref)))
  ;; CHECK-TEXT:      (type $=>anyref (func (result anyref)))
  ;; CHECK-BIN:      (type $=>anyref (func (result anyref)))
  (type $=>anyref (func (result anyref)))
  (type $mixed_results (func (result anyref f32 anyref f32)))
  (type $i32-i32 (func (param i32) (result i32)))

  ;; CHECK-TEXT:      (type $10 (func (param (ref null $mixed_results))))

  ;; CHECK-TEXT:      (elem declare func $call-ref $call-ref-more)

  ;; CHECK-TEXT:      (func $call-ref (type $void)
  ;; CHECK-TEXT-NEXT:  (call_ref $void
  ;; CHECK-TEXT-NEXT:   (ref.func $call-ref)
  ;; CHECK-TEXT-NEXT:  )
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (type $10 (func (param (ref null $mixed_results))))

  ;; CHECK-BIN:      (elem declare func $call-ref $call-ref-more)

  ;; CHECK-BIN:      (func $call-ref (type $void)
  ;; CHECK-BIN-NEXT:  (call_ref $void
  ;; CHECK-BIN-NEXT:   (ref.func $call-ref)
  ;; CHECK-BIN-NEXT:  )
  ;; CHECK-BIN-NEXT: )
  (func $call-ref
    (call_ref $void (ref.func $call-ref))
  )

  ;; CHECK-TEXT:      (func $return-call-ref (type $void)
  ;; CHECK-TEXT-NEXT:  (return_call_ref $void
  ;; CHECK-TEXT-NEXT:   (ref.func $call-ref)
  ;; CHECK-TEXT-NEXT:  )
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $return-call-ref (type $void)
  ;; CHECK-BIN-NEXT:  (return_call_ref $void
  ;; CHECK-BIN-NEXT:   (ref.func $call-ref)
  ;; CHECK-BIN-NEXT:  )
  ;; CHECK-BIN-NEXT: )
  (func $return-call-ref
    (return_call_ref $void (ref.func $call-ref))
  )

  ;; CHECK-TEXT:      (func $call-ref-more (type $i32-i32) (param $0 i32) (result i32)
  ;; CHECK-TEXT-NEXT:  (call_ref $i32-i32
  ;; CHECK-TEXT-NEXT:   (i32.const 42)
  ;; CHECK-TEXT-NEXT:   (ref.func $call-ref-more)
  ;; CHECK-TEXT-NEXT:  )
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $call-ref-more (type $i32-i32) (param $0 i32) (result i32)
  ;; CHECK-BIN-NEXT:  (call_ref $i32-i32
  ;; CHECK-BIN-NEXT:   (i32.const 42)
  ;; CHECK-BIN-NEXT:   (ref.func $call-ref-more)
  ;; CHECK-BIN-NEXT:  )
  ;; CHECK-BIN-NEXT: )
  (func $call-ref-more (param i32) (result i32)
    (call_ref $i32-i32 (i32.const 42) (ref.func $call-ref-more))
  )

  ;; CHECK-TEXT:      (func $call_from-param (type $5) (param $f (ref $i32-i32)) (result i32)
  ;; CHECK-TEXT-NEXT:  (call_ref $i32-i32
  ;; CHECK-TEXT-NEXT:   (i32.const 42)
  ;; CHECK-TEXT-NEXT:   (local.get $f)
  ;; CHECK-TEXT-NEXT:  )
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $call_from-param (type $5) (param $f (ref $i32-i32)) (result i32)
  ;; CHECK-BIN-NEXT:  (call_ref $i32-i32
  ;; CHECK-BIN-NEXT:   (i32.const 42)
  ;; CHECK-BIN-NEXT:   (local.get $f)
  ;; CHECK-BIN-NEXT:  )
  ;; CHECK-BIN-NEXT: )
  (func $call_from-param (param $f (ref $i32-i32)) (result i32)
    (call_ref $i32-i32 (i32.const 42) (local.get $f))
  )

  ;; CHECK-TEXT:      (func $call_from-param-null (type $6) (param $f (ref null $i32-i32)) (result i32)
  ;; CHECK-TEXT-NEXT:  (call_ref $i32-i32
  ;; CHECK-TEXT-NEXT:   (i32.const 42)
  ;; CHECK-TEXT-NEXT:   (local.get $f)
  ;; CHECK-TEXT-NEXT:  )
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $call_from-param-null (type $6) (param $f (ref null $i32-i32)) (result i32)
  ;; CHECK-BIN-NEXT:  (call_ref $i32-i32
  ;; CHECK-BIN-NEXT:   (i32.const 42)
  ;; CHECK-BIN-NEXT:   (local.get $f)
  ;; CHECK-BIN-NEXT:  )
  ;; CHECK-BIN-NEXT: )
  (func $call_from-param-null (param $f (ref null $i32-i32)) (result i32)
    (call_ref $i32-i32 (i32.const 42) (local.get $f))
  )

  ;; CHECK-TEXT:      (func $call_from-local-null (type $7) (result i32)
  ;; CHECK-TEXT-NEXT:  (local $f (ref null $i32-i32))
  ;; CHECK-TEXT-NEXT:  (local.set $f
  ;; CHECK-TEXT-NEXT:   (ref.func $call-ref-more)
  ;; CHECK-TEXT-NEXT:  )
  ;; CHECK-TEXT-NEXT:  (call_ref $i32-i32
  ;; CHECK-TEXT-NEXT:   (i32.const 42)
  ;; CHECK-TEXT-NEXT:   (local.get $f)
  ;; CHECK-TEXT-NEXT:  )
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $call_from-local-null (type $7) (result i32)
  ;; CHECK-BIN-NEXT:  (local $f (ref null $i32-i32))
  ;; CHECK-BIN-NEXT:  (local.set $f
  ;; CHECK-BIN-NEXT:   (ref.func $call-ref-more)
  ;; CHECK-BIN-NEXT:  )
  ;; CHECK-BIN-NEXT:  (call_ref $i32-i32
  ;; CHECK-BIN-NEXT:   (i32.const 42)
  ;; CHECK-BIN-NEXT:   (local.get $f)
  ;; CHECK-BIN-NEXT:  )
  ;; CHECK-BIN-NEXT: )
  (func $call_from-local-null (result i32)
    (local $f (ref null $i32-i32))
    (local.set $f (ref.func $call-ref-more))
    (call_ref $i32-i32 (i32.const 42) (local.get $f))
  )

  ;; CHECK-TEXT:      (func $ref-in-sig (type $f64_=>_ref_null<_->_eqref>) (param $0 f64) (result (ref null $_=>_eqref))
  ;; CHECK-TEXT-NEXT:  (ref.null nofunc)
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $ref-in-sig (type $f64_=>_ref_null<_->_eqref>) (param $0 f64) (result (ref null $_=>_eqref))
  ;; CHECK-BIN-NEXT:  (ref.null nofunc)
  ;; CHECK-BIN-NEXT: )
  (func $ref-in-sig (param $0 f64) (result (ref null $=>eqref))
    (ref.null $=>eqref)
  )

  ;; CHECK-TEXT:      (func $type-only-in-tuple-local (type $void)
  ;; CHECK-TEXT-NEXT:  (local $x (tuple i32 (ref null $=>anyref) f64))
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $type-only-in-tuple-local (type $void)
  ;; CHECK-BIN-NEXT:  (local $x i32)
  ;; CHECK-BIN-NEXT:  (local $1 f64)
  ;; CHECK-BIN-NEXT:  (local $2 (ref null $=>anyref))
  ;; CHECK-BIN-NEXT: )
  (func $type-only-in-tuple-local
    (local $x (tuple i32 (ref null $=>anyref) f64))
  )

  ;; CHECK-TEXT:      (func $type-only-in-tuple-block (type $void)
  ;; CHECK-TEXT-NEXT:  (tuple.drop 3
  ;; CHECK-TEXT-NEXT:   (block $block (type $3) (result i32 (ref null $mixed_results) f64)
  ;; CHECK-TEXT-NEXT:    (unreachable)
  ;; CHECK-TEXT-NEXT:   )
  ;; CHECK-TEXT-NEXT:  )
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $type-only-in-tuple-block (type $void)
  ;; CHECK-BIN-NEXT:  (local $scratch (tuple i32 (ref null $mixed_results) f64))
  ;; CHECK-BIN-NEXT:  (local $scratch_1 (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:  (local $scratch_2 i32)
  ;; CHECK-BIN-NEXT:  (drop
  ;; CHECK-BIN-NEXT:   (block (result i32)
  ;; CHECK-BIN-NEXT:    (local.set $scratch_2
  ;; CHECK-BIN-NEXT:     (tuple.extract 3 0
  ;; CHECK-BIN-NEXT:      (local.tee $scratch
  ;; CHECK-BIN-NEXT:       (block (type $3) (result i32 (ref null $mixed_results) f64)
  ;; CHECK-BIN-NEXT:        (unreachable)
  ;; CHECK-BIN-NEXT:       )
  ;; CHECK-BIN-NEXT:      )
  ;; CHECK-BIN-NEXT:     )
  ;; CHECK-BIN-NEXT:    )
  ;; CHECK-BIN-NEXT:    (drop
  ;; CHECK-BIN-NEXT:     (block (result (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:      (local.set $scratch_1
  ;; CHECK-BIN-NEXT:       (tuple.extract 3 1
  ;; CHECK-BIN-NEXT:        (local.get $scratch)
  ;; CHECK-BIN-NEXT:       )
  ;; CHECK-BIN-NEXT:      )
  ;; CHECK-BIN-NEXT:      (drop
  ;; CHECK-BIN-NEXT:       (tuple.extract 3 2
  ;; CHECK-BIN-NEXT:        (local.get $scratch)
  ;; CHECK-BIN-NEXT:       )
  ;; CHECK-BIN-NEXT:      )
  ;; CHECK-BIN-NEXT:      (local.get $scratch_1)
  ;; CHECK-BIN-NEXT:     )
  ;; CHECK-BIN-NEXT:    )
  ;; CHECK-BIN-NEXT:    (local.get $scratch_2)
  ;; CHECK-BIN-NEXT:   )
  ;; CHECK-BIN-NEXT:  )
  ;; CHECK-BIN-NEXT: )
  (func $type-only-in-tuple-block
    (tuple.drop 3
      (block $block (result i32 (ref null $mixed_results) f64)
        (unreachable)
      )
    )
  )

  ;; CHECK-TEXT:      (func $ref-types-first (type $void)
  ;; CHECK-TEXT-NEXT:  (local $r1 (ref null $mixed_results))
  ;; CHECK-TEXT-NEXT:  (local $r2 (ref null $mixed_results))
  ;; CHECK-TEXT-NEXT:  (local $i1 i32)
  ;; CHECK-TEXT-NEXT:  (local $r3 anyref)
  ;; CHECK-TEXT-NEXT:  (local $i2 i64)
  ;; CHECK-TEXT-NEXT:  (local $r4 anyref)
  ;; CHECK-TEXT-NEXT:  (local $i3 i64)
  ;; CHECK-TEXT-NEXT:  (local $r5 anyref)
  ;; CHECK-TEXT-NEXT:  (local $r6 funcref)
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $ref-types-first (type $void)
  ;; CHECK-BIN-NEXT:  (local $r1 (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:  (local $r2 (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:  (local $r3 anyref)
  ;; CHECK-BIN-NEXT:  (local $r4 anyref)
  ;; CHECK-BIN-NEXT:  (local $r5 anyref)
  ;; CHECK-BIN-NEXT:  (local $r6 funcref)
  ;; CHECK-BIN-NEXT:  (local $i1 i32)
  ;; CHECK-BIN-NEXT:  (local $i2 i64)
  ;; CHECK-BIN-NEXT:  (local $i3 i64)
  ;; CHECK-BIN-NEXT: )
  (func $ref-types-first
    ;; 6 reference types and 3 MVP types. The binary format should emit all the
    ;; reference types first since a reference type appears first. In addition,
    ;; types should be emitted in blocks there, that is, locals of identical
    ;; types should be adjacent.
    (local $r1 (ref null $mixed_results))
    (local $r2 (ref null $mixed_results))
    (local $i1 i32)
    (local $r3 anyref)
    (local $i2 i64)
    (local $r4 anyref)
    (local $i3 i64)
    (local $r5 anyref)
    (local $r6 funcref)
  )

  ;; CHECK-TEXT:      (func $mvp-types-first (type $void)
  ;; CHECK-TEXT-NEXT:  (local $i1 i32)
  ;; CHECK-TEXT-NEXT:  (local $r1 (ref null $mixed_results))
  ;; CHECK-TEXT-NEXT:  (local $r2 (ref null $mixed_results))
  ;; CHECK-TEXT-NEXT:  (local $r3 anyref)
  ;; CHECK-TEXT-NEXT:  (local $i2 i64)
  ;; CHECK-TEXT-NEXT:  (local $r4 anyref)
  ;; CHECK-TEXT-NEXT:  (local $i3 i64)
  ;; CHECK-TEXT-NEXT:  (local $r5 anyref)
  ;; CHECK-TEXT-NEXT:  (local $r6 funcref)
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $mvp-types-first (type $void)
  ;; CHECK-BIN-NEXT:  (local $i1 i32)
  ;; CHECK-BIN-NEXT:  (local $i2 i64)
  ;; CHECK-BIN-NEXT:  (local $i3 i64)
  ;; CHECK-BIN-NEXT:  (local $r1 (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:  (local $r2 (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:  (local $r3 anyref)
  ;; CHECK-BIN-NEXT:  (local $r4 anyref)
  ;; CHECK-BIN-NEXT:  (local $r5 anyref)
  ;; CHECK-BIN-NEXT:  (local $r6 funcref)
  ;; CHECK-BIN-NEXT: )
  (func $mvp-types-first
    ;; Reversed from before, now an MVP type appears first, so they should all
    ;; be before reference types in the binary format.
    (local $i1 i32) ;; only this local was moved up.
    (local $r1 (ref null $mixed_results))
    (local $r2 (ref null $mixed_results))
    (local $r3 anyref)
    (local $i2 i64)
    (local $r4 anyref)
    (local $i3 i64)
    (local $r5 anyref)
    (local $r6 funcref)
  )

  ;; CHECK-TEXT:      (func $mvp-types-first-param (type $10) (param $r0 (ref null $mixed_results))
  ;; CHECK-TEXT-NEXT:  (local $i1 i32)
  ;; CHECK-TEXT-NEXT:  (local $r1 (ref null $mixed_results))
  ;; CHECK-TEXT-NEXT:  (local $r2 (ref null $mixed_results))
  ;; CHECK-TEXT-NEXT:  (local $r3 anyref)
  ;; CHECK-TEXT-NEXT:  (local $i2 i64)
  ;; CHECK-TEXT-NEXT:  (local $r4 anyref)
  ;; CHECK-TEXT-NEXT:  (local $i3 i64)
  ;; CHECK-TEXT-NEXT:  (local $r5 anyref)
  ;; CHECK-TEXT-NEXT:  (local $r6 funcref)
  ;; CHECK-TEXT-NEXT: )
  ;; CHECK-BIN:      (func $mvp-types-first-param (type $10) (param $r0 (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:  (local $i1 i32)
  ;; CHECK-BIN-NEXT:  (local $i2 i64)
  ;; CHECK-BIN-NEXT:  (local $i3 i64)
  ;; CHECK-BIN-NEXT:  (local $r1 (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:  (local $r2 (ref null $mixed_results))
  ;; CHECK-BIN-NEXT:  (local $r3 anyref)
  ;; CHECK-BIN-NEXT:  (local $r4 anyref)
  ;; CHECK-BIN-NEXT:  (local $r5 anyref)
  ;; CHECK-BIN-NEXT:  (local $r6 funcref)
  ;; CHECK-BIN-NEXT: )
  (func $mvp-types-first-param (param $r0 (ref null $mixed_results))
    ;; As before, but now there is a reference type *parameter*. We should
    ;; ignore that and sort as in the last function.
    (local $i1 i32) ;; only this local was moved up.
    (local $r1 (ref null $mixed_results))
    (local $r2 (ref null $mixed_results))
    (local $r3 anyref)
    (local $i2 i64)
    (local $r4 anyref)
    (local $i3 i64)
    (local $r5 anyref)
    (local $r6 funcref)
  )
)
;; CHECK-BIN-NODEBUG:      (type $0 (func (result anyref f32 anyref f32)))

;; CHECK-BIN-NODEBUG:      (type $1 (func (param i32) (result i32)))

;; CHECK-BIN-NODEBUG:      (type $2 (func))

;; CHECK-BIN-NODEBUG:      (type $3 (func (result i32 (ref null $0) f64)))

;; CHECK-BIN-NODEBUG:      (type $4 (func (result eqref)))

;; CHECK-BIN-NODEBUG:      (type $5 (func (param (ref $1)) (result i32)))

;; CHECK-BIN-NODEBUG:      (type $6 (func (param (ref null $1)) (result i32)))

;; CHECK-BIN-NODEBUG:      (type $7 (func (result i32)))

;; CHECK-BIN-NODEBUG:      (type $8 (func (param f64) (result (ref null $4))))

;; CHECK-BIN-NODEBUG:      (type $9 (func (result anyref)))

;; CHECK-BIN-NODEBUG:      (type $10 (func (param (ref null $0))))

;; CHECK-BIN-NODEBUG:      (elem declare func $0 $2)

;; CHECK-BIN-NODEBUG:      (func $0 (type $2)
;; CHECK-BIN-NODEBUG-NEXT:  (call_ref $2
;; CHECK-BIN-NODEBUG-NEXT:   (ref.func $0)
;; CHECK-BIN-NODEBUG-NEXT:  )
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $1 (type $2)
;; CHECK-BIN-NODEBUG-NEXT:  (return_call_ref $2
;; CHECK-BIN-NODEBUG-NEXT:   (ref.func $0)
;; CHECK-BIN-NODEBUG-NEXT:  )
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $2 (type $1) (param $0 i32) (result i32)
;; CHECK-BIN-NODEBUG-NEXT:  (call_ref $1
;; CHECK-BIN-NODEBUG-NEXT:   (i32.const 42)
;; CHECK-BIN-NODEBUG-NEXT:   (ref.func $2)
;; CHECK-BIN-NODEBUG-NEXT:  )
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $3 (type $5) (param $0 (ref $1)) (result i32)
;; CHECK-BIN-NODEBUG-NEXT:  (call_ref $1
;; CHECK-BIN-NODEBUG-NEXT:   (i32.const 42)
;; CHECK-BIN-NODEBUG-NEXT:   (local.get $0)
;; CHECK-BIN-NODEBUG-NEXT:  )
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $4 (type $6) (param $0 (ref null $1)) (result i32)
;; CHECK-BIN-NODEBUG-NEXT:  (call_ref $1
;; CHECK-BIN-NODEBUG-NEXT:   (i32.const 42)
;; CHECK-BIN-NODEBUG-NEXT:   (local.get $0)
;; CHECK-BIN-NODEBUG-NEXT:  )
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $5 (type $7) (result i32)
;; CHECK-BIN-NODEBUG-NEXT:  (local $0 (ref null $1))
;; CHECK-BIN-NODEBUG-NEXT:  (local.set $0
;; CHECK-BIN-NODEBUG-NEXT:   (ref.func $2)
;; CHECK-BIN-NODEBUG-NEXT:  )
;; CHECK-BIN-NODEBUG-NEXT:  (call_ref $1
;; CHECK-BIN-NODEBUG-NEXT:   (i32.const 42)
;; CHECK-BIN-NODEBUG-NEXT:   (local.get $0)
;; CHECK-BIN-NODEBUG-NEXT:  )
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $6 (type $8) (param $0 f64) (result (ref null $4))
;; CHECK-BIN-NODEBUG-NEXT:  (ref.null nofunc)
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $7 (type $2)
;; CHECK-BIN-NODEBUG-NEXT:  (local $0 i32)
;; CHECK-BIN-NODEBUG-NEXT:  (local $1 f64)
;; CHECK-BIN-NODEBUG-NEXT:  (local $2 (ref null $9))
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $8 (type $2)
;; CHECK-BIN-NODEBUG-NEXT:  (local $scratch (tuple i32 (ref null $0) f64))
;; CHECK-BIN-NODEBUG-NEXT:  (local $scratch_1 (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:  (local $scratch_2 i32)
;; CHECK-BIN-NODEBUG-NEXT:  (drop
;; CHECK-BIN-NODEBUG-NEXT:   (block (result i32)
;; CHECK-BIN-NODEBUG-NEXT:    (local.set $scratch_2
;; CHECK-BIN-NODEBUG-NEXT:     (tuple.extract 3 0
;; CHECK-BIN-NODEBUG-NEXT:      (local.tee $scratch
;; CHECK-BIN-NODEBUG-NEXT:       (block (type $3) (result i32 (ref null $0) f64)
;; CHECK-BIN-NODEBUG-NEXT:        (unreachable)
;; CHECK-BIN-NODEBUG-NEXT:       )
;; CHECK-BIN-NODEBUG-NEXT:      )
;; CHECK-BIN-NODEBUG-NEXT:     )
;; CHECK-BIN-NODEBUG-NEXT:    )
;; CHECK-BIN-NODEBUG-NEXT:    (drop
;; CHECK-BIN-NODEBUG-NEXT:     (block (result (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:      (local.set $scratch_1
;; CHECK-BIN-NODEBUG-NEXT:       (tuple.extract 3 1
;; CHECK-BIN-NODEBUG-NEXT:        (local.get $scratch)
;; CHECK-BIN-NODEBUG-NEXT:       )
;; CHECK-BIN-NODEBUG-NEXT:      )
;; CHECK-BIN-NODEBUG-NEXT:      (drop
;; CHECK-BIN-NODEBUG-NEXT:       (tuple.extract 3 2
;; CHECK-BIN-NODEBUG-NEXT:        (local.get $scratch)
;; CHECK-BIN-NODEBUG-NEXT:       )
;; CHECK-BIN-NODEBUG-NEXT:      )
;; CHECK-BIN-NODEBUG-NEXT:      (local.get $scratch_1)
;; CHECK-BIN-NODEBUG-NEXT:     )
;; CHECK-BIN-NODEBUG-NEXT:    )
;; CHECK-BIN-NODEBUG-NEXT:    (local.get $scratch_2)
;; CHECK-BIN-NODEBUG-NEXT:   )
;; CHECK-BIN-NODEBUG-NEXT:  )
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $9 (type $2)
;; CHECK-BIN-NODEBUG-NEXT:  (local $0 (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:  (local $1 (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:  (local $2 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $3 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $4 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $5 funcref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $6 i32)
;; CHECK-BIN-NODEBUG-NEXT:  (local $7 i64)
;; CHECK-BIN-NODEBUG-NEXT:  (local $8 i64)
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $10 (type $2)
;; CHECK-BIN-NODEBUG-NEXT:  (local $0 i32)
;; CHECK-BIN-NODEBUG-NEXT:  (local $1 i64)
;; CHECK-BIN-NODEBUG-NEXT:  (local $2 i64)
;; CHECK-BIN-NODEBUG-NEXT:  (local $3 (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:  (local $4 (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:  (local $5 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $6 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $7 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $8 funcref)
;; CHECK-BIN-NODEBUG-NEXT: )

;; CHECK-BIN-NODEBUG:      (func $11 (type $10) (param $0 (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:  (local $1 i32)
;; CHECK-BIN-NODEBUG-NEXT:  (local $2 i64)
;; CHECK-BIN-NODEBUG-NEXT:  (local $3 i64)
;; CHECK-BIN-NODEBUG-NEXT:  (local $4 (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:  (local $5 (ref null $0))
;; CHECK-BIN-NODEBUG-NEXT:  (local $6 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $7 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $8 anyref)
;; CHECK-BIN-NODEBUG-NEXT:  (local $9 funcref)
;; CHECK-BIN-NODEBUG-NEXT: )
