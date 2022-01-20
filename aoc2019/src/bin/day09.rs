use std::error::Error;
use std::fmt::Display;
use std::fs::read_to_string;

#[derive(Debug, PartialEq)]
enum ProgramError {
    BadLen,
    DoesNotTerminate,
    InvalidOpCode(i64),
    InvalidMode(i64),
    BadOutput(Vec<i64>),
}

impl Display for ProgramError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl Error for ProgramError {}

#[derive(Debug, PartialEq)]
enum AddressMode {
    PositionMode,
    ImmediateMode,
    RelativeMode(i64),
}

impl AddressMode {
    fn new(
        mode: i64,
        bit: u32,
        base: i64,
    ) -> Result<AddressMode, ProgramError> {
        use AddressMode::*;
        match mode / 10_i64.pow(bit + 2) % 10 {
            0 => Ok(PositionMode),
            1 => Ok(ImmediateMode),
            2 => Ok(RelativeMode(base)),
            _ => Err(ProgramError::InvalidMode(mode)),
        }
    }

    fn read(self, mem: &[i64], addr: i64) -> i64 {
        use AddressMode::*;
        match self {
            PositionMode => *mem.get(addr as usize).unwrap_or(&0),
            ImmediateMode => addr,
            RelativeMode(base) => {
                *mem.get((base + addr) as usize).unwrap_or(&0)
            }
        }
    }

    fn write(
        self,
        mem: &mut Vec<i64>,
        addr: i64,
        value: i64,
    ) -> Result<(), ProgramError> {
        use AddressMode::*;
        let addr = match self {
            PositionMode => addr,
            ImmediateMode => return Err(ProgramError::InvalidMode(1)),
            RelativeMode(base) => base + addr,
        } as usize;
        mem.resize(mem.len().max(addr + 1), 0);
        mem[addr] = value;
        Ok(())
    }
}

fn run(mem: &[i64], input: &[i64]) -> Result<Vec<i64>, ProgramError> {
    let mut mem = mem.to_vec();
    let mut out = vec![];
    let mut rp = 0; // read pointer
    let mut ip = 0; // instruction pointer
    let mut rb = 0; // relative base
    while ip < mem.len() {
        let opc = mem[ip] % 100;
        let ma = AddressMode::new(mem[ip], 0, rb)?;
        let mb = AddressMode::new(mem[ip], 1, rb)?;
        let mc = AddressMode::new(mem[ip], 2, rb)?;
        match (opc, &mem[ip + 1..]) {
            (1, &[a, b, c, ..]) => {
                // add
                let a = ma.read(&mem, a);
                let b = mb.read(&mem, b);
                mc.write(&mut mem, c, a + b)?;
                ip += 4;
            }
            (2, &[a, b, c, ..]) => {
                // mul
                let a = ma.read(&mem, a);
                let b = mb.read(&mem, b);
                mc.write(&mut mem, c, a * b)?;
                ip += 4;
            }
            (3, &[a, ..]) => {
                // inp
                assert!(rp < input.len());
                ma.write(&mut mem, a, input[rp])?;
                rp += 1;
                ip += 2;
            }
            (4, &[a, ..]) => {
                // out
                out.push(ma.read(&mem, a));
                ip += 2;
            }
            (5 | 6, &[a, b, ..]) => {
                // jump if true | false
                let a = ma.read(&mem, a);
                if (opc == 5 && a != 0) || (opc == 6 && a == 0) {
                    ip = mb.read(&mem, b) as usize;
                } else {
                    ip += 3;
                }
            }
            (7, &[a, b, c, ..]) => {
                // less than
                let a = ma.read(&mem, a);
                let b = mb.read(&mem, b);
                mc.write(&mut mem, c, (a < b) as i64)?;
                ip += 4;
            }
            (8, &[a, b, c, ..]) => {
                // equal
                let a = ma.read(&mem, a);
                let b = mb.read(&mem, b);
                mc.write(&mut mem, c, (a == b) as i64)?;
                ip += 4;
            }
            (9, &[a, ..]) => {
                // adjust relative base
                rb += ma.read(&mem, a);
                ip += 2;
            }
            (1..=9, _) => return Err(ProgramError::BadLen),
            (99, _) => return Ok(out),
            _ => return Err(ProgramError::InvalidOpCode(opc)),
        };
    }
    Err(ProgramError::DoesNotTerminate)
}

fn q(mem: &[i64], inp: i64) -> Result<i64, ProgramError> {
    let out = run(mem, &[inp])?;
    match out[..] {
        [out] => Ok(out),
        _ => Err(ProgramError::BadOutput(out)),
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let mem: Vec<i64> = read_to_string("input/day09.in")?
        .trim()
        .split(',')
        .map(|v| v.parse::<i64>())
        .collect::<Result<Vec<i64>, _>>()?;

    println!("Q1: {}", q(&mem, 1)?);
    println!("Q2: {}", q(&mem, 2)?);
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    fn test_run(prog: &[i64], inp: &[i64], out: &[i64]) {
        let r = run(prog, inp);
        assert!(r.is_ok());
        assert_eq!(r.unwrap(), out);
    }

    #[test]
    fn test_equal() {
        // position mode
        test_run(&[3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8], &[8], &[1]);
        test_run(&[3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8], &[5], &[0]);

        // immediate mode
        test_run(&[3, 3, 1108, -1, 8, 3, 4, 3, 99], &[8], &[1]);
        test_run(&[3, 3, 1108, -1, 8, 3, 4, 3, 99], &[5], &[0]);
    }

    #[test]
    fn test_less_than() {
        // position mode
        test_run(&[3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8], &[5], &[1]);
        test_run(&[3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8], &[10], &[0]);

        // immediate mode
        test_run(&[3, 3, 1107, -1, 8, 3, 4, 3, 99], &[5], &[1]);
        test_run(&[3, 3, 1107, -1, 8, 3, 4, 3, 99], &[10], &[0]);
    }

    #[test]
    fn test_jump() {
        // position mode
        let prog = [3, 12, 6, 12, 15, 1, 13, 14, 13, 4, 13, 99, -1, 0, 1, 9];
        test_run(&prog, &[0], &[0]);
        test_run(&prog, &[5], &[1]);

        // immediate mode
        let prog = [3, 3, 1105, -1, 9, 1101, 0, 0, 12, 4, 12, 99, 1];
        test_run(&prog, &[0], &[0]);
        test_run(&prog, &[5], &[1]);
    }

    #[test]
    fn test_relative() {
        let prog = [
            109, 1, 204, -1, 1001, 100, 1, 100, 1008, 100, 16, 101, 1006, 101,
            0, 99,
        ];
        test_run(&prog, &[], &prog);
    }

    #[test]
    fn test_large() {
        let prog = [1102, 34915192, 34915192, 7, 4, 7, 99, 0];
        test_run(&prog, &[], &[1219070632396864]); // a 16 digit number

        let prog = [104, 1125899906842624, 99];
        test_run(&prog, &[], &prog[1..=1]);
    }
}
