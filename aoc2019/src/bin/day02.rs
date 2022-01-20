use std::error::Error;
use std::fmt::Display;
use std::fs::read_to_string;

#[derive(Debug, PartialEq)]
enum ProgramError {
    BadLen,
    DoesNotTerminate,
    InvalidOpCode,
    NoSolution,
}

impl Display for ProgramError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl Error for ProgramError {}

fn run(mem: &[usize]) -> Result<usize, ProgramError> {
    let mut mem = mem.to_vec();
    for i in (0..mem.len()).step_by(4) {
        match mem[i..] {
            [1, a, b, c, ..] => mem[c] = mem[a] + mem[b],
            [2, a, b, c, ..] => mem[c] = mem[a] * mem[b],
            [1 | 2, ..] => return Err(ProgramError::BadLen),
            [99, ..] => return Ok(mem[0]),
            _ => return Err(ProgramError::InvalidOpCode),
        }
    }
    Err(ProgramError::DoesNotTerminate)
}

fn run_with(
    mem: &[usize],
    noun: usize,
    verb: usize,
) -> Result<usize, ProgramError> {
    run(&[&[mem[0], noun, verb], &mem[3..]].concat())
}

fn q1(mem: &[usize]) -> Result<usize, ProgramError> {
    run_with(mem, 12, 2)
}

fn q2(mem: &[usize], goal: usize) -> Result<usize, ProgramError> {
    for noun in 0..=99 {
        for verb in 0..=99 {
            if Ok(goal) == run_with(mem, noun, verb) {
                return Ok(100 * noun + verb);
            }
        }
    }
    Err(ProgramError::NoSolution)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_run() -> Result<(), ProgramError> {
        assert_eq!(run(&[1, 0, 0, 0, 99])?, 2);
        assert_eq!(run(&[1, 1, 1, 4, 99, 5, 6, 0, 99])?, 30);
        Ok(())
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let mem: Vec<usize> = read_to_string("input/day02.in")?
        .trim()
        .split(',')
        .map(|v| v.parse::<usize>())
        .collect::<Result<Vec<usize>, _>>()?;

    println!("Q1: {}", q1(&mem)?);
    println!("Q2: {}", q2(&mem, 19690720)?);
    Ok(())
}
