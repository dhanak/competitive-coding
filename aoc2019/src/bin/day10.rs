use std::fs::read_to_string;

use aoc2019::utils::{Angle, Point};
use itertools::Itertools;

fn main() {
    let asteroids = parse_asteroids(&read_to_string("input/day10.in").unwrap());
    println!("Q1: {}", q1(&asteroids));
    println!("Q2: {}", q2(&asteroids));
}

fn parse_asteroids(input: &str) -> Vec<Point<i32>> {
    let mut asteroids = vec![];
    for (y, row) in input.split('\n').enumerate() {
        for (x, ch) in row.chars().enumerate() {
            if ch == '#' {
                asteroids.push(Point::new(x as i32, y as i32))
            }
        }
    }
    asteroids
}

fn q1(asteroids: &[Point<i32>]) -> usize {
    find_station(asteroids).1
}

fn q2(asteroids: &[Point<i32>]) -> i32 {
    let center = find_station(asteroids).0;
    let mut asteroids = asteroids
        .iter()
        .filter(|&&point| point != center)
        .map(|&point| {
            let delta = point - center;
            let angle = (delta.r90().gradient().to_0_2pi() * 1e3) as i32;
            let dist = (delta.len() * 1e3) as i32;
            (angle, dist, point)
        })
        .collect::<Vec<_>>();
    asteroids.sort_by_key(|(angle, dist, _)| (*angle, *dist));

    // replace distances with ranks
    asteroids[0].1 = 0;
    let mut prev = asteroids[0];
    for point in &mut asteroids[1..] {
        point.1 = if point.0 == prev.0 { prev.1 + 1 } else { 0 };
        prev = *point;
    }

    // take the 200th item
    let point = asteroids
        .select_nth_unstable_by_key(199, |(angle, rank, _)| (*rank, *angle))
        .1
         .2;

    100 * point.x + point.y
}

fn find_station(asteroids: &[Point<i32>]) -> (Point<i32>, usize) {
    let sights: Vec<usize> = asteroids
        .iter()
        .map(|&center| {
            asteroids
                .iter()
                .filter(|&&point| point != center)
                .map(|&point| (point - center).gradient().to_bits())
                .unique()
                .count()
        })
        .collect();
    let i = sights.iter().position_max().unwrap();
    (asteroids[i], sights[i])
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_q1() {
        let test = ".#..#\n\
                    .....\n\
                    #####\n\
                    ....#\n\
                    ...##";
        assert_eq!(q1(&parse_asteroids(test)), 8);

        let test = "......#.#.\n\
                    #..#.#....\n\
                    ..#######.\n\
                    .#.#.###..\n\
                    .#..#.....\n\
                    ..#....#.#\n\
                    #..#....#.\n\
                    .##.#..###\n\
                    ##...#..#.\n\
                    .#....####";
        assert_eq!(q1(&parse_asteroids(test)), 33);

        let test = "#.#...#.#.\n\
                    .###....#.\n\
                    .#....#...\n\
                    ##.#.#.#.#\n\
                    ....#.#.#.\n\
                    .##..###.#\n\
                    ..#...##..\n\
                    ..##....##\n\
                    ......#...\n\
                    .####.###.";
        assert_eq!(q1(&parse_asteroids(test)), 35);

        let test = ".#..##.###...#######\n\
                    ##.############..##.\n\
                    .#.######.########.#\n\
                    .###.#######.####.#.\n\
                    #####.##.#.##.###.##\n\
                    ..#####..#.#########\n\
                    ####################\n\
                    #.####....###.#.#.##\n\
                    ##.#################\n\
                    #####.##.###..####..\n\
                    ..######..##.#######\n\
                    ####.##.####...##..#\n\
                    .#####..#.######.###\n\
                    ##...#.##########...\n\
                    #.##########.#######\n\
                    .####.#.###.###.#.##\n\
                    ....##.##.###..#####\n\
                    .#.#.###########.###\n\
                    #.#.#.#####.####.###\n\
                    ###.##.####.##.#..##";
        assert_eq!(q1(&parse_asteroids(test)), 210);
    }

    #[test]
    fn test_q2() {
        let test = ".#..##.###...#######\n\
                    ##.############..##.\n\
                    .#.######.########.#\n\
                    .###.#######.####.#.\n\
                    #####.##.#.##.###.##\n\
                    ..#####..#.#########\n\
                    ####################\n\
                    #.####....###.#.#.##\n\
                    ##.#################\n\
                    #####.##.###..####..\n\
                    ..######..##.#######\n\
                    ####.##.####...##..#\n\
                    .#####..#.######.###\n\
                    ##...#.##########...\n\
                    #.##########.#######\n\
                    .####.#.###.###.#.##\n\
                    ....##.##.###..#####\n\
                    .#.#.###########.###\n\
                    #.#.#.#####.####.###\n\
                    ###.##.####.##.#..##";
        assert_eq!(q2(&parse_asteroids(test)), 802);
    }
}
