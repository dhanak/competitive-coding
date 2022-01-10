extern crate aoc2019;

use aoc2019::utils::read_lines;

fn q1() -> i64 { 0 }

fn q2() -> i64 { 0 }

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_q1() {
        assert_eq!(q1(), 0);
    }

    #[test]
    fn test_q2() {
        assert_eq!(q2(), 0);
    }
}

fn main() {
    let input: Vec<i64> = read_lines("input/day00.in")
        .map(|line| line.unwrap().parse::<i64>().unwrap())
        .collect();

    println!("Q1: {}", q1());
    println!("Q2: {}", q2());
}
