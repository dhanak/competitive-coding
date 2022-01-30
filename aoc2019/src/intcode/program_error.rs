use std::error::Error;
use std::fmt::Display;

#[derive(Debug, PartialEq)]
pub enum ProgramError {
    BadLen,
    DoesNotTerminate,
    InvalidOpCode(i64),
    InvalidMode(i64),
    InvalidColor(i64),
}

impl Display for ProgramError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl Error for ProgramError {}
