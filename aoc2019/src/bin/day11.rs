use aoc2019::intcode::Intcode;
use aoc2019::utils::Point;

use itertools::Itertools;
use std::collections::HashMap;
use std::error::Error;

fn robot(
    code: Intcode,
    input: i64,
) -> Result<HashMap<Point<i32>, i64>, Box<dyn Error>> {
    let (tx, rx) = code.run_in_thread();
    tx.send(input)?;

    let mut tiles = HashMap::new();
    let mut dir = 0;
    let mut pos = Point::<i32>::default();
    while let (Ok(color), Ok(turn)) = (rx.recv(), rx.recv()) {
        tiles.insert(pos, color);
        dir = ((dir as i64 + (2 * turn - 1) + 4) % 4) as usize;
        pos = pos + Point::new([0, 1, 0, -1][dir], [-1, 0, 1, 0][dir]);
        tx.send(*tiles.get(&pos).unwrap_or(&0))?;
    }

    Ok(tiles)
}

fn q1(code: Intcode) -> Result<usize, Box<dyn Error>> {
    Ok(robot(code, 0)?.len())
}

fn q2(code: Intcode) -> Result<String, Box<dyn Error>> {
    let tiles = robot(code, 1)?;
    let (l, r) = tiles
        .keys()
        .map(|p| p.x)
        .minmax()
        .into_option()
        .unwrap_or_default();
    let (t, b) = tiles
        .keys()
        .map(|p| p.y)
        .minmax()
        .into_option()
        .unwrap_or_default();

    let mut out = String::new();
    for y in t..=b {
        for x in l..=r {
            let tile = match *tiles.get(&Point::new(x, y)).unwrap_or(&0) {
                0 => "  ",
                1 => "##",
                c => return Err(format!("invalid color: {}", c).into()),
            };
            out.push_str(tile);
        }
        out.push('\n');
    }
    Ok(out)
}

fn main() -> Result<(), Box<dyn Error>> {
    let code = Intcode::load("input/day11.in")?;
    println!("Q1: {}", q1(code.clone())?);
    println!("Q2:\n{}", q2(code)?);
    Ok(())
}
