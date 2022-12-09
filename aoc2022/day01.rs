use std::io::{self, BufRead};

// fn main() {
//     // Read the input from stdin.
//     let stdin = io::stdin();
//     let lines: Vec<String> = stdin.lock().lines().map(|line| line.unwrap()).collect();

//     // Parse the input and compute the result.
//     let result:u64 = lines
//         .split(|line| line.is_empty())
//         .map(|group| group.iter().map(|line| line.parse::<u64>().unwrap()).sum())
//         .max()
//         .unwrap();

//     // Print the result to stdout.
//     println!("{}", result);
// }

fn main() {
    // Read the input from stdin.
    let stdin = io::stdin();
    let lines: Vec<String> = stdin.lock().lines().map(|line| line.unwrap()).collect();

    // Parse the input and compute the result.
    let mut result: Vec<u64> = Vec::new();

    for group in lines.split(|line| line.is_empty()) {
        let total: u64 = group.iter().map(|line| line.parse::<u64>().unwrap()).sum();
        result.push(total);
    }

    result.sort_unstable_by(|a, b| b.cmp(a));

    let result:u64 = result[0..3].iter().sum();

    // Print the result to stdout.
    println!("{}", result);
}
