use aoc2019::intcode::Intcode;

use std::error::Error;

fn q(mut code: Intcode, inp: i64) -> Result<i64, Box<dyn Error>> {
    let out = code.run_with(&[inp])?;
    match out[..] {
        [out] => Ok(out),
        _ => Err(format!("bad output: {:?}", out).into()),
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let code = Intcode::load("input/day09.in")?;
    println!("Q1: {}", q(code.clone(), 1)?);
    println!("Q2: {}", q(code, 2)?);
    Ok(())
}
