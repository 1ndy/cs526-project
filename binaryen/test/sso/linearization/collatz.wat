(module
    (func $test (export "test") (param i32) (result i32) (local i32)
        (i32.const 1)
        (local.set 1)
        (local.get 0)
        (local.set 1)
        (block $alpha
            (loop $beta
                (local.get 1)
                (local.set 1)
                (local.get 0)
                (local.tee 0)
                (i32.const 1)
                (i32.eq)
                (br_if 1)
                (local.get 1)
                (i32.const 1)
                (i32.add)
                (local.set 1)
                (local.get 0)
                (i32.const 3)
                (i32.mul)
                (i32.const 1)
                (i32.add)
                (local.get 0)
                (i32.const 1)
                (i32.shr_s)
                (local.get 0)
                (i32.const 1)
                (i32.and)
                (select)
                (local.set 0)
                (br 0)
            )
        )
        (local.get 1)
    )
)
