use aoc2019::intcode::Intcode;
use itertools::Itertools;

fn q1(scaffolds: &str) -> usize {
    let scaffolds = scaffolds
        .trim()
        .split('\n')
        .map(|s| s.chars().collect::<Vec<_>>())
        .collect::<Vec<_>>();

    let mut total = 0;
    for r in 1..scaffolds.len() - 1 {
        for c in 1..scaffolds[r].len() - 1 {
            let v = [
                scaffolds[r - 1][c],
                scaffolds[r][c - 1],
                scaffolds[r][c],
                scaffolds[r][c + 1],
                scaffolds[r + 1][c],
            ];
            if v.iter().all(|&c| c == '#') {
                total += r * c;
            }
        }
    }
    total
}

#[cfg(any(test, debug_assertions))]
fn q2_walk(scaffolds: &str) -> String {
    let scaffolds: Vec<char> = scaffolds.chars().collect();
    let w = scaffolds.iter().find_position(|&&c| c == '\n').unwrap().0 as i32;
    let (mut p, dir) = scaffolds
        .iter()
        .find_position(|&c| "^v<>".chars().contains(c))
        .unwrap();

    let step = [-(w + 1), -1, w + 1, 1];
    let mut dir = match dir {
        '^' => 0,
        '<' => 1,
        'v' => 2,
        '>' => 3,
        _ => panic!("bad robot!"),
    };

    let mut walk = String::new();
    let mut f = 0;

    while let Some((p1, dir1)) = [dir, (dir + 3) % 4, (dir + 1) % 4]
        .map(|d| ((p as i32 + step[d]) as usize, d))
        .iter()
        .find(|(p, _)| scaffolds.get(*p) == Some(&'#'))
    {
        match (dir1 + 4 - dir) % 4 {
            0 => f += 1,
            1 => {
                walk.push_str(&format!("{f},L,"));
                f = 1;
            }
            3 => {
                walk.push_str(&format!("{f},R,"));
                f = 1;
            }
            _ => panic!("Bad turn!"),
        }
        p = *p1;
        dir = *dir1;
    }
    walk.push_str(&format!("{f}"));

    walk.trim_start_matches("0,").to_string()
}

const Q2_PROG: &str = "A,B,A,B,C,B,C,A,C,C\n\
                       R,12,L,10,L,10\n\
                       L,6,L,12,R,12,L,4\n\
                       L,12,R,12,L,6\n\
                       n\n";

fn q2(mut code: Intcode) -> i64 {
    code[0] = 2;
    let prog = Q2_PROG
        .chars()
        .map(|c| u32::from(c) as i64)
        .collect::<Vec<_>>();
    *code.run_with(&prog).unwrap().last().unwrap()
}

fn main() {
    let code = Intcode::load("input/day17.in").unwrap();
    let out = code.clone().run_with(&[]).unwrap();
    let scaffolds = out
        .iter()
        .map(|&c| char::from_u32(c as u32).unwrap())
        .join("");

    #[cfg(debug_assertions)]
    println!("{}", scaffolds);
    println!("Q1: {}", q1(&scaffolds));
    #[cfg(debug_assertions)]
    println!("Q2 walk: {}", q2_walk(&scaffolds));
    println!("Q2: {}", q2(code));
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_q1() {
        let input = "..#..........\n\
                     ..#..........\n\
                     #######...###\n\
                     #.#...#...#.#\n\
                     #############\n\
                     ..#...#...#..\n\
                     ..#####...^..\n";
        assert_eq!(q1(input), 76);
    }

    #[test]
    fn test_q2() {
        let code = Intcode::load("input/day17.in").unwrap();
        let out = code.clone().run_with(&[]).unwrap();
        let scaffolds = out
            .iter()
            .map(|&c| char::from_u32(c as u32).unwrap())
            .join("");

        if let [m, a, b, c] =
            Q2_PROG.split('\n').take(4).collect::<Vec<_>>()[..]
        {
            let prog = m.replace('A', a).replace('B', b).replace('C', c);
            assert_eq!(q2_walk(&scaffolds), prog);
        } else {
            assert!(false);
        }
    }
}
