pub mod utils {
    use std::fs::File;
    use std::io::{BufRead, BufReader, Lines};
    use std::ops::{Add, Mul};
    use std::path::Path;

    // Returns an Iterator to the Reader of the lines of the file.
    pub fn read_lines<P>(filename: P) -> Lines<BufReader<File>>
    where
        P: AsRef<Path>,
    {
        let file = File::open(filename).unwrap();
        BufReader::new(file).lines()
    }

    #[derive(Clone, Debug, Eq, Hash, PartialEq)]
    pub struct Point<T> {
        pub x: T,
        pub y: T,
    }

    impl<T: Copy> Copy for Point<T> {}

    impl<T: Default> Default for Point<T> {
        fn default() -> Self {
            Point {
                x: T::default(),
                y: T::default(),
            }
        }
    }

    impl<T: Copy + Add<Output = T>> Add for &Point<T> {
        type Output = Point<T>;

        fn add(self, rhs: Self) -> Self::Output {
            Point {
                x: self.x + rhs.x,
                y: self.y + rhs.y,
            }
        }
    }

    impl<T: Copy + Add<Output = T>> Add for Point<T> {
        type Output = Point<T>;

        fn add(self, rhs: Self) -> Self::Output {
            &self + &rhs
        }
    }

    impl<T, S> Mul<S> for &Point<T>
    where
        T: Copy + Mul<S, Output = T>,
        S: Copy,
    {
        type Output = Point<T>;

        fn mul(self, scalar: S) -> Self::Output {
            Point {
                x: self.x * scalar,
                y: self.y * scalar,
            }
        }
    }

    impl<T, S> Mul<S> for Point<T>
    where
        T: Copy + Mul<S, Output = T>,
        S: Copy,
    {
        type Output = Point<T>;

        fn mul(self, scalar: S) -> Self::Output {
            &self * scalar
        }
    }

    #[cfg(test)]
    mod tests {
        use super::*;

        #[test]
        fn test_point() {
            let p: Point<i32> = Point::default();
            assert_eq!(p.x, 0);
            assert_eq!(p.y, 0);

            let p: Point<u8> = Point::default();
            assert_eq!(p.x, 0);
            assert_eq!(p.y, 0);

            assert_eq!(
                Point { x: 1, y: 2 } + Point { x: 3, y: 4 },
                Point { x: 4, y: 6 }
            );

            assert_eq!(Point { x: 1, y: 2 } * 2, Point { x: 2, y: 4 });

            let r: Point<u8> = Point { x: 1, y: 2 };
            assert_eq!(p + r, r);
        }
    }
}
