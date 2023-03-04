use rand::seq::SliceRandom;
use rand::thread_rng;

//use std::cell;

enum CellState {
    Covered,
    Flagged,
    Uncovered,
    Kablamo
}

#[derive(Clone)]
struct Cell {
    uncovered: bool,
    flagged: bool,
    mine: bool,
    //adjacent_mines: u32,
}

impl Cell {
    fn to_str(&self) -> String {
        format!("uncovered: {}, mine: {}, flagged: {}", self.uncovered, self.mine, self.flagged)
    }
}

pub struct Minefield {
    rows: u32,
    cols: u32,
    mines: u32,
    adjacent_matrix: Vec<usize>,
    cells: Vec<Cell>,
}

impl Minefield {
    pub fn new(rows: u32, cols: u32, mines: u32) -> Result<Minefield, &'static str> {
        if mines > rows * cols {
            return Err("More mines than cells");
        }

        let mut minefield = Minefield {
            rows: rows,
            cols: cols,
            mines: mines,
            adjacent_matrix: vec![0; (rows * cols) as usize], //Vec::new(),
            cells: vec![Cell { uncovered: false, flagged: false, mine: false }; (rows * cols).try_into().unwrap()], //Vec::new(),
        };

        //for _row in 0..rows {
        //    minefield.cells.push(vec![Cell{ uncovered: false, flagged: false, mine: false, adjacent_mines: 0 }; cols as usize])
        //}

        populate_mines(&mut minefield);

        populate_adjacent(&mut minefield);

        Ok(minefield)
    }

    fn get_cell<'a>(&'a mut self, row: u32, col: u32) -> Option<&'a mut Cell> {
        if row > self.rows || col > self.cols {
            None
        }
        else {
            let index: usize = (row * self.cols + col).try_into().unwrap();
            Some(&mut self.cells[index])
        }
    }

    pub fn toggle_flag(&mut self, row: u32, col: u32) {
        match self.get_cell(row, col) {
            Some(cell) => cell.flagged = !cell.flagged,
            _ => ()
        }
    }

    pub fn uncover_adjacent(&mut self, row: u32, col: u32) {
    }

    pub fn uncover_cell(&mut self, row: u32, col: u32) {
        match self.get_cell(row, col) {
            Some(cell) => cell.uncovered = true,
            _ => ()
        }
    }

    pub fn no_kaboom(&mut self, row: u32, col: u32) {
        // don't blow up the first uncovered cell
        let index: usize = self.coordToIndex(row, col);
        if self.cells[index].mine {
            println!("NO KABOOM at ({}, {})", col, row);
            if let Some(first_open) = self.cells.iter_mut().find(|cell| !cell.mine) {
                first_open.mine = true;
            }
            self.cells[index].mine = false;

            populate_adjacent(self)
        }

        self.uncover_cell(row, col)
    }

    pub fn print(&self) {
        let mut index = 0;
        for y in 0..self.rows {
            for x in 0..self.cols {
                let cell = &self.cells[index];
                if cell.flagged {
                    print!("🚩");
                }
                else if !cell.uncovered {
                    if cell.mine {
                        print!("💣");
                    }
                    else {
                        match self.adjacent_matrix[index] {
                            0 => print!("0️⃣"),
                            1 => print!("1️⃣"),
                            2 => print!("2️⃣"),
                            3 => print!("3️⃣"),
                            _ => print!("💀"),
                        }
                        print!("{}", self.adjacent_matrix[index]);
                    }
                }
                else {
                    print!("⬛");
                }
                //println!("({}, {}) adjacent={} {}", x, y, self.adjacent_matrix[index], self.cells[index].to_str());
                index += 1;
            }
            println!("");
        }
        //for (x, row) in self.cells.iter().enumerate() {
        //    for (y, col) in row.iter().enumerate() {
        //        println!("({}, {}) {}", x, y, col.to_str())
        //    }
        //}
    }

    fn coordToIndex(&self, row: u32, col: u32) -> usize {
        (row * self.cols + col).try_into().unwrap()
    }
}

fn populate_mines(minefield: &mut Minefield) {
    let mut cells: Vec<usize> = (0..minefield.cells.len()).collect();
    let mut rng = thread_rng();
    cells.shuffle(&mut rng);
    for i in 0..minefield.mines {
        let index: usize = i.try_into().unwrap();
        minefield.cells[cells[index]].mine = true;
    }


    //let mut mines: Vec<u32> = vec![cells];
    //for i in 0..cells {
    //    mines[i] = i;
    //}

    //let mut foo: Vec<usize> = vec![0; minefield.mines.try_into().unwrap()];
    //for i in 0..foo.len() {
    //    foo[i] = i.as
    //}

    //for i in 0..foo.len() {
        //let r = rand::thread_rng().gen_range(0..foo.len());
        //let r = i;
        //let temp = foo[i];
        //foo[i] = foo[r];
        //foo[r] = temp;
    //}

    //for i in 0..minefield.mines {
    //    let index: usize = i.try_into().unwrap();
    //    minefield.cells[index].mine = true;
    //}
}

fn populate_adjacent(minefield: &mut Minefield) {
    let mut i: usize = 0;
    let rows = minefield.rows as usize;
    let cols = minefield.cols as usize;
    for y in 0..minefield.rows {
        for x in 0..minefield.cols {
            let mut adjacent_mines = 0;
            // check top 3
            if y != 0 {
                // top left corner
                if x != 0 {
                    let n = i - cols - 1;
                    if minefield.cells[n].mine {
                        adjacent_mines += 1;
                    }
                }
                // top right corner
                if x != minefield.cols - 1 {
                    let n = i - rows + 1;
                    if minefield.cells[n].mine {
                        adjacent_mines += 1;
                    }
                }
                // top
                let n = i - cols;
                if minefield.cells[n].mine {
                    adjacent_mines += 1;
                }
            }
            // check bottom 3
            if y != minefield.rows - 1 {
                // bottom left corner
                if x != 0 {
                    let n = i + cols - 1;
                    if minefield.cells[n].mine {
                        adjacent_mines += 1;
                    }
                }
                // bottom right corner
                if x != minefield.cols - 1 {
                    let n = i + cols + 1;
                    if minefield.cells[n].mine {
                        adjacent_mines += 1;
                    }
                }
                // bottom
                let n = i + cols;
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
            if x != (cols - 1).try_into().unwrap() {
            //if x != u32::from(cols).try_into().unwrap() - 1 {
            //if x != cols - 1 {
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
