mod minefield;

use minefield::Minefield;

fn main() {
    println!("Hello, world!");

    let mut minefield = Minefield::new(10, 10, 10).unwrap();
    minefield.no_kaboom(0, 5);
    minefield.print();

}
