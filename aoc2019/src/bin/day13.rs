use aoc2019::intcode::Intcode;

#[cfg(debug_assertions)]
use aoc2019::arcade_screen::Screen;

const TRIGGER: i64 = -2;

fn q1(code: Intcode) -> i64 {
    let (_, rx) = code.run_in_thread();
    let mut blocks = 0;
    #[cfg(debug_assertions)]
    let mut screen = Screen::<45, 26>::new();
    while let (Ok(_x), Ok(_y), Ok(tile_id)) = (rx.recv(), rx.recv(), rx.recv())
    {
        if tile_id == 2 {
            blocks += 1;
        }
        #[cfg(debug_assertions)]
        screen.update(_x, _y, tile_id);
    }
    #[cfg(debug_assertions)]
    println!("{}", screen);
    blocks
}

fn q2(mut code: Intcode) -> i64 {
    code[0] = 2; // play for free
    code.trigger = Some(TRIGGER); // signal pending input with a trigger
    let (tx, rx) = code.run_in_thread();
    #[cfg(debug_assertions)]
    let mut screen = Screen::<45, 26>::new();
    let mut score = 0;
    let mut paddle_x: i64 = 0;
    let mut ball_x = 0;
    while let Ok(x) = rx.recv() {
        if x == TRIGGER {
            #[cfg(debug_assertions)]
            println!("{}", screen);
            tx.send((ball_x - paddle_x).signum()).unwrap();
        } else if let (Ok(y), Ok(z)) = (rx.recv(), rx.recv()) {
            if x == -1 {
                assert_eq!(y, 0);
                score = z;
            } else if z == 3 {
                paddle_x = x;
            } else if z == 4 {
                ball_x = x;
            }
            #[cfg(debug_assertions)]
            screen.update(x, y, z);
        } else {
            panic!("failed to receive enough values!");
        }
    }
    score
}

fn main() {
    let code = Intcode::load("input/day13.in").unwrap();
    println!("Q1: {}", q1(code.clone()));
    println!("Q2: {}", q2(code));
}
