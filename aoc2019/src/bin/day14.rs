use std::collections::HashMap;
use std::error::Error;
use std::fs::read_to_string;

use num::Integer;

type ReactionParts = Vec<(String, i64)>;
type ReactionMap = HashMap<String, (ReactionParts, i64)>;

peg::parser! {
    grammar reaction() for str {
        pub rule map() -> ReactionMap
            = r:(reaction() ** "\n") "\n"* { HashMap::from_iter(r) }

        rule reaction() -> (String, (ReactionParts, i64))
            = parts:(counted_chem() ++ ", ") " => " target:counted_chem() {
                (target.0, (parts, target.1))
            }

        rule counted_chem() -> (String, i64)
            = c:number() " " n:name() { (n, c) }

        rule number() -> i64
            = n:$(['0'..='9']+) {? n.parse().or(Err("number")) }

        rule name() -> String
            = n:$(['A'..='Z']+) { String::from(n) }
    }
}

fn balance_reactions<'a>(
    reactions: &'a ReactionMap,
    chemicals: &mut HashMap<&'a str, (i64, i64)>,
) {
    while let Some((&chem, (need, prod))) =
        chemicals.iter_mut().find(|(_, (need, prod))| need > prod)
    {
        let (parts, count) = &reactions[chem];
        let mul = (*need - *prod).div_ceil(count);
        *prod += mul * count;
        for (part, count) in parts {
            chemicals.entry(part).or_default().0 += mul * count;
        }
    }
}

fn q1(reactions: &ReactionMap) -> i64 {
    let mut chemicals =
        HashMap::from([("ORE", (0, i64::MAX)), ("FUEL", (1, 0))]);
    balance_reactions(reactions, &mut chemicals);
    chemicals["ORE"].0
}

fn q2(reactions: &ReactionMap) -> i64 {
    let mut chemicals =
        HashMap::from([("ORE", (0, i64::MAX)), ("FUEL", (1, 0))]);
    balance_reactions(reactions, &mut chemicals);
    let ore_per_fuel = chemicals["ORE"].0;
    while 1_000_000_000_000 - chemicals["ORE"].0 >= ore_per_fuel {
        let (mut need, prod) = chemicals["FUEL"];
        need += (1_000_000_000_000 - chemicals["ORE"].0) / ore_per_fuel;
        chemicals.insert("FUEL", (need, prod));
        balance_reactions(reactions, &mut chemicals);
    }
    chemicals["FUEL"].1
}

fn main() -> Result<(), Box<dyn Error>> {
    let reactions = reaction::map(&read_to_string("input/day14.in")?)?;
    println!("Q1: {}", q1(&reactions));
    println!("Q2: {}", q2(&reactions));
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_ex1() {
        let test = "10 ORE => 10 A\n\
                    1 ORE => 1 B\n\
                    7 A, 1 B => 1 C\n\
                    7 A, 1 C => 1 D\n\
                    7 A, 1 D => 1 E\n\
                    7 A, 1 E => 1 FUEL";
        let reactions = reaction::map(test).unwrap();
        assert_eq!(q1(&reactions), 31);
    }

    #[test]
    fn test_ex2() {
        let test = "9 ORE => 2 A\n\
                    8 ORE => 3 B\n\
                    7 ORE => 5 C\n\
                    3 A, 4 B => 1 AB\n\
                    5 B, 7 C => 1 BC\n\
                    4 C, 1 A => 1 CA\n\
                    2 AB, 3 BC, 4 CA => 1 FUEL";
        let reactions = reaction::map(test).unwrap();
        assert_eq!(q1(&reactions), 165);
    }

    #[test]
    fn test_ex3() {
        let test = "157 ORE => 5 NZVS\n\
                    165 ORE => 6 DCFZ\n\
                    44 XJWVT, 5 KHKGT, 1 QDVJ, 29 NZVS, 9 GPVTF, 48 HKGWZ => 1 FUEL\n\
                    12 HKGWZ, 1 GPVTF, 8 PSHF => 9 QDVJ\n\
                    179 ORE => 7 PSHF\n\
                    177 ORE => 5 HKGWZ\n\
                    7 DCFZ, 7 PSHF => 2 XJWVT\n\
                    165 ORE => 2 GPVTF\n\
                    3 DCFZ, 7 NZVS, 5 HKGWZ, 10 PSHF => 8 KHKGT";
        let reactions = reaction::map(test).unwrap();
        assert_eq!(q1(&reactions), 13312);
        assert_eq!(q2(&reactions), 82892753);
    }

    #[test]
    fn test_ex4() {
        let test = "2 VPVL, 7 FWMGM, 2 CXFTF, 11 MNCFX => 1 STKFG\n\
                    17 NVRVD, 3 JNWZP => 8 VPVL\n\
                    53 STKFG, 6 MNCFX, 46 VJHF, 81 HVMC, 68 CXFTF, 25 GNMV => 1 FUEL\n\
                    22 VJHF, 37 MNCFX => 5 FWMGM\n\
                    139 ORE => 4 NVRVD\n\
                    144 ORE => 7 JNWZP\n\
                    5 MNCFX, 7 RFSQX, 2 FWMGM, 2 VPVL, 19 CXFTF => 3 HVMC\n\
                    5 VJHF, 7 MNCFX, 9 VPVL, 37 CXFTF => 6 GNMV\n\
                    145 ORE => 6 MNCFX\n\
                    1 NVRVD => 8 CXFTF\n\
                    1 VJHF, 6 MNCFX => 4 RFSQX\n\
                    176 ORE => 6 VJHF";
        let reactions = reaction::map(test).unwrap();
        assert_eq!(q1(&reactions), 180697);
        assert_eq!(q2(&reactions), 5586022);
    }

    #[test]
    fn test_ex5() {
        let test = "171 ORE => 8 CNZTR\n\
                    7 ZLQW, 3 BMBT, 9 XCVML, 26 XMNCP, 1 WPTQ, 2 MZWV, 1 RJRHP => 4 PLWSL\n\
                    114 ORE => 4 BHXH\n\
                    14 VRPVC => 6 BMBT\n\
                    6 BHXH, 18 KTJDG, 12 WPTQ, 7 PLWSL, 31 FHTLT, 37 ZDVW => 1 FUEL\n\
                    6 WPTQ, 2 BMBT, 8 ZLQW, 18 KTJDG, 1 XMNCP, 6 MZWV, 1 RJRHP => 6 FHTLT\n\
                    15 XDBXC, 2 LTCX, 1 VRPVC => 6 ZLQW\n\
                    13 WPTQ, 10 LTCX, 3 RJRHP, 14 XMNCP, 2 MZWV, 1 ZLQW => 1 ZDVW\n\
                    5 BMBT => 4 WPTQ\n\
                    189 ORE => 9 KTJDG\n\
                    1 MZWV, 17 XDBXC, 3 XCVML => 2 XMNCP\n\
                    12 VRPVC, 27 CNZTR => 2 XDBXC\n\
                    15 KTJDG, 12 BHXH => 5 XCVML\n\
                    3 BHXH, 2 VRPVC => 7 MZWV\n\
                    121 ORE => 7 VRPVC\n\
                    7 XCVML => 6 RJRHP\n\
                    5 BHXH, 4 VRPVC => 5 LTCX";
        let reactions = reaction::map(test).unwrap();
        assert_eq!(q1(&reactions), 2210736);
        assert_eq!(q2(&reactions), 460664);
    }
}
