extern crate aoc2019;

use std::collections::HashMap;
use std::io;

use aoc2019::utils::read_lines;

const DEFAULT_OBJECTS: [(&str, usize); 3] =
    [("COM", 0), ("YOU", 1), ("SAN", 2)];

fn q1(orbits: &[i32]) -> i64 {
    let mut orbit_count = 0;
    for center in orbits.iter() {
        let mut object = *center as i32;
        while object != -1 {
            orbit_count += 1;
            object = orbits[object as usize];
        }
    }
    orbit_count
}

fn q2(orbits: &[i32]) -> i64 {
    let your_path = path_from(orbits, 1);
    let santas_path = path_from(orbits, 2);
    let diverge_at = (0..your_path.len())
        .find(|&i| your_path[i] != santas_path[i])
        .unwrap();
    (your_path.len() + santas_path.len() - 2 * diverge_at - 2) as i64
}

fn path_from(orbits: &[i32], object: i32) -> Vec<i32> {
    let mut object = object;
    let mut path = vec![];
    while object != -1 {
        path.push(object);
        object = orbits[object as usize];
    }
    path.reverse();
    path
}

fn parse_lines<L>(lines: L) -> io::Result<Vec<i32>>
where
    L: Iterator<Item = io::Result<String>>,
{
    let orbits: Vec<(String, String)> = lines
        .map(|line| {
            use std::io::{Error, ErrorKind::*};
            let line = line?;
            match line.split(')').collect::<Vec<_>>()[..] {
                [a, b] => Ok((a.to_owned(), b.to_owned())),
                _ => Err(Error::new(Other, format!("corrupt line: {}", line))),
            }
        })
        .collect::<io::Result<_>>()?;

    let mut objects = HashMap::from(DEFAULT_OBJECTS);
    let mut centers = vec![-1; objects.len()];
    for (center, object) in &orbits {
        let ci = *objects
            .entry(center)
            .or_insert_with(|| new_center(&mut centers));
        let oi = *objects
            .entry(object)
            .or_insert_with(|| new_center(&mut centers));
        assert_eq!(centers[oi], -1);
        centers[oi] = ci as i32;
    }

    Ok(centers)
}

fn new_center(centers: &mut Vec<i32>) -> usize {
    centers.push(-1);
    centers.len() - 1
}

#[cfg(test)]
mod tests {
    use super::*;

    const TEST_Q1: &str =
        "COM)B\nB)C\nC)D\nD)E\nE)F\nB)G\nG)H\nD)I\nE)J\nJ)K\nK)L";
    const TEST_Q2: &str = "\nK)YOU\nI)SAN";

    #[test]
    fn test_q1() -> io::Result<()> {
        let orbits =
            parse_lines(TEST_Q1.split('\n').map(|s| Ok(s.to_owned())))?;
        assert_eq!(q1(&orbits), 42);
        Ok(())
    }

    #[test]
    fn test_q2() -> io::Result<()> {
        let test = [TEST_Q1, TEST_Q2].concat();
        let orbits = parse_lines(test.split('\n').map(|s| Ok(s.to_owned())))?;
        assert_eq!(q2(&orbits), 4);
        Ok(())
    }
}

fn main() -> io::Result<()> {
    let orbits = parse_lines(read_lines("input/day06.in"))?;
    println!("Q1: {}", q1(&orbits));
    println!("Q2: {}", q2(&orbits));
    Ok(())
}
