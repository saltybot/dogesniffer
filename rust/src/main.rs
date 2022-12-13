mod minefield;

use minefield::Minefield;

fn main() {
    println!("Hello, world!");

    let minefield = Minefield::new(10, 10, 10).unwrap();
    minefield.print();
}
