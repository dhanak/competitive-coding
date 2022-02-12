use std::{collections::VecDeque, i64::MAX};

use aoc2019::intcode::Intcode;

fn q1(code: &Intcode) -> i64 {
    let channels = (0..50)
        .map(|i| {
            let (tx, rx) = code.clone().run_in_thread();
            tx.send(i).unwrap();
            (tx, rx)
        })
        .collect::<Vec<_>>();
    let mut queues: [VecDeque<(i64, i64)>; 50] =
        [0; 50].map(|_| VecDeque::new());
    for (i, (tx, rx)) in channels.iter().enumerate().cycle() {
        let v = rx.recv().unwrap();
        if v < MAX {
            let (x, y) = (rx.recv().unwrap(), rx.recv().unwrap());
            if v == 255 {
                return y;
            }
            queues[v as usize].push_back((x, y));
        } else if let Some((x, y)) = queues[i].pop_front() {
            tx.send(x).unwrap();
            tx.send(y).unwrap();
        } else {
            tx.send(-1).unwrap();
        }
    }
    0
}

fn q2(code: &Intcode) -> i64 {
    let channels = (0..50)
        .map(|i| {
            let (tx, rx) = code.clone().run_in_thread();
            tx.send(i).unwrap();
            (tx, rx)
        })
        .collect::<Vec<_>>();
    let mut queues: [VecDeque<(i64, i64)>; 50] =
        [0; 50].map(|_| VecDeque::new());
    let mut nat_packet = None;
    let mut idle_count = 0;
    let mut last_0_y = None;
    for (i, (tx, rx)) in channels.iter().enumerate().cycle() {
        let v = rx.recv().unwrap();
        if v < MAX {
            let packet = (rx.recv().unwrap(), rx.recv().unwrap());
            if v == 255 {
                nat_packet = Some(packet);
            } else {
                queues[v as usize].push_back(packet);
            }
            idle_count = 0;
        } else if let Some((x, y)) = queues[i].pop_front() {
            tx.send(x).unwrap();
            tx.send(y).unwrap();
            idle_count = 0;
        } else if let (0, 2000.., Some((x, y))) = (i, idle_count, nat_packet) {
            if last_0_y == Some(y) {
                break;
            }
            tx.send(x).unwrap();
            tx.send(y).unwrap();
            last_0_y = Some(y);
            nat_packet = None;
            idle_count = 0;
        } else {
            tx.send(-1).unwrap();
            idle_count += 1;
        }
    }
    last_0_y.unwrap()
}

fn main() {
    let mut code = Intcode::load("input/day23.in").unwrap();
    code.trigger = Some(MAX);
    println!("Q1: {}", q1(&code));
    println!("Q2: {}", q2(&code));
}
