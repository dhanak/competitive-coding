use std::error::Error;
use std::fmt::Display;
use std::fs::read_to_string;

#[derive(Debug, PartialEq)]
enum ProgramError {
    BadLen,
    DoesNotTerminate,
    InvalidOpCode(i64),
    BadOutput(Vec<i64>),
}

impl Display for ProgramError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl Error for ProgramError {}

fn run(mem: &[i64], input: &[i64]) -> Result<Vec<i64>, ProgramError> {
    let mut mem = mem.to_vec();
    let mut out = vec![];
    let mut rp = 0; // read pointer
    let mut ip = 0; // instruction pointer
    while ip < mem.len() {
        let opc = mem[ip] % 100;
        let ma = (mem[ip] / 100) % 10;
        let mb = (mem[ip] / 1000) % 10;
        let mc = (mem[ip] / 10000) % 10;
        match (opc, &mem[ip + 1..]) {
            (1, &[a, b, c, ..]) => {
                // add
                assert_eq!(mc, 0);
                mem[c as usize] = par(&mem, ma, a) + par(&mem, mb, b);
                ip += 4;
            }
            (2, &[a, b, c, ..]) => {
                // mul
                assert_eq!(mc, 0);
                mem[c as usize] = par(&mem, ma, a) * par(&mem, mb, b);
                ip += 4;
            }
            (3, &[a, ..]) => {
                // inp
                assert_eq!(ma, 0);
                assert!(rp < input.len());
                mem[a as usize] = input[rp];
                rp += 1;
                ip += 2;
            }
            (4, &[a, ..]) => {
                // out
                out.push(par(&mem, ma, a));
                ip += 2;
            }
            (5 | 6, &[a, b, ..]) => {
                // jump if true | false
                let a = par(&mem, ma, a);
                if (opc == 5 && a != 0) || (opc == 6 && a == 0) {
                    ip = par(&mem, mb, b) as usize;
                } else {
                    ip += 3;
                }
            }
            (7, &[a, b, c, ..]) => {
                // less than
                assert_eq!(mc, 0);
                mem[c as usize] = (par(&mem, ma, a) < par(&mem, mb, b)) as i64;
                ip += 4;
            }
            (8, &[a, b, c, ..]) => {
                // equal
                assert_eq!(mc, 0);
                mem[c as usize] = (par(&mem, ma, a) == par(&mem, mb, b)) as i64;
                ip += 4;
            }
            (1..=8, _) => return Err(ProgramError::BadLen),
            (99, _) => return Ok(out),
            _ => return Err(ProgramError::InvalidOpCode(opc)),
        };
    }
    Err(ProgramError::DoesNotTerminate)
}

fn par(mem: &[i64], mode: i64, value: i64) -> i64 {
    match mode {
        0 => mem[value as usize], // position mode
        1 => value,               // immediate mode
        _ => panic!("unrecognized parameter mode {}", mode),
    }
}

fn q1(mem: &[i64]) -> Result<i64, ProgramError> {
    let out = run(mem, &[1])?;
    match out.split_last() {
        Some((diag, rest)) if rest.iter().all(|&v| v == 0) => Ok(*diag),
        _ => Err(ProgramError::BadOutput(out)),
    }
}

fn q2(mem: &[i64]) -> Result<i64, ProgramError> {
    let out = run(mem, &[5])?;
    match out[..] {
        [diagnostic_code] => Ok(diagnostic_code),
        _ => Err(ProgramError::BadOutput(out)),
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let mem: Vec<i64> = read_to_string("input/day05.in")?
        .trim()
        .split(',')
        .map(|v| v.parse::<i64>())
        .collect::<Result<Vec<i64>, _>>()?;

    println!("{}", q1(&mem)?);
    println!("{}", q2(&mem)?);
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    fn test_run(prog: &[i64], inp: i64, out: i64) {
        assert_eq!(run(prog, &[inp]), Ok(vec![out]));
    }

    #[test]
    fn test_equal() {
        // position mode
        test_run(&[3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8], 8, 1);
        test_run(&[3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8], 5, 0);

        // immediate mode
        test_run(&[3, 3, 1108, -1, 8, 3, 4, 3, 99], 8, 1);
        test_run(&[3, 3, 1108, -1, 8, 3, 4, 3, 99], 5, 0);
    }

    #[test]
    fn test_less_than() {
        // position mode
        test_run(&[3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8], 5, 1);
        test_run(&[3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8], 10, 0);

        // immediate mode
        test_run(&[3, 3, 1107, -1, 8, 3, 4, 3, 99], 5, 1);
        test_run(&[3, 3, 1107, -1, 8, 3, 4, 3, 99], 10, 0);
    }

    #[test]
    fn test_jump() {
        // position mode
        let prog = [3, 12, 6, 12, 15, 1, 13, 14, 13, 4, 13, 99, -1, 0, 1, 9];
        test_run(&prog, 0, 0);
        test_run(&prog, 5, 1);

        // immediate mode
        let prog = [3, 3, 1105, -1, 9, 1101, 0, 0, 12, 4, 12, 99, 1];
        test_run(&prog, 0, 0);
        test_run(&prog, 5, 1);
    }
}
