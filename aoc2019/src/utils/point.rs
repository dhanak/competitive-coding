use std::ops::{Add, Mul, Neg, Sub};

#[derive(Clone, Copy, Debug, Default, Eq, Hash, PartialEq)]
pub struct Point<T> {
    pub x: T,
    pub y: T,
}

impl<T> Point<T> {
    pub fn new(x: T, y: T) -> Self {
        Point { x, y }
    }

    pub fn transpose(self) -> Self {
        Point::new(self.y, self.x)
    }
}

impl<T> Point<T>
where
    T: Neg<Output = T>,
{
    pub fn r90(self) -> Self {
        Point::new(-self.y, self.x)
    }

    pub fn r270(self) -> Self {
        Point::new(self.y, -self.x)
    }
}

impl<T> Point<T>
where
    T: Into<f64>,
{
    pub fn len(self) -> f64 {
        self.x.into().hypot(self.y.into())
    }

    pub fn gradient(self) -> f64 {
        self.y.into().atan2(self.x.into())
    }
}

impl<T> Neg for Point<T>
where
    T: Neg<Output = T>,
{
    type Output = Point<T>;

    fn neg(self) -> Self::Output {
        Point::new(-self.x, -self.y)
    }
}

impl<T> Add for Point<T>
where
    T: Copy + Add<Output = T>,
{
    type Output = Point<T>;

    fn add(self, rhs: Self) -> Self::Output {
        Point::new(self.x + rhs.x, self.y + rhs.y)
    }
}

impl<T> Sub for Point<T>
where
    T: Copy + Sub<Output = T>,
{
    type Output = Point<T>;

    fn sub(self, rhs: Self) -> Self::Output {
        Point::new(self.x - rhs.x, self.y - rhs.y)
    }
}

impl<T, S> Mul<S> for Point<T>
where
    T: Copy + Mul<S, Output = T>,
    S: Copy,
{
    type Output = Point<T>;

    fn mul(self, scalar: S) -> Self::Output {
        Point::new(self.x * scalar, self.y * scalar)
    }
}

#[cfg(test)]
mod tests {
    use std::f64::consts::PI;

    use super::*;

    #[test]
    fn test_point() {
        let p: Point<u8> = Point::default();
        assert_eq!(p.x, 0);
        assert_eq!(p.y, 0);
        assert_eq!(p.len(), 0.0);

        let p: Point<i32> = Point::default();
        assert_eq!(p.x, 0);
        assert_eq!(p.y, 0);
        assert_eq!(p.len(), 0.0);

        let r: Point<i32> = Point::new(3, 4);
        assert_eq!(r.x, 3);
        assert_eq!(r.y, 4);
        assert_eq!(r.transpose().x, 4);
        assert_eq!(r.transpose().y, 3);
        assert_eq!(r.len(), 5.0);
        assert_eq!(r.r90().x, -4);
        assert_eq!(r.r90().y, 3);
        assert_eq!(r.r270().x, 4);
        assert_eq!(r.r270().y, -3);
        assert_eq!(p + r, r);
        assert_eq!(r - p, r);

        assert_eq!(Point::new(1, 2) + Point::new(3, 4), Point::new(4, 6));
        assert_eq!(Point::new(3, 4) - Point::new(1, 2), Point::new(2, 2));
        assert_eq!(Point::new(1, 2) * 2, Point::new(2, 4));

        assert_eq!(Point::new(1, 0).gradient(), 0.0);
        assert_eq!(Point::new(2, 2).gradient(), PI / 4.0);
        assert_eq!(Point::new(0, 3).gradient(), PI / 2.0);
        assert_eq!(Point::new(-4, 4).gradient(), 3.0 * PI / 4.0);
        assert_eq!(Point::new(-5, 0).gradient(), PI);
        assert_eq!(Point::new(-6, -6).gradient(), -3.0 * PI / 4.0);
        assert_eq!(Point::new(0, -7).gradient(), -PI / 2.0);
        assert_eq!(Point::new(8, -8).gradient(), -PI / 4.0);
    }
}
