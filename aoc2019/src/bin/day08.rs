use std::fs::read_to_string;

const WIDTH: usize = 25;
const HEIGHT: usize = 6;
const LAYER_SIZE: usize = WIDTH * HEIGHT;

fn q1(layers: &[&[i8]]) -> usize {
    let layer = *layers
        .iter()
        .min_by_key(|&&layer| layer.iter().filter(|&&d| d == 0).count())
        .unwrap();

    layer.iter().filter(|&&d| d == 1).count()
        * layer.iter().filter(|&&d| d == 2).count()
}

fn q2(layers: &[&[i8]]) -> String {
    let image: Vec<i8> = (0..layers[0].len())
        .map(|i| {
            layers
                .iter()
                .fold(2, |p, &layer| if p < 2 { p } else { layer[i] })
        })
        .collect();

    (0..HEIGHT)
        .map(|r| {
            image[r * WIDTH..(r + 1) * WIDTH]
                .iter()
                .map(|d| if *d > 0 { "##" } else { "  " })
                .collect::<String>()
        })
        .collect::<Vec<String>>()
        .join("\n")
}

fn main() {
    let digits: Vec<i8> = read_to_string("input/day08.in")
        .unwrap()
        .trim()
        .chars()
        .map(|c| {
            assert!(c.is_digit(10), "{} is not a digit!", c);
            (c as i8) - ('0' as i8)
        })
        .collect();
    assert_eq!(digits.len() % LAYER_SIZE, 0);

    let layers: Vec<&[i8]> = (0..digits.len())
        .step_by(LAYER_SIZE)
        .map(|i| &digits[i..i + LAYER_SIZE])
        .collect();

    println!("Q1: {}", q1(&layers));
    println!("Q2:\n{}", q2(&layers));
}
