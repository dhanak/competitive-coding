use super::ProgramError;

#[derive(Debug, PartialEq)]
pub enum AddressMode {
    Position,
    Immediate,
    Relative(i64),
}

impl AddressMode {
    pub fn new(
        mode: i64,
        bit: u32,
        base: i64,
    ) -> Result<AddressMode, ProgramError> {
        use AddressMode::*;
        match mode / 10_i64.pow(bit + 2) % 10 {
            0 => Ok(Position),
            1 => Ok(Immediate),
            2 => Ok(Relative(base)),
            _ => Err(ProgramError::InvalidMode(mode)),
        }
    }

    pub fn read(self, mem: &[i64], addr: i64) -> i64 {
        use AddressMode::*;
        match self {
            Position => *mem.get(addr as usize).unwrap_or(&0),
            Immediate => addr,
            Relative(base) => *mem.get((base + addr) as usize).unwrap_or(&0),
        }
    }

    pub fn write(
        self,
        mem: &mut Vec<i64>,
        addr: i64,
        value: i64,
    ) -> Result<(), ProgramError> {
        use AddressMode::*;
        let addr = match self {
            Position => addr,
            Immediate => return Err(ProgramError::InvalidMode(1)),
            Relative(base) => base + addr,
        } as usize;
        mem.resize(mem.len().max(addr + 1), 0);
        mem[addr] = value;
        Ok(())
    }
}
