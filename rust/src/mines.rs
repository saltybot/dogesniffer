#[derive(Clone)]
struct Cell {
    mine: bool,
    flag: bool,
    uncovered: bool,
}

pub struct Minefield {
    width: usize,
    height: usize,
    adjacent_matrix: Vec<usize>,
    cells: Vec<Cell>,
}

impl Minefield {
    pub fn new(width: usize, height: usize, mines: usize) -> Option<Minefield> {
        if mines > width * height {
            return None;
        }

        let mut minefield = Minefield {
            width: width,
            height: height,
            adjacent_matrix: vec![0; width * height],
            cells: vec![Cell { mine: false, flag: false, uncovered: false }; width * height],
        };
        populate_adjacent(&mut minefield);
        Some(minefield)
    }
}

fn populate_adjacent(minefield: &mut Minefield) {
    let mut i = 0;
    for y in 0..minefield.height {
        for x in 0..minefield.width {
            println!("X={} Y={}", x, y);
            let mut adjacent_mines = 0;
            // check top 3
            if y != 0 {
                // top left corner
                if x != 0 {
                    let n = i - minefield.width - 1;
                    if minefield.cells[n].mine {
                        adjacent_mines += 1;
                    }
                }
                // top right corner
                else if x != minefield.width - 1 {
                    let n = i - minefield.width + 1;
                    if minefield.cells[n].mine {
                        adjacent_mines += 1;
                    }
                }
                // top
                let n = i - minefield.width;
                if minefield.cells[n].mine {
                    adjacent_mines += 1;
                }
            }
            // check bottom 3
            if y != minefield.height - 1 {
                // bottom left corner
                if x != 0 {
                    let n = i + minefield.width - 1;
                    if minefield.cells[n].mine {
                        adjacent_mines += 1;
                    }
                }
                // bottom right corner
                else if x != minefield.width - 1 {
                    let n = i + minefield.width + 1;
                    if minefield.cells[n].mine {
                        adjacent_mines += 1;
                    }
                }
                // bottom
                let n = i + minefield.width;
                if minefield.cells[n].mine {
                    adjacent_mines += 1;
                }
            }
            // check left
            if x != 0 {
                let n = i - 1;
                if minefield.cells[n].mine {
                    adjacent_mines += 1;
                }
            }
            // check right
            if x != minefield.width - 1 {
                let n = i + 1;
                if minefield.cells[n].mine {
                    adjacent_mines += 1;
                }
            }
            minefield.adjacent_matrix[i] = adjacent_mines;
            i += 1;
        }
    }
    //let i = 0;
    //let x, y = 0;
    //for mine in &minefield.mines {
        // top row

        // middle rows
        // bottom row

    //}
    //for i, mine in mines {
        // check left
    //    if i == 0 {

    //    }
    //    else if i ==

    //}
}