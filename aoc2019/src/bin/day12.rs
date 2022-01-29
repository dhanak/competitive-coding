use itertools::izip;
use num::integer::lcm;
use std::collections::HashSet;

fn moons<const N: usize>(initial: &[[i32; N]]) -> Vec<([i32; N], [i32; N])> {
    initial.iter().map(|&pos| (pos, [0; N])).collect::<Vec<_>>()
}

fn steps<const N: usize>(moons: &mut [([i32; N], [i32; N])], n: i32) {
    for _ in 0..n {
        step(moons);
    }
}

fn step<const N: usize>(moons: &mut [([i32; N], [i32; N])]) {
    // update velocities
    for i in 0..moons.len() {
        let (pa, mut va) = moons[i];
        for (pb, _) in moons.iter() {
            for (v, a, b) in izip!(&mut va, &pa, pb) {
                *v += (b - a).signum();
            }
        }
        moons[i] = (pa, va);
    }

    // update positions
    for (p, v) in moons {
        for i in 0..p.len() {
            p[i] += v[i];
        }
    }
}

fn step_coord(moons: &mut [(i32, i32)]) {
    // update velocities
    for i in 0..moons.len() {
        let (a, mut v) = moons[i];
        for (b, _) in moons.iter() {
            v += (b - a).signum();
        }
        moons[i] = (a, v);
    }

    // update positions
    for (p, v) in moons {
        *p += *v;
    }
}

fn energy<const N: usize>(moons: &[([i32; N], [i32; N])]) -> i32 {
    moons
        .iter()
        .map(|moon| {
            let pot: i32 = moon.0.iter().map(|v| v.abs()).sum();
            let kin: i32 = moon.1.iter().map(|v| v.abs()).sum();
            pot * kin
        })
        .sum()
}

fn q2<const N: usize>(initial: &[[i32; N]]) -> usize {
    (0..N)
        .map(|i| cycle_len(initial.iter().map(move |p| (p[i], 0)).collect()))
        .fold(1, lcm)
}

fn cycle_len(mut moons: Vec<(i32, i32)>) -> usize {
    let mut seen = HashSet::new();
    while !seen.contains(&moons) {
        seen.insert(moons.clone());
        step_coord(&mut moons);
    }
    seen.len()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_ex1() {
        let input = [[-1, 0, 2], [2, -10, -7], [4, -8, 8], [3, 5, -1]];
        let mut moons = moons(&input);
        steps(&mut moons, 10);
        assert_eq!(
            moons,
            [
                ([2, 1, -3], [-3, -2, 1]),
                ([1, -8, 0], [-1, 1, 3]),
                ([3, -6, 1], [3, 2, -3]),
                ([2, 0, 4], [1, -1, -1]),
            ]
        );
        assert_eq!(energy(&moons), 179);
        assert_eq!(q2(&input), 2772);
    }

    #[test]
    fn test_ex2() {
        let input = [[-8, -10, 0], [5, 5, 10], [2, -7, 3], [9, -8, -3]];
        let mut moons = moons(&input);
        steps(&mut moons, 100);
        assert_eq!(
            moons,
            [
                ([8, -12, -9], [-7, 3, 0]),
                ([13, 16, -3], [3, -11, -5]),
                ([-29, -11, -1], [-3, 7, 4]),
                ([16, -13, 23], [7, 1, 1]),
            ]
        );
        assert_eq!(energy(&moons), 1940);
        assert_eq!(q2(&input), 4686774924);
    }
}

fn main() {
    let input = [[14, 4, 5], [12, 10, 8], [1, 7, -10], [16, -5, 3]];
    let mut moons = moons(&input);
    steps(&mut moons, 1000);
    println!("Q1: {}", energy(&moons));
    println!("Q2: {}", q2(&input));
}
