use super::*;

fn test_run(mut prog: Intcode, inp: &[i64], out: &[i64]) {
    assert_eq!(prog.run_with(inp).unwrap(), out);
}

#[test]
fn test_equal() {
    // position mode
    let code = Intcode::from([3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8]);
    test_run(code.clone(), &[8], &[1]);
    test_run(code, &[5], &[0]);

    // immediate mode
    let code = Intcode::from([3, 3, 1108, -1, 8, 3, 4, 3, 99]);
    test_run(code.clone(), &[8], &[1]);
    test_run(code, &[5], &[0]);
}

#[test]
fn test_less_than() {
    // position mode
    let code = Intcode::from([3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8]);
    test_run(code.clone(), &[5], &[1]);
    test_run(code, &[10], &[0]);

    // immediate mode
    let code = Intcode::from([3, 3, 1107, -1, 8, 3, 4, 3, 99]);
    test_run(code.clone(), &[5], &[1]);
    test_run(code, &[10], &[0]);
}

#[test]
fn test_jump() {
    // position mode
    let code = Intcode::from([
        3, 12, 6, 12, 15, 1, 13, 14, 13, 4, 13, 99, -1, 0, 1, 9,
    ]);
    test_run(code.clone(), &[0], &[0]);
    test_run(code, &[5], &[1]);

    // immediate mode
    let code = Intcode::from([3, 3, 1105, -1, 9, 1101, 0, 0, 12, 4, 12, 99, 1]);
    test_run(code.clone(), &[0], &[0]);
    test_run(code, &[5], &[1]);
}

#[test]
fn test_relative() {
    let prog = [
        109, 1, 204, -1, 1001, 100, 1, 100, 1008, 100, 16, 101, 1006, 101, 0,
        99,
    ];
    test_run(Intcode::from(prog), &[], &prog);
}

#[test]
fn test_large() {
    let code = Intcode::from([1102, 34915192, 34915192, 7, 4, 7, 99, 0]);
    test_run(code, &[], &[1219070632396864]); // a 16 digit number

    let code = Intcode::from([104, 1125899906842624_i64, 99]);
    test_run(code, &[], &[1125899906842624]);
}
