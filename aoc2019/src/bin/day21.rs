use aoc2019::intcode::Intcode;
use itertools::Itertools;

fn solve(code: &Intcode, instr: &str) -> i64 {
    let instr = instr.chars().map(|c| c as i64).collect::<Vec<_>>();
    let out = code.clone().run_with(&instr).unwrap();
    if let Some(&c) = out.last() {
        if c > 255 {
            return c;
        }
    }
    println!(
        "{}",
        out.iter()
            .map(|&c| char::from_u32(c as u32).unwrap())
            .join("")
    );
    0
}

fn q1(code: &Intcode) -> i64 {
    // (!A || !B || !C) && D
    solve(
        code,
        "NOT A J\n\
         NOT B T\n\
         OR T J\n\
         NOT C T\n\
         OR T J\n\
         AND D J\n\
         WALK\n",
    )
}

fn q2(code: &Intcode) -> i64 {
    // (!A || !B || !C) && D && (E || H)
    solve(
        code,
        "NOT A J\n\
         NOT B T\n\
         OR T J\n\
         NOT C T\n\
         OR T J\n\
         AND D J\n\
         NOT E T\n\
         NOT T T\n\
         OR H T\n\
         AND T J\n\
         RUN\n",
    )
}

fn main() {
    let code = Intcode::load("input/day21.in").unwrap();
    println!("Q1: {}", q1(&code));
    println!("Q2: {}", q2(&code));
}
