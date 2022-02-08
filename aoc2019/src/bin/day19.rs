use aoc2019::intcode::Intcode;

fn query(code: &Intcode, x: i64, y: i64) -> i64 {
    code.clone().run_with(&[x, y]).unwrap().iter().sum()
}

fn q1(code: &Intcode) -> i64 {
    let mut out = 0;
    for y in 0..50 {
        for x in 0..50 {
            out += query(code, x, y);
        }
    }
    out
}

fn q2(code: &Intcode, s: i64) -> i64 {
    let mut x = 0;
    let mut y = 0;
    let mut moved = true;
    while moved {
        moved = false;
        while query(code, x + s - 1, y) == 0 {
            y += 1;
            moved = true;
        }
        while query(code, x, y + s - 1) == 0 {
            x += 1;
            moved = true;
        }
    }
    assert_eq!(query(code, x, y), 1);
    assert_eq!(query(code, x + s - 1, y), 1);
    assert_eq!(query(code, x + s, y), 0);
    assert_eq!(query(code, x, y + s - 1), 1);
    assert_eq!(query(code, x, y + s), 0);
    x * 10000 + y
}

fn main() {
    let code = Intcode::load("input/day19.in").unwrap();
    println!("Q1: {}", q1(&code));
    println!("Q2: {}", q2(&code, 100));
}
