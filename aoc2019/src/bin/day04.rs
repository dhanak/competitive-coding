const RANGE: (&[i64], &[i64]) = (&[2, 5, 4, 0, 3, 2], &[7, 8, 9, 8, 6, 0]);

fn count_if<F>((min, max): (&[i64], &[i64]), f: F) -> usize
where
    F: Fn(&[i64]) -> bool,
{
    let mut n = min.to_owned();
    let mut count = 0;
    let mut i = n.len();
    loop {
        if i == n.len() {
            if (1..n.len()).all(|j| n[j - 1] <= n[j]) && f(&n[..]) {
                count += 1;
            }
            i -= 1;
        } else {
            let v = n[i];
            n[i] = v + 1;
            n[i + 1..].iter_mut().for_each(|d| *d = v);
            if n[i] <= 9 && n[..] <= *max {
                i += 1;
            } else if i > 0 {
                i -= 1;
            } else {
                break;
            }
        }
    }
    count
}

fn q1() -> usize {
    count_if(RANGE, |n| (1..n.len()).any(|i| n[i - 1] == n[i]))
}

fn q2() -> usize {
    count_if(RANGE, |n| {
        (1..n.len()).any(|i| {
            n[i - 1] == n[i]
                && (i == 1 || n[i - 2] < n[i - 1])
                && (i + 1 == n.len() || n[i] < n[i + 1])
        })
    })
}

fn main() {
    println!("Q1: {}", q1());
    println!("Q2: {}", q2());
}
