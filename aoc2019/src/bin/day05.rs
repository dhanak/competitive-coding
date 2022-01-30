use aoc2019::intcode::Intcode;

use std::error::Error;

fn q1(mut code: Intcode) -> Result<i64, Box<dyn Error>> {
    let out = code.run_with(&[1])?;
    match out.split_last() {
        Some((diag, rest)) if rest.iter().all(|&v| v == 0) => Ok(*diag),
        _ => Err(format!("bad output: {:?}", out).into()),
    }
}

fn q2(mut code: Intcode) -> Result<i64, Box<dyn Error>> {
    let out = code.run_with(&[5])?;
    match out[..] {
        [diagnostic_code] => Ok(diagnostic_code),
        _ => Err(format!("bad output: {:?}", out).into()),
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let code = Intcode::load("input/day05.in")?;
    println!("Q1: {}", q1(code.clone())?);
    println!("Q2: {}", q2(code)?);
    Ok(())
}
