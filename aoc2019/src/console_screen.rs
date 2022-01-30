use core::fmt;

pub struct Screen<const W: usize, const H: usize> {
    pub lines: [String; H],
    pub score: i64,
}

impl<const W: usize, const H: usize> fmt::Display for Screen<W, H> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "Score: {}\n", self.score)?;
        for line in self.lines.iter() {
            writeln!(f, "{}", line)?;
        }
        Ok(())
    }
}

impl<const W: usize, const H: usize> Screen<W, H> {
    pub fn new() -> Screen<W, H> {
        Screen {
            lines: [0; H].map(|_| " ".repeat(W)),
            score: 0,
        }
    }

    pub fn update(&mut self, x: i64, y: i64, tile_id: i64) {
        if x == -1 && y == 0 {
            self.score = tile_id;
        } else {
            let tile = match tile_id {
                0 => " ",
                1 => "#",
                2 => "*",
                3 => "-",
                4 => "O",
                _ => panic!("invalid tile_id {}", tile_id),
            };
            let (x, y) = (x as usize, y as usize);
            self.lines[y].replace_range(x..=x, &tile);
        }
    }
}
