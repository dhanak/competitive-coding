use std::iter::repeat_with;
use std::sync::mpsc::{channel, Receiver, Sender};
use std::thread;

use aoc2019::intcode::Intcode;
use itertools::{zip, Itertools};

fn run_sequence(code: &Intcode, seq: &[i64]) -> i64 {
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
        let mut code = code.clone();
        thread::spawn(move || code.run(rx, tx).unwrap());
    }

    // loop last Receiver and first Sender until it terimates
    let mut out = 0;
    while let Ok(v) = rx.recv() {
        let _ = tx.send(v); // last send may fail, that's ok
        out = v;
    }
    out
}

fn q1(code: &Intcode) -> i64 {
    (0..=4)
        .permutations(5)
        .map(|seq| run_sequence(code, &seq))
        .max()
        .unwrap()
}

fn q2(code: &Intcode) -> i64 {
    (5..=9)
        .permutations(5)
        .map(|seq| run_sequence(code, &seq))
        .max()
        .unwrap()
}

fn main() {
    let code = Intcode::load("input/day07.in").unwrap();
    println!("Q1: {}", q1(&code));
    println!("Q2: {}", q2(&code));
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_run_sequence() {
        let code = Intcode::from([
            3, 15, 3, 16, 1002, 16, 10, 16, 1, 16, 15, 15, 4, 15, 99, 0, 0,
        ]);
        assert_eq!(run_sequence(&code, &[4, 3, 2, 1, 0]), 43210);

        let code = Intcode::from([
            3, 23, 3, 24, 1002, 24, 10, 24, 1002, 23, -1, 23, 101, 5, 23, 23,
            1, 24, 23, 23, 4, 23, 99, 0, 0,
        ]);
        assert_eq!(run_sequence(&code, &[0, 1, 2, 3, 4]), 54321);
    }

    #[test]
    fn test_q1() {
        let code = Intcode::from([
            3, 15, 3, 16, 1002, 16, 10, 16, 1, 16, 15, 15, 4, 15, 99, 0, 0,
        ]);
        assert_eq!(q1(&code), 43210);

        let code = Intcode::from([
            3, 23, 3, 24, 1002, 24, 10, 24, 1002, 23, -1, 23, 101, 5, 23, 23,
            1, 24, 23, 23, 4, 23, 99, 0, 0,
        ]);
        assert_eq!(q1(&code), 54321);

        let code = Intcode::from([
            3, 31, 3, 32, 1002, 32, 10, 32, 1001, 31, -2, 31, 1007, 31, 0, 33,
            1002, 33, 7, 33, 1, 33, 31, 31, 1, 32, 31, 31, 4, 31, 99, 0, 0, 0,
        ]);
        assert_eq!(q1(&code), 65210);
    }

    #[test]
    fn test_q2() {
        let code = Intcode::from([
            3, 26, 1001, 26, -4, 26, 3, 27, 1002, 27, 2, 27, 1, 27, 26, 27, 4,
            27, 1001, 28, -1, 28, 1005, 28, 6, 99, 0, 0, 5,
        ]);
        assert_eq!(q2(&code), 139629729);
        let code = Intcode::from([
            3, 52, 1001, 52, -5, 52, 3, 53, 1, 52, 56, 54, 1007, 54, 5, 55,
            1005, 55, 26, 1001, 54, -5, 54, 1105, 1, 12, 1, 53, 54, 53, 1008,
            54, 0, 55, 1001, 55, 1, 55, 2, 53, 55, 53, 4, 53, 1001, 56, -1, 56,
            1005, 56, 6, 99, 0, 0, 0, 0, 10,
        ]);
        assert_eq!(q2(&code), 18216);
    }
}
