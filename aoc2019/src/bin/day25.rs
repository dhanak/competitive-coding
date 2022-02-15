use std::{io::stdin, thread, time::Duration};

use aoc2019::intcode::{to_ascii, Intcode};
use itertools::Itertools;

fn main() {
    let mut code = Intcode::load("input/day25.in").unwrap();
    if cfg!(debug_assertions) {
        // run interactive quest in debug mode
        code.timeout = Duration::from_secs(1_000_000);
        let (tx, rx) = to_ascii(code.run_in_thread());
        thread::spawn(move || rx.iter().for_each(|line| print!("{line}")));
        let mut line = String::new();
        while stdin().read_line(&mut line).is_ok() && line != "q\n" {
            line = match line.as_str() {
                "n\n" => "north\n",
                "e\n" => "east\n",
                "s\n" => "south\n",
                "w\n" => "west\n",
                other => other,
            }
            .to_string();
            if tx.send(line.split_off(0)).is_err() {
                break;
            }
        }
    } else {
        // run solution in release mode
        let walk = "east;\
                    take antenna;\
                    west;north;\
                    take weather machine;\
                    north;east;\
                    take spool of cat6;\
                    east;south;\
                    take mug;\
                    north;west;south;south;east;";
        let (tx, rx) = to_ascii(code.run_in_thread());
        tx.send(walk.replace(';', "\n")).unwrap();
        let mut out = rx.iter().join("");
        out.replace_range(0..out.find("A loud").unwrap(), "");
        print!("{}", out);
    }
}

/*
                                                 Science Lab
                                                (tambourine)
                                                     |
                Observatory                       Holodeck
                  (cake)                        (molten lava)
                    |                                |
 Navigation ---  Hot Choco  ---   Sick Bay   ---  Corridor
(escape pod)    (electrom.)      (inf. loop)         |
                                     |               |
  Storage   ---  Stables    --- Gift Center       Passages --- Engineering
 (klein b.)    (spool cat6)          |            (shell)           |
    |               |                |                              |
 Hallway        Warp Maint.       Kitchen                         Arcade
(weather m.)        |              (mug)                        (photons)
    |               |
    |           Sec. Check  --- Press. Floor
    |
    |
Hull Breach -- Crew Quarters
    ^           (antenna)

Needed to enter: antenna, weather machine, spool of cat6, mug
*/
