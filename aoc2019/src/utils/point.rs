use std::ops::{Add, Mul};

#[derive(Clone, Copy, Debug, Default, Eq, Hash, PartialEq)]
pub struct Point<T> {
    pub x: T,
    pub y: T,
}

impl<T: Copy + Add<Output = T>> Add for Point<T> {
    type Output = Point<T>;

    fn add(self, rhs: Self) -> Self::Output {
        Point {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
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
        Point {
            x: self.x * scalar,
            y: self.y * scalar,
        }
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

        let r: Point<u8> = Point { x: 1, y: 2 };
        assert_eq!(p + r, r);

        assert_eq!(Point { x: 1, y: 2 } * 2, Point { x: 2, y: 4 });
    }
}
