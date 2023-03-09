mod minefield;

use minefield::Minefield;
use std::io;

fn main() {
    let mut minefield = Minefield::new(10, 10, 10).unwrap();
    minefield.no_kaboom(0, 5);
    minefield.print();

    let stdin = io::stdin();
    loop {
    }
}
