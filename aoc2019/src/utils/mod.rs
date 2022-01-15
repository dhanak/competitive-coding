use std::fs::File;
use std::io::{BufRead, BufReader, Lines};
use std::path::Path;

// Returns an Iterator to the Reader of the lines of the file.
pub fn read_lines<P>(filename: P) -> Lines<BufReader<File>>
where
    P: AsRef<Path>,
{
    let file = File::open(filename).unwrap();
    BufReader::new(file).lines()
}

mod point;
pub use point::Point;
