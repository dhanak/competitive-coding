pub mod program_error;
use std::error::Error;
use std::fs::read_to_string;
use std::iter::from_fn;
use std::ops::{Index, IndexMut};
use std::slice::SliceIndex;
use std::sync::mpsc::{channel, Receiver, Sender};
use std::thread;
use std::time::Duration;

pub use program_error::ProgramError;

mod address_mode;
use address_mode::AddressMode;

#[cfg(test)]
mod tests;

const TIMEOUT: Duration = Duration::from_secs(5);

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct Intcode {
    mem: Vec<i64>,
    pub trigger: Option<i64>,
}

impl<T> From<&[T]> for Intcode
where
    T: Copy + Into<i64>,
{
    fn from(source: &[T]) -> Intcode {
        Intcode {
            mem: source.iter().map(|v| (*v).into()).collect(),
            trigger: None,
        }
    }
}

impl<T, const N: usize> From<[T; N]> for Intcode
where
    T: Copy + Into<i64>,
{
    fn from(source: [T; N]) -> Intcode {
        Intcode::from(source.as_slice())
    }
}

impl<Idx> Index<Idx> for Intcode
where
    Idx: SliceIndex<[i64]>,
{
    type Output = Idx::Output;

    fn index(&self, index: Idx) -> &Self::Output {
        &self.mem[index]
    }
}

impl<Idx> IndexMut<Idx> for Intcode
where
    Idx: SliceIndex<[i64]>,
{
    fn index_mut(&mut self, index: Idx) -> &mut Self::Output {
        &mut self.mem[index]
    }
}

impl Intcode {
    pub fn load(path: &str) -> Result<Intcode, Box<dyn Error>> {
        let input = read_to_string(path)?;
        let mem = input
            .trim()
            .split(',')
            .map(|v| v.parse::<i64>())
            .collect::<Result<Vec<i64>, _>>()?;
        Ok(Intcode { mem, trigger: None })
    }

    pub fn run(
        &mut self,
        rx: Receiver<i64>,
        tx: Sender<i64>,
    ) -> Result<(), Box<dyn Error>> {
        let mem = &mut self.mem;
        let mut ip = 0; // instruction pointer
        let mut rb = 0; // relative base
        while ip < mem.len() {
            let opc = mem[ip] % 100;
            let ma = AddressMode::new(mem[ip], 0, rb)?;
            let mb = AddressMode::new(mem[ip], 1, rb)?;
            let mc = AddressMode::new(mem[ip], 2, rb)?;
            match (opc, &mem[ip + 1..]) {
                (1, &[a, b, c, ..]) => {
                    // add
                    let a = ma.read(mem, a);
                    let b = mb.read(mem, b);
                    mc.write(mem, c, a + b)?;
                    ip += 4;
                }
                (2, &[a, b, c, ..]) => {
                    // mul
                    let a = ma.read(mem, a);
                    let b = mb.read(mem, b);
                    mc.write(mem, c, a * b)?;
                    ip += 4;
                }
                (3, &[a, ..]) => {
                    // inp
                    if let Some(v) = self.trigger {
                        tx.send(v)?;
                    }
                    ma.write(mem, a, rx.recv_timeout(TIMEOUT)?)?;
                    ip += 2;
                }
                (4, &[a, ..]) => {
                    // out
                    tx.send(ma.read(mem, a))?;
                    ip += 2;
                }
                (5 | 6, &[a, b, ..]) => {
                    // jump if true | false
                    let a = ma.read(mem, a);
                    if (opc == 5 && a != 0) || (opc == 6 && a == 0) {
                        ip = mb.read(mem, b) as usize;
                    } else {
                        ip += 3;
                    }
                }
                (7, &[a, b, c, ..]) => {
                    // less than
                    let a = ma.read(mem, a);
                    let b = mb.read(mem, b);
                    mc.write(mem, c, (a < b) as i64)?;
                    ip += 4;
                }
                (8, &[a, b, c, ..]) => {
                    // equal
                    let a = ma.read(mem, a);
                    let b = mb.read(mem, b);
                    mc.write(mem, c, (a == b) as i64)?;
                    ip += 4;
                }
                (9, &[a, ..]) => {
                    // adjust relative base
                    rb += ma.read(mem, a);
                    ip += 2;
                }
                (1..=9, _) => return Err(Box::new(ProgramError::BadLen)),
                (99, _) => return Ok(()),
                _ => return Err(Box::new(ProgramError::InvalidOpCode(opc))),
            };
        }
        Err(Box::new(ProgramError::DoesNotTerminate))
    }

    pub fn run_with(
        &mut self,
        input: &[i64],
    ) -> Result<Vec<i64>, Box<dyn Error>> {
        let ((tx, rxt), (txt, rx)) = (channel(), channel());
        input.iter().for_each(|&code| tx.send(code).unwrap());
        self.run(rxt, txt)?;
        Ok(from_fn(|| rx.recv_timeout(TIMEOUT).ok()).collect())
    }

    pub fn run_in_thread(mut self) -> (Sender<i64>, Receiver<i64>) {
        let ((tx, rxt), (txt, rx)) = (channel(), channel());
        thread::spawn(move || self.run(rxt, txt).unwrap());
        (tx, rx)
    }
}
