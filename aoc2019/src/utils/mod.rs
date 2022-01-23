use std::f64::consts::PI;
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

pub trait Angle {
    fn to_0_2pi(self) -> Self;
    fn to_npi_pi(self) -> Self;
}

impl Angle for f64 {
    fn to_0_2pi(self) -> Self {
        assert!(-PI < self && self <= PI);
        if self < 0.0 {
            self + 2.0 * PI
        } else {
            self
        }
    }

    fn to_npi_pi(self) -> Self {
        assert!((0.0..2.0 * PI).contains(&self));
        if self > PI {
            self - 2.0 * PI
        } else {
            self
        }
    }
}

mod point;
pub use point::Point;
