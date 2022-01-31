use std::iter::successors;

use aoc2019::utils::read_lines;

fn q1(modules: &[i64]) -> i64 {
    modules.iter().map(|&val| val / 3 - 2).sum()
}

fn q2(modules: &[i64]) -> i64 {
    modules
        .iter()
        .map(|&val| val / 3 - 2)
        .map(|val| -> i64 {
            successors(Some(val), |&v| Some(v / 3 - 2))
                .take_while(|&v| v > 0)
                .sum()
        })
        .sum()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_q1() {
        assert_eq!(q1(&[12, 14, 1969, 100756]), 2 + 2 + 654 + 33583);
    }

    #[test]
    fn test_q2() {
        assert_eq!(q2(&[14, 1969, 100756]), 2 + 966 + 50346);
    }
}

fn main() {
    let modules: Vec<i64> = read_lines("input/day01.in")
        .map(|line| line.unwrap().parse::<i64>().unwrap())
        .collect();

    println!("Q1: {}", q1(&modules));
    println!("Q2: {}", q2(&modules));
}
