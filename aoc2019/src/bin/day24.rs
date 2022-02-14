use std::collections::HashSet;

#[cfg(debug_assertions)]
use itertools::Itertools;

fn q1(bugs: &str) -> usize {
    let mut bugs: Vec<char> = bugs.chars().collect();
    let mut ratings = HashSet::new();
    while ratings.insert(rating(&bugs)) {
        bugs = bugs
            .iter()
            .enumerate()
            .map(|(i, &c)| {
                let i = i as i32;
                let n = [i - 6, i - 1, i + 1, i + 6]
                    .iter()
                    .filter(|&&j| bugs.get(j as usize) == Some(&'#'))
                    .count();
                match c {
                    '.' if (1..=2).contains(&n) => '#',
                    '#' if n != 1 => '.',
                    _ => c,
                }
            })
            .collect();
    }
    rating(&bugs)
}

fn plutonian_neighbors((d, i): (i32, usize)) -> Vec<(i32, usize)> {
    let (r, c) = ((i / 5) as i32, (i % 5) as i32);
    let mut neighbors = vec![];
    for (rn, cn) in [(r - 1, c), (r, c - 1), (r, c + 1), (r + 1, c)] {
        neighbors.extend(match (r, c, rn, cn) {
            (_, _, -1, _) => vec![(d - 1, 7)],
            (_, _, _, -1) => vec![(d - 1, 11)],
            (_, _, _, 5) => vec![(d - 1, 13)],
            (_, _, 5, _) => vec![(d - 1, 17)],
            (1, 2, 2, 2) => (0..5).map(|i| (d + 1, i)).collect(),
            (3, 2, 2, 2) => (20..25).map(|i| (d + 1, i)).collect(),
            (2, 1, 2, 2) => (0..21).step_by(5).map(|i| (d + 1, i)).collect(),
            (2, 3, 2, 2) => (4..25).step_by(5).map(|i| (d + 1, i)).collect(),
            _ => vec![(d, (rn * 5 + cn) as usize)],
        });
    }
    neighbors
}

#[cfg(debug_assertions)]
fn print_levels(tiles: &HashSet<(i32, usize)>) {
    let (&dmin, &dmax) =
        tiles.iter().map(|(d, _)| d).minmax().into_option().unwrap();
    for d in dmin..=dmax {
        println!("Depth {d}:");
        for r in 0..5 {
            for c in 0..5 {
                let p = (d, r * 5 + c);
                print!("{}", if tiles.contains(&p) { '#' } else { '.' });
            }
            println!();
        }
        println!();
    }
}

fn q2(bugs: &str, mins: usize) -> usize {
    let mut tiles: HashSet<(i32, usize)> = bugs
        .chars()
        .filter(|&c| ".#".contains(c))
        .enumerate()
        .filter(|&(_, c)| c == '#')
        .map(|(i, _)| (0, i))
        .collect();
    for _ in 0..mins {
        let mut next = tiles.clone();
        tiles
            .iter()
            .for_each(|&i| next.extend(plutonian_neighbors(i)));
        next.retain(|i| {
            let n = plutonian_neighbors(*i)
                .iter()
                .filter(|j| tiles.contains(j))
                .count();
            n == 1 || !tiles.contains(i) && n == 2
        });
        tiles = next;
    }
    #[cfg(debug_assertions)]
    print_levels(&tiles);
    tiles.len()
}

#[cfg(test)]
mod tests {
    use super::*;

    const INPUT: &str = "....#\n\
                         #..#.\n\
                         #..##\n\
                         ..#..\n\
                         #....";

    #[test]
    fn test_q1() {
        assert_eq!(q1(INPUT), 2129920);
    }

    #[test]
    fn test_neighbors() {
        assert_eq!(
            plutonian_neighbors((0, 18)),
            [(0, 13), (0, 17), (0, 19), (0, 23)]
        );
        assert_eq!(
            plutonian_neighbors((1, 6)), // G
            [(1, 1), (1, 5), (1, 7), (1, 11)]
        );
        assert_eq!(
            plutonian_neighbors((1, 3)), // D
            [(0, 7), (1, 2), (1, 4), (1, 8)]
        );
        assert_eq!(
            plutonian_neighbors((1, 4)), // E
            [(0, 7), (1, 3), (0, 13), (1, 9)]
        );
        assert_eq!(
            plutonian_neighbors((0, 13)),
            [
                (0, 8),
                (1, 4),
                (1, 9),
                (1, 14),
                (1, 19),
                (1, 24),
                (0, 14),
                (0, 18)
            ]
        );
        assert_eq!(
            plutonian_neighbors((1, 13)), // N
            [
                (1, 8),
                (2, 4),
                (2, 9),
                (2, 14),
                (2, 19),
                (2, 24),
                (1, 14),
                (1, 18),
            ]
        );
    }

    #[test]
    fn test_q2() {
        assert_eq!(q2(INPUT, 10), 99);
    }
}

fn main() {
    let input = "#..##\n\
                 #.#..\n\
                 #...#\n\
                 ##..#\n\
                 #..##";
    println!("Q1: {}", q1(input));
    println!("Q2: {}", q2(input, 200));
}

fn rating(bugs: &[char]) -> usize {
    bugs.iter()
        .filter(|&&c| ".#".contains(c))
        .enumerate()
        .map(|(i, &c)| if c == '#' { 1 << i } else { 0 })
        .sum()
}
