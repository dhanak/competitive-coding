use std::cmp::Reverse;
use std::collections::hash_map::Entry;
use std::collections::{BinaryHeap, HashMap, HashSet, VecDeque};
use std::fs::read_to_string;

use itertools::Itertools;

fn moves(
    vault: &[char],
    w: usize,
    p: usize,
    n_steps: usize,
    keys: usize,
) -> Vec<(usize, char, usize)> {
    let mut moves = vec![];
    let mut deque = VecDeque::from([p]);
    let mut dist = HashMap::from([(p, n_steps)]);
    while let Some(p) = deque.pop_front() {
        let n_steps = dist[&p] + 1;
        for p in [p - w - 1, p - 1, p + 1, p + w + 1] {
            match vault.get(p) {
                None | Some('#') => {}
                Some(&key @ 'a'..='z')
                    if keys & (1 << (key as u32 - 'a' as u32)) == 0 =>
                {
                    moves.push((p, key, n_steps));
                }
                Some(&gate @ 'A'..='Z')
                    if keys & (1 << (gate as u32 - 'A' as u32)) == 0 => {}
                Some(_) => {
                    if let Entry::Vacant(entry) = dist.entry(p) {
                        entry.insert(n_steps);
                        deque.push_back(p);
                    }
                }
            };
        }
    }
    moves
}

fn solve<const N: usize>(vault: &[char], p0: [usize; N]) -> usize {
    let n_keys = vault.iter().filter(|&c| ('a'..='z').contains(c)).count();
    let w = vault.iter().find_position(|&&c| c == '\n').unwrap().0;
    let mut heap = BinaryHeap::from([(Reverse(0), p0, 0)]);
    let mut visited = HashSet::<([usize; N], usize)>::new();
    while let Some((Reverse(n_steps), p, keys)) = heap.pop() {
        if !visited.insert((p, keys)) {
            continue;
        }
        if keys == (1 << n_keys) - 1 {
            return n_steps;
        }
        for i in 0..p.len() {
            for (pi, key, n_steps) in moves(vault, w, p[i], n_steps, keys) {
                assert!(('a'..='z').contains(&key));
                let keys = keys | (1 << (key as u32 - 'a' as u32));
                let mut p = p;
                p[i] = pi;
                heap.push((Reverse(n_steps), p, keys));
            }
        }
    }
    0
}

fn q1(vault: &[char]) -> usize {
    let p = vault.iter().find_position(|&&c| c == '@').unwrap().0;
    solve(vault, [p])
}

fn q2(vault: &[char]) -> usize {
    let mut vault = vault.to_owned();
    let w = vault.iter().find_position(|&&c| c == '\n').unwrap().0;
    let p = vault.iter().find_position(|&&c| c == '@').unwrap().0;
    assert_eq!(vault[p - w - 2..=p - w], ['.', '.', '.']);
    assert_eq!(vault[p - 1..=p + 1], ['.', '@', '.']);
    assert_eq!(vault[p + w..=p + w + 2], ['.', '.', '.']);
    vault.splice(p - w - 2..=p - w, ['@', '#', '@']);
    vault.splice(p - 1..=p + 1, ['#', '#', '#']);
    vault.splice(p + w..=p + w + 2, ['@', '#', '@']);
    solve(&vault, [p - w - 2, p - w, p + w, p + w + 2])
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_q1() {
        let test: Vec<char> = "#########\n\
                               #b.A.@.a#\n\
                               #########"
            .chars()
            .collect();
        assert_eq!(q1(&test), 8);

        let test: Vec<char> = "########################\n\
                               #f.D.E.e.C.b.A.@.a.B.c.#\n\
                               ######################.#\n\
                               #d.....................#\n\
                               ########################"
            .chars()
            .collect();
        assert_eq!(q1(&test), 86);

        let test: Vec<char> = "########################\n\
                               #...............b.C.D.f#\n\
                               #.######################\n\
                               #.....@.a.B.c.d.A.e.F.g#\n\
                               ########################"
            .chars()
            .collect();
        assert_eq!(q1(&test), 132);

        let test: Vec<char> = "#################\n\
                               #i.G..c...e..H.p#\n\
                               ########.########\n\
                               #j.A..b...f..D.o#\n\
                               ########@########\n\
                               #k.E..a...g..B.n#\n\
                               ########.########\n\
                               #l.F..d...h..C.m#\n\
                               #################"
            .chars()
            .collect();
        assert_eq!(q1(&test), 136);

        let test: Vec<char> = "########################\n\
                               #@..............ac.GI.b#\n\
                               ###d#e#f################\n\
                               ###A#B#C################\n\
                               ###g#h#i################\n\
                               ########################"
            .chars()
            .collect();
        assert_eq!(q1(&test), 81);
    }

    #[test]
    fn test_q2() {
        let test: Vec<char> = "#######\n\
                               #a.#Cd#\n\
                               ##...##\n\
                               ##.@.##\n\
                               ##...##\n\
                               #cB#Ab#\n\
                               #######"
            .chars()
            .collect();
        assert_eq!(q2(&test), 8);

        let test: Vec<char> = "###############\n\
                               #d.ABC.#.....a#\n\
                               ######...######\n\
                               ######.@.######\n\
                               ######...######\n\
                               #b.....#.....c#\n\
                               ###############"
            .chars()
            .collect();
        assert_eq!(q2(&test), 24);

        let test: Vec<char> = "#############\n\
                               #DcBa.#.GhKl#\n\
                               #.###...#I###\n\
                               #e#d#.@.#j#k#\n\
                               ###C#...###J#\n\
                               #fEbA.#.FgHi#\n\
                               #############\n\
                               "
        .chars()
        .collect();
        assert_eq!(q2(&test), 32);

        let test: Vec<char> = "#############\n\
                               #g#f.D#..h#l#\n\
                               #F###e#E###.#\n\
                               #dCba...BcIJ#\n\
                               #####.@.#####\n\
                               #nK.L...G...#\n\
                               #M###N#H###.#\n\
                               #o#m..#i#jk.#\n\
                               #############"
            .chars()
            .collect();
        assert_eq!(q2(&test), 72);
    }
}

fn main() {
    let vault: Vec<char> =
        read_to_string("input/day18.in").unwrap().chars().collect();
    println!("Q1: {}", q1(&vault));
    println!("Q2: {}", q2(&vault));
}
