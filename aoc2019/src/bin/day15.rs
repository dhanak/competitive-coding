use std::collections::HashMap;
use std::iter::successors;
use std::ops::Index;

use aoc2019::intcode::Intcode;
use itertools::Itertools;

type Point = aoc2019::utils::Point<i8>;

#[derive(Debug, Copy, Clone, Hash, PartialEq, Eq, PartialOrd, Ord)]
enum Square {
    Wall,
    Free,
    Oxygenator,
    Unknown,
}

impl From<i64> for Square {
    fn from(i: i64) -> Square {
        use Square::*;
        match i {
            0 => Wall,
            1 => Free,
            2 => Oxygenator,
            _ => panic!("Unknown square type {i}!"),
        }
    }
}

#[derive(Debug, Clone)]
struct Map(HashMap<Point, Square>);

impl Index<Point> for Map {
    type Output = Square;

    fn index(&self, index: Point) -> &Self::Output {
        self.0.get(&index).unwrap_or(&Square::Unknown)
    }
}

impl Map {
    pub fn new() -> Map {
        Map(HashMap::from([(Point::default(), Square::Free)]))
    }

    pub fn insert(&mut self, index: Point, v: Square) {
        self.0.insert(index, v);
    }

    pub fn find(&self, p: Square) -> Option<Point> {
        Some(*self.0.iter().find(|(_, &v)| v == p)?.0)
    }

    pub fn bounds(&self) -> [i8; 4] {
        let (l, r) = self.0.keys().map(|p| p.x).minmax().into_option().unwrap();
        let (t, b) = self.0.keys().map(|p| p.y).minmax().into_option().unwrap();
        [l, t, r, b]
    }

    pub fn draw(&self, droid: Point) {
        let [l, t, r, b] = self.bounds();
        for y in t..=b {
            for x in l..=r {
                let p = Point::new(x, y);
                let x = match self[p] {
                    _ if p == droid => "D",
                    Square::Wall => "#",
                    Square::Free => ".",
                    Square::Oxygenator => "O",
                    Square::Unknown => " ",
                };
                print!("{x}");
            }
            println!();
        }
    }
}

fn step(p: Point, dir: i8) -> Point {
    match dir {
        1 => Point::new(p.x, p.y - 1),
        2 => Point::new(p.x, p.y + 1),
        3 => Point::new(p.x - 1, p.y),
        4 => Point::new(p.x + 1, p.y),
        _ => panic!("Bad direction {dir}!"),
    }
}

fn find_path(map: &Map, start: Point, stop: Square) -> Result<Vec<i8>, usize> {
    let mut queue = vec![start];
    let mut parent = vec![(0, 0)];
    let mut i = 0;

    while i < queue.len() {
        let p = queue[i];
        if map[p] == stop {
            let mut path = vec![];
            while i != 0 {
                path.push(parent[i].1);
                i = parent[i].0;
            }
            path.reverse();
            return Ok(path);
        } else if map[p] == Square::Free || map[p] == Square::Oxygenator {
            for dir in 1..=4 {
                let p1 = step(p, dir);
                if !queue.contains(&p1) {
                    queue.push(p1);
                    parent.push((i, dir));
                }
            }
        }
        i += 1;
    }

    // no path, find distance of farthest free square
    let i = queue
        .iter()
        .enumerate()
        .rfind(|(_, &p)| map[p] == Square::Free)
        .unwrap()
        .0;
    let dist = successors(Some(i), |&i| match parent[i].0 {
        0 => None,
        v => Some(v),
    })
    .count();
    Err(dist)
}

fn main() {
    let code = Intcode::load("input/day15.in").unwrap();
    let (tx, rx) = code.run_in_thread();
    let mut map = Map::new();
    let mut p = Point::default();

    while let Ok(path) = find_path(&map, p, Square::Unknown) {
        let (&last_step, path0) = path.split_last().unwrap();
        path.iter().for_each(|&dir| tx.send(dir as i64).unwrap());
        p = path0.iter().fold(p, |p, &dir| step(p, dir));
        let mut resp: Vec<Square> = path
            .iter()
            .map(|_| Square::from(rx.recv().unwrap()))
            .collect();
        let sq = resp.pop().unwrap();
        assert!(resp.iter().all(|&s| s == Square::Free));
        map.insert(step(p, last_step), sq);
        if cfg!(debug_assertions) {
            map.draw(p);
            println!();
        }
        match sq {
            Square::Wall => {}
            Square::Free | Square::Oxygenator => p = step(p, last_step),
            Square::Unknown => panic!("Unexpected square {:?}!", sq),
        }
    }

    let q1 = find_path(&map, Point::default(), Square::Oxygenator)
        .unwrap()
        .len();
    println!("Q1: {q1}");

    let oxy = map.find(Square::Oxygenator).unwrap();
    let q2 = find_path(&map, oxy, Square::Unknown).err().unwrap();
    println!("Q2: {q2}");
}
