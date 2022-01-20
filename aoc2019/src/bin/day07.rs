use std::error::Error;
use std::fmt::Display;
use std::fs::read_to_string;
use std::iter::repeat_with;
use std::sync::mpsc::{channel, Receiver, Sender};
use std::thread;

use itertools::{zip, Itertools};

#[derive(Debug, PartialEq)]
enum ProgramError {
    BadLen,
    DoesNotTerminate,
    InvalidOpCode(i64),
    InvalidMode(i64),
}

impl Display for ProgramError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl Error for ProgramError {}

fn run(
    mut mem: Vec<i64>,
    rx: Receiver<i64>,
    tx: Sender<i64>,
) -> Result<(), Box<dyn Error>> {
    let mut ip = 0;
    while ip < mem.len() {
        let opc = mem[ip] % 100;
        let ma = (mem[ip] / 100) % 10;
        let mb = (mem[ip] / 1000) % 10;
        let mc = (mem[ip] / 10000) % 10;
        match (opc, &mem[ip + 1..]) {
            (1, &[a, b, c, ..]) => {
                // add
                assert_eq!(mc, 0);
                mem[c as usize] = par(&mem, ma, a)? + par(&mem, mb, b)?;
                ip += 4;
            }
            (2, &[a, b, c, ..]) => {
                // mul
                assert_eq!(mc, 0);
                mem[c as usize] = par(&mem, ma, a)? * par(&mem, mb, b)?;
                ip += 4;
            }
            (3, &[a, ..]) => {
                // inp
                assert_eq!(ma, 0);
                mem[a as usize] = rx.recv()?;
                ip += 2;
            }
            (4, &[a, ..]) => {
                // out
                tx.send(par(&mem, ma, a)?)?;
                ip += 2;
            }
            (5 | 6, &[a, b, ..]) => {
                // jump if true | false
                let a = par(&mem, ma, a)?;
                if (opc == 5 && a != 0) || (opc == 6 && a == 0) {
                    ip = par(&mem, mb, b)? as usize;
                } else {
                    ip += 3;
                }
            }
            (7, &[a, b, c, ..]) => {
                // less than
                assert_eq!(mc, 0);
                mem[c as usize] =
                    (par(&mem, ma, a)? < par(&mem, mb, b)?) as i64;
                ip += 4;
            }
            (8, &[a, b, c, ..]) => {
                // equal
                assert_eq!(mc, 0);
                mem[c as usize] =
                    (par(&mem, ma, a)? == par(&mem, mb, b)?) as i64;
                ip += 4;
            }
            (1..=8, _) => return Err(Box::new(ProgramError::BadLen)),
            (99, _) => return Ok(()),
            _ => return Err(Box::new(ProgramError::InvalidOpCode(opc))),
        };
    }
    Err(Box::new(ProgramError::DoesNotTerminate))
}

fn par(mem: &[i64], mode: i64, value: i64) -> Result<i64, ProgramError> {
    match mode {
        0 => Ok(mem[value as usize]), // position mode
        1 => Ok(value),               // immediate mode
        _ => Err(ProgramError::InvalidMode(mode)),
    }
}

fn run_sequence(mem: &[i64], seq: &[i64]) -> i64 {
    // create channels
    let (mut txs, mut rxs): (Vec<Sender<i64>>, Vec<Receiver<i64>>) =
        repeat_with(channel).take(seq.len() + 1).unzip();

    // initialize amplifiers with phase sequence
    for (tx, &n) in zip(&txs, seq) {
        tx.send(n).unwrap();
    }

    // peel off first Sender and last Receiver
    let tx = txs.remove(0);
    let rx = rxs.pop().unwrap();
    tx.send(0).unwrap();

    // start amplifiers and connect channels
    for (tx, rx) in zip(txs, rxs) {
        let mem = mem.to_vec();
        thread::spawn(move || run(mem, rx, tx).unwrap());
    }

    // loop last Receiver and first Sender until it terimates
    let mut out = 0;
    while let Ok(v) = rx.recv() {
        let _ = tx.send(v); // last send may fail, that's ok
        out = v;
    }
    out
}

fn q1(mem: &[i64]) -> i64 {
    (0..=4)
        .permutations(5)
        .map(|seq| run_sequence(mem, &seq))
        .max()
        .unwrap()
}

fn q2(mem: &[i64]) -> i64 {
    (5..=9)
        .permutations(5)
        .map(|seq| run_sequence(mem, &seq))
        .max()
        .unwrap()
}

fn main() {
    let mem: Vec<i64> = read_to_string("input/day07.in")
        .unwrap()
        .trim()
        .split(',')
        .map(|v| v.parse::<i64>().unwrap())
        .collect();

    println!("Q1: {}", q1(&mem));
    println!("Q2: {}", q2(&mem));
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_run_sequence() {
        assert_eq!(
            run_sequence(
                &[
                    3, 15, 3, 16, 1002, 16, 10, 16, 1, 16, 15, 15, 4, 15, 99,
                    0, 0
                ],
                &[4, 3, 2, 1, 0]
            ),
            43210
        );
        assert_eq!(
            run_sequence(
                &[
                    3, 23, 3, 24, 1002, 24, 10, 24, 1002, 23, -1, 23, 101, 5,
                    23, 23, 1, 24, 23, 23, 4, 23, 99, 0, 0
                ],
                &[0, 1, 2, 3, 4]
            ),
            54321
        );
    }

    #[test]
    fn test_q1() {
        assert_eq!(
            q1(&[
                3, 15, 3, 16, 1002, 16, 10, 16, 1, 16, 15, 15, 4, 15, 99, 0, 0
            ]),
            43210
        );
        assert_eq!(
            q1(&[
                3, 23, 3, 24, 1002, 24, 10, 24, 1002, 23, -1, 23, 101, 5, 23,
                23, 1, 24, 23, 23, 4, 23, 99, 0, 0
            ]),
            54321
        );
        assert_eq!(
            q1(&[
                3, 31, 3, 32, 1002, 32, 10, 32, 1001, 31, -2, 31, 1007, 31, 0,
                33, 1002, 33, 7, 33, 1, 33, 31, 31, 1, 32, 31, 31, 4, 31, 99,
                0, 0, 0
            ]),
            65210
        );
    }

    #[test]
    fn test_q2() {
        assert_eq!(
            q2(&[
                3, 26, 1001, 26, -4, 26, 3, 27, 1002, 27, 2, 27, 1, 27, 26, 27,
                4, 27, 1001, 28, -1, 28, 1005, 28, 6, 99, 0, 0, 5
            ]),
            139629729
        );
        assert_eq!(
            q2(&[
                3, 52, 1001, 52, -5, 52, 3, 53, 1, 52, 56, 54, 1007, 54, 5, 55,
                1005, 55, 26, 1001, 54, -5, 54, 1105, 1, 12, 1, 53, 54, 53,
                1008, 54, 0, 55, 1001, 55, 1, 55, 2, 53, 55, 53, 4, 53, 1001,
                56, -1, 56, 1005, 56, 6, 99, 0, 0, 0, 0, 10
            ]),
            18216
        );
    }
}
