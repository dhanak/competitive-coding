extern crate aoc2019;

use std::iter::successors;

use aoc2019::utils::read_lines;

fn main() {
    let fuel: Vec<i64> = read_lines("input/day01.in")
        .map(|line| line.unwrap().parse::<i64>().unwrap())
        .map(|val| val / 3 - 2)
        .collect();
    println!("Q1: {}", fuel.iter().sum::<i64>());

    let fuel = fuel.iter().map(|&v| {
        successors(Some(v), |&v| Some(v / 3 - 2))
            .take_while(|&v| v > 0)
            .sum::<i64>()
    });
    println!("Q2: {}", fuel.sum::<i64>());
}
