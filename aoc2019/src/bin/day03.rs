extern crate aoc2019;

use std::collections::HashSet;
use std::io;
use std::iter::successors;

use aoc2019::utils::{read_lines, Point};

fn path_to_coords(path: &str) -> Vec<Point<i64>> {
    let mut wire = vec![Point::default()];
    for p in path.split(',') {
        let l: i64 = p[1..].parse().unwrap();
        let m = match p.chars().nth(0).unwrap() {
            'L' => Point { x: -1, y: 0 },
            'R' => Point { x: 1, y: 0 },
            'U' => Point { x: 0, y: -1 },
            'D' => Point { x: 0, y: 1 },
            _ => panic!("unexpected move"),
        };
        let line = successors(wire.last().cloned(), |&p| Some(p + m)).skip(1);
        wire.extend(line.take(l as usize));
    }
    wire
}

fn q1(wire1: &[Point<i64>], wire2: &[Point<i64>]) -> i64 {
    let s1: HashSet<&Point<i64>> = HashSet::from_iter(wire1);
    let s2: HashSet<&Point<i64>> = HashSet::from_iter(wire2);
    *s1.intersection(&s2)
        .map(|&p| p.x.abs() + p.y.abs())
        .collect::<Vec<i64>>()
        .select_nth_unstable(1)
        .1
}

fn q2(wire1: &[Point<i64>], wire2: &[Point<i64>]) -> i64 {
    let s1: HashSet<&Point<i64>> = HashSet::from_iter(wire1);
    let s2: HashSet<&Point<i64>> = HashSet::from_iter(wire2);
    *s1.intersection(&s2)
        .map(|&pi| {
            let i1 = wire1.iter().position(|p1| p1 == pi).unwrap();
            let i2 = wire2.iter().position(|p2| p2 == pi).unwrap();
            i1 + i2
        })
        .collect::<Vec<usize>>()
        .select_nth_unstable(1)
        .1 as i64
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_q1() {
        let wire1 = path_to_coords("R8,U5,L5,D3");
        let wire2 = path_to_coords("U7,R6,D4,L4");
        assert_eq!(q1(&wire1, &wire2), 6);

        let wire1 = path_to_coords("R75,D30,R83,U83,L12,D49,R71,U7,L72");
        let wire2 = path_to_coords("U62,R66,U55,R34,D71,R55,D58,R83");
        assert_eq!(q1(&wire1, &wire2), 159);
    }

    #[test]
    fn test_q2() {
        let wire1 = path_to_coords("R8,U5,L5,D3");
        let wire2 = path_to_coords("U7,R6,D4,L4");
        assert_eq!(q2(&wire1, &wire2), 30);

        let wire1 = path_to_coords("R75,D30,R83,U83,L12,D49,R71,U7,L72");
        let wire2 = path_to_coords("U62,R66,U55,R34,D71,R55,D58,R83");
        assert_eq!(q2(&wire1, &wire2), 610);
    }
}

fn main() {
    let lines = read_lines("input/day03.in")
        .collect::<io::Result<Vec<_>>>()
        .unwrap();
    let (wire1, wire2) = match &lines[..] {
        [wire1, wire2] => (path_to_coords(wire1), path_to_coords(wire2)),
        _ => panic!("Expected two lines"),
    };

    println!("Q1: {}", q1(&wire1, &wire2));
    println!("Q2: {}", q2(&wire1, &wire2));
}
