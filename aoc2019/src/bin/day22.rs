use std::fs::read_to_string;

use mod_exp::mod_exp;
use modinverse::modinverse as mod_inverse;

#[derive(Debug, Clone, Hash, PartialEq, PartialOrd, Eq, Ord)]
pub enum Shuffle {
    Stack,
    Cut(i128),
    Deal(i128),
}

peg::parser! {
    grammar shuffles() for str {
        pub rule list() -> Vec<Shuffle>
            = s:shuffle() ** "\n" "\n"* { s }

        rule shuffle() -> Shuffle
            = stack() / cut() / deal();

        rule stack() -> Shuffle
            = "deal into new stack" { Shuffle::Stack }

        rule cut() -> Shuffle
            = "cut " n:number() { Shuffle::Cut(n) }

        rule deal() -> Shuffle
            = "deal with increment " n:number() { Shuffle::Deal(n) }

        rule number() -> i128
            = n:$("-"? ['0'..='9']+) {? n.parse().or(Err("number")) }
    }
}

struct Linear {
    pub a: i128,
    pub b: i128,
    pub m: i128,
}

impl Linear {
    pub fn apply(&self, i: i128) -> i128 {
        (self.a * i + self.b) % self.m
    }

    pub fn inverse(&self) -> Linear {
        // j = a * i + b -> i = (j - b) / a = a^-1 * j - b * a^-1
        let a_inv = mod_inverse(self.a, self.m).unwrap();
        Linear {
            a: a_inv,
            b: self.m - self.b * a_inv % self.m,
            m: self.m,
        }
    }

    pub fn exp(&self, exponent: i128) -> Linear {
        // apply (a * i + b) r times
        // a^r * i + (a^(r-1) + a^(r-2) + ... + 1) * b
        // a^(r-1) + a^(r-2) + ... + 1 = (a^r - 1) / (a - 1)
        let a_r = mod_exp(self.a, exponent, self.m);
        let a1_inv = mod_inverse(self.a - 1, self.m).unwrap();
        Linear {
            a: a_r,
            b: (a_r - 1) * a1_inv % self.m * self.b % self.m,
            m: self.m,
        }
    }
}

/// Given an index `i`, return coefficients `(a, b)`, such that the
/// `shuffle` moves card `i` to `a * i + b`.
fn reduce(shuffle: &[Shuffle], m: i128) -> Linear {
    let (a, b) = shuffle.iter().fold((1, 0), |(a, b), s| match *s {
        Shuffle::Stack => (m - a, m - b - 1),
        Shuffle::Cut(n) => {
            let n = if n < 0 { m + n } else { n };
            (a, (b + m - n) % m)
        }
        Shuffle::Deal(n) => (a * n % m, b * n % m),
    });
    Linear { a, b, m }
}

fn q1(shuffle: &[Shuffle]) -> i128 {
    let shuffle = reduce(shuffle, 10007);
    shuffle.apply(2019)
}

fn q2(shuffle: &[Shuffle]) -> i128 {
    let shuffle = reduce(shuffle, 119315717514047)
        .inverse()
        .exp(101741582076661);
    shuffle.apply(2020)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn shuffle_deck(input: &str) -> Vec<i128> {
        let shuffle = reduce(&shuffles::list(input).unwrap(), 10).inverse();
        (0..shuffle.m).map(|i| shuffle.apply(i)).collect()
    }

    #[test]
    fn test_shuffle() {
        assert_eq!(
            shuffle_deck(
                "deal with increment 7\n\
                 deal into new stack\n\
                 deal into new stack"
            ),
            [0, 3, 6, 9, 2, 5, 8, 1, 4, 7]
        );
        assert_eq!(
            shuffle_deck(
                "cut 6\n\
                 deal with increment 7\n\
                 deal into new stack"
            ),
            [3, 0, 7, 4, 1, 8, 5, 2, 9, 6]
        );
        assert_eq!(
            shuffle_deck(
                "deal with increment 7\n\
                 deal with increment 9\n\
                 cut -2"
            ),
            [6, 3, 0, 7, 4, 1, 8, 5, 2, 9]
        );
        assert_eq!(
            shuffle_deck(
                "deal into new stack\n\
                 cut -2\n\
                 deal with increment 7\n\
                 cut 8\n\
                 cut -4\n\
                 deal with increment 7\n\
                 cut 3\n\
                 deal with increment 9\n\
                 deal with increment 3\n\
                 cut -1"
            ),
            [9, 2, 5, 8, 1, 4, 7, 0, 3, 6]
        );
    }
}

fn main() {
    let input = read_to_string("input/day22.in").unwrap();
    let shuffle: Vec<Shuffle> = shuffles::list(&input).unwrap();
    println!("Q1: {}", q1(&shuffle));
    println!("Q2: {}", q2(&shuffle));
}
