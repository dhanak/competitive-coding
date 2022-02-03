use std::{fs::read_to_string, iter::repeat};

fn pattern(n: usize) -> Box<dyn Iterator<Item = i32>> {
    Box::new(
        repeat(0)
            .take(n)
            .chain(repeat(1).take(n))
            .chain(repeat(0).take(n))
            .chain(repeat(-1).take(n))
            .cycle()
            .skip(1),
    )
}

fn fft_phase(input: &[i32]) -> Vec<i32> {
    (1..=input.len())
        .map(|i| {
            pattern(i)
                .zip(input)
                .map(|(a, &b)| a * b)
                .sum::<i32>()
                .abs()
                % 10
        })
        .collect()
}

fn q1(input: &[i32]) -> i32 {
    let mut v = input.to_vec();
    (0..100).for_each(|_| v = fft_phase(&v));
    v[0..8].iter().fold(0, |v, d| v * 10 + d)
}

fn q2(input: &[i32]) -> i32 {
    let offset = input[0..7].iter().fold(0, |v, d| v * 10 + d) as usize;
    let mut v = input
        .iter()
        .cycle()
        .take(10000 * input.len())
        .skip(offset)
        .copied()
        .collect::<Vec<_>>();
    assert!(offset > v.len());

    v.reverse();
    (0..100).for_each(|_| {
        (1..v.len()).for_each(|i| v[i] = (v[i] + v[i - 1]) % 10);
    });
    v.reverse();

    v[0..8].iter().fold(0, |v, d| v * 10 + d)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_fft() {
        let mut v = vec![1, 2, 3, 4, 5, 6, 7, 8];
        v = fft_phase(&v);
        assert_eq!(v, [4, 8, 2, 2, 6, 1, 5, 8]);
        v = fft_phase(&v);
        assert_eq!(v, [3, 4, 0, 4, 0, 4, 3, 8]);
        v = fft_phase(&v);
        assert_eq!(v, [0, 3, 4, 1, 5, 5, 1, 8]);
        v = fft_phase(&v);
        assert_eq!(v, [0, 1, 0, 2, 9, 4, 9, 8]);
    }

    fn q1_str(input: &str) -> i32 {
        let input: Vec<i32> = input
            .chars()
            .map(|c| c.to_digit(10).unwrap() as i32)
            .collect();
        q1(&input)
    }

    #[test]
    fn test_q1() {
        assert_eq!(q1_str("80871224585914546619083218645595"), 24176176);
        assert_eq!(q1_str("19617804207202209144916044189917"), 73745418);
        assert_eq!(q1_str("69317163492948606335995924319873"), 52432133);
    }

    fn q2_str(input: &str) -> i32 {
        let input: Vec<i32> = input
            .chars()
            .map(|c| c.to_digit(10).unwrap() as i32)
            .collect();
        q2(&input)
    }

    #[test]
    fn test_q2() {
        assert_eq!(q2_str("03036732577212944063491565474664"), 84462026);
        assert_eq!(q2_str("02935109699940807407585447034323"), 78725270);
        assert_eq!(q2_str("03081770884921959731165446850517"), 53553731);
    }
}

fn main() {
    let input: Vec<i32> = read_to_string("input/day16.in")
        .unwrap()
        .trim()
        .chars()
        .map(|c| c.to_digit(10).unwrap() as i32)
        .collect();

    println!("Q1: {}", q1(&input));
    println!("Q2: {}", q2(&input));
}
