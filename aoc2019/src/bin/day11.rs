use aoc2019::utils::Point;

use itertools::Itertools;
use std::collections::HashMap;
use std::error::Error;
use std::fmt::Display;
use std::fs::read_to_string;
use std::sync::mpsc::{channel, Receiver, Sender};
use std::thread;

#[derive(Debug, PartialEq)]
enum ProgramError {
    BadLen,
    DoesNotTerminate,
    InvalidOpCode(i64),
    InvalidMode(i64),
    InvalidColor(i64),
}

impl Display for ProgramError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl Error for ProgramError {}

#[derive(Debug, PartialEq)]
enum AddressMode {
    Position,
    Immediate,
    Relative(i64),
}

impl AddressMode {
    fn new(
        mode: i64,
        bit: u32,
        base: i64,
    ) -> Result<AddressMode, ProgramError> {
        use AddressMode::*;
        match mode / 10_i64.pow(bit + 2) % 10 {
            0 => Ok(Position),
            1 => Ok(Immediate),
            2 => Ok(Relative(base)),
            _ => Err(ProgramError::InvalidMode(mode)),
        }
    }

    fn read(self, mem: &[i64], addr: i64) -> i64 {
        use AddressMode::*;
        match self {
            Position => *mem.get(addr as usize).unwrap_or(&0),
            Immediate => addr,
            Relative(base) => *mem.get((base + addr) as usize).unwrap_or(&0),
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
            Position => addr,
            Immediate => return Err(ProgramError::InvalidMode(1)),
            Relative(base) => base + addr,
        } as usize;
        mem.resize(mem.len().max(addr + 1), 0);
        mem[addr] = value;
        Ok(())
    }
}

fn run(
    mut mem: Vec<i64>,
    rx: Receiver<i64>,
    tx: Sender<i64>,
) -> Result<(), Box<dyn Error>> {
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
                ma.write(&mut mem, a, rx.recv()?)?;
                ip += 2;
            }
            (4, &[a, ..]) => {
                // out
                tx.send(ma.read(&mem, a))?;
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
            (1..=9, _) => return Err(Box::new(ProgramError::BadLen)),
            (99, _) => return Ok(()),
            _ => return Err(Box::new(ProgramError::InvalidOpCode(opc))),
        };
    }
    Err(Box::new(ProgramError::DoesNotTerminate))
}

fn robot(
    mem: &[i64],
    input: i64,
) -> Result<HashMap<Point<i32>, i64>, Box<dyn Error>> {
    let mem = mem.to_vec();
    let ((tx, rxt), (txt, rx)) = (channel(), channel());
    thread::spawn(move || run(mem, rxt, txt).unwrap());
    tx.send(input)?;

    let mut tiles = HashMap::new();
    let mut dir = 0;
    let mut pos = Point::<i32>::default();
    while let (Ok(color), Ok(turn)) = (rx.recv(), rx.recv()) {
        tiles.insert(pos, color);
        dir = ((dir as i64 + (2 * turn - 1) + 4) % 4) as usize;
        pos = pos + Point::new([0, 1, 0, -1][dir], [-1, 0, 1, 0][dir]);
        tx.send(*tiles.get(&pos).unwrap_or(&0))?;
    }

    Ok(tiles)
}

fn q1(mem: &[i64]) -> Result<usize, Box<dyn Error>> {
    Ok(robot(mem, 0)?.len())
}

fn q2(mem: &[i64]) -> Result<String, Box<dyn Error>> {
    let tiles = robot(mem, 1)?;
    let (l, r) = tiles
        .keys()
        .map(|p| p.x)
        .minmax()
        .into_option()
        .unwrap_or_default();
    let (t, b) = tiles
        .keys()
        .map(|p| p.y)
        .minmax()
        .into_option()
        .unwrap_or_default();

    let mut out = String::new();
    for y in t..=b {
        for x in l..=r {
            let tile = match *tiles.get(&Point::new(x, y)).unwrap_or(&0) {
                0 => "  ",
                1 => "##",
                c => return Err(Box::new(ProgramError::InvalidColor(c))),
            };
            out.push_str(tile);
        }
        out.push('\n');
    }
    Ok(out)
}

fn main() -> Result<(), Box<dyn Error>> {
    let mem: Vec<i64> = read_to_string("input/day11.in")?
        .trim()
        .split(',')
        .map(|v| v.parse::<i64>())
        .collect::<Result<Vec<i64>, _>>()?;

    println!("Q1: {}", q1(&mem)?);
    println!("Q2:\n{}", q2(&mem)?);
    Ok(())
}
