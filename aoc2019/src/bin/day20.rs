use std::{
    cmp::Reverse,
    collections::{hash_map::Entry, BinaryHeap, HashMap, HashSet},
    fs::read_to_string,
};

use itertools::Itertools;

struct Portals(usize, usize, HashMap<usize, (usize, i8)>);

impl Portals {
    fn entry(&self) -> usize {
        self.0
    }

    fn exit(&self) -> usize {
        self.1
    }

    fn gate(&self, p: usize) -> Option<(usize, i8)> {
        self.2.get(&p).copied()
    }
}

fn side(p: i32, w: i32, h: i32) -> usize {
    let x = p % w;
    let y = p / w;
    if x == 2 || y == 2 || x == w - 4 || y == h - 3 {
        0
    } else {
        1
    }
}

fn find_portals(maze: &[char]) -> Portals {
    let w = maze.iter().find_position(|&&c| c == '\n').unwrap().0 as i32 + 1;
    let h = maze.len() as i32 / w;
    let letters = maze.iter().enumerate().filter(|&(_, c)| c.is_uppercase());
    let mut portals = HashMap::new();
    for (p, _) in letters {
        let p0 = p as i32;
        for p1 in [p0 - w, p0 - 1, p0 + 1, p0 + w] {
            if maze.get(p1 as usize) == Some(&'.') {
                let mut code = [p0, 2 * p0 - p1];
                code.sort_unstable();
                let code = code.map(|i| maze[i as usize]).iter().join("");
                let side = side(p1, w, h);
                match portals.entry(code) {
                    Entry::Vacant(entry) => {
                        entry.insert([0; 2])[side] = p1 as usize;
                    }
                    Entry::Occupied(mut entry) => {
                        assert_eq!(entry.get()[side], 0);
                        entry.get_mut()[side] = p1 as usize;
                    }
                }
            }
        }
    }
    let mut pairs = HashMap::new();
    for &[a, b] in portals.values() {
        if a != 0 && b != 0 {
            pairs.insert(a, (b, -1));
            pairs.insert(b, (a, 1));
        }
    }
    Portals(portals["AA"][0], portals["ZZ"][0], pairs)
}

fn moves_from(maze: &[char], start: usize) -> Vec<(usize, usize)> {
    let w = maze.iter().find_position(|&&c| c == '\n').unwrap().0 + 1;
    let mut queue = vec![start];
    let mut dists = vec![0];
    let mut moves = vec![];
    let mut i = 0;
    while i < queue.len() {
        let p = queue[i];
        let d = dists[i];
        for p1 in [p - w, p - 1, p + 1, p + w] {
            if maze[p1] == '.' && !queue.contains(&p1) {
                queue.push(p1);
                dists.push(d + 1);
            } else if maze[p1].is_uppercase() && p != start {
                moves.push((p, d));
            }
        }
        i += 1;
    }
    moves
}

fn moves(
    maze: &[char],
    portals: &Portals,
) -> HashMap<usize, Vec<(usize, usize)>> {
    HashMap::from_iter(
        [portals.entry()]
            .iter()
            .chain(portals.2.keys())
            .map(|&p| (p, moves_from(maze, p))),
    )
}

fn solve(maze: &[char], levels: bool) -> usize {
    let portals = find_portals(maze);
    let moves = moves(maze, &portals);
    let mut heap = BinaryHeap::from([(Reverse(0), portals.entry(), 0)]);
    let mut visited = HashSet::new();
    while let Some((Reverse(d), p, l)) = heap.pop() {
        if !visited.insert((p, l)) {
            continue;
        }
        if p == portals.exit() {
            if l == 0 {
                return d;
            }
        } else {
            for &(p1, d1) in &moves[&p] {
                heap.push((Reverse(d + d1), p1, l));
            }
            if let Some((p1, dl)) = portals.gate(p) {
                let l1 = l + dl * levels as i8;
                if l1 >= 0 {
                    heap.push((Reverse(d + 1), p1, l1));
                }
            }
        }
    }
    0
}

fn main() {
    let maze: Vec<char> =
        read_to_string("input/day20.in").unwrap().chars().collect();
    println!("Q1: {}", solve(&maze, false));
    println!("Q2: {}", solve(&maze, true));
}

#[cfg(test)]
mod tests {
    use super::*;

    fn check(file: &str, levels: bool, solution: usize) {
        let maze: Vec<char> = read_to_string(file).unwrap().chars().collect();
        assert_eq!(solve(&maze, levels), solution);
    }

    #[test]
    fn test_q1() {
        check("input/day20_t1.in", false, 23);
        check("input/day20_t2.in", false, 58);
    }

    #[test]
    fn test_q2() {
        check("input/day20_t1.in", true, 26);
        check("input/day20_t3.in", true, 396);
    }
}
