#![allow(dead_code)]

struct Point<'a> {
    x: i32,
    y: i32,
    z: i32,
    name: &'a str,
}

impl Point<'_> {
    #[inline(never)]
    #[no_mangle]
    fn distance(&self) -> i32 {
        self.x + self.y + self.z
    }
}

static pt: Point = Point {
    x: 1,
    y: 2,
    z: 3,
    name: "hello",
};

static pt2: Point = Point {
    x: 1,
    y: 2,
    z: 3,
    name: "goodbye",
};
