#[derive(Clone)]
struct Cell {
    uncovered: bool,
    flagged: bool,
    bomb: bool,
    adjacent_bombs: u32,
}

impl Cell {
    fn to_str(&self) -> String {
        format!("uncovered: {}, bomb: {}, adjacent: {}, flagged: {}", self.uncovered, self.bomb, self.adjacent_bombs, self.flagged)
    }
}

pub struct Minefield {
    rows: u32,
    cols: u32,
    bombs: u32,
    cells: Vec<Vec<Cell>>,
}

impl Minefield {
    pub fn new(rows: u32, cols: u32, bombs: u32) -> Result<Minefield, &'static str> {
        if bombs > rows * cols {
            return Err("More bombs than cells");
        }

        let mut minefield = Minefield {
            rows: rows,
            cols: cols,
            bombs: bombs,
            cells: Vec::new(),
        };

        for _row in 0..rows {
            minefield.cells.push(vec![Cell{ uncovered: false, flagged: false, bomb: false, adjacent_bombs: 0 }; cols as usize])
        }

        Ok(minefield)
    }

    fn get_cell<'a>(&'a mut self, row: u32, col: u32) -> Option<&'a mut Cell> {
        if row > self.rows || col > self.cols {
            None
        }
        else {
            let row = &mut self.cells[row as usize];
            let cell = &mut row[col as usize];
            Some(cell)
            //return Some<&mut row[col as usize]>;
            //Some<self.cells[row as usize][col as usize]>
        }
    }

    pub fn toggle_flag(&mut self, row: u32, col: u32) {
        match self.get_cell(row, col) {
            Some(cell) => cell.flagged = !cell.flagged,
            _ => ()
        }
        //let &mut cell = self.get_cell(row, col).unwrap_or_el
        //if (row < self.rows && col < self.cols) {
        //    let row = &mut self.cells[row as usize];
        //    let mut cell = &mut row[col as usize];
        //    cell.flagged = !cell.flagged;
        //}
    }

    pub fn uncover_adjacent(&mut self, row: u32, col: u32) {
    }

    pub fn uncover_cell(&mut self, row: u32, col: u32) {
        match self.get_cell(row, col) {
            Some(cell) => cell.uncovered = true,
            _ => ()
        }
        //if (row < self.rows && col < self.cols) {
        //    let row = &mut self.cells[row as usize];
        //    let mut cell = &mut row[col as usize];
        //    cell.uncovered = true;
        //}
    }

    pub fn print(&self) {
        for (x, row) in self.cells.iter().enumerate() {
            for (y, col) in row.iter().enumerate() {
                println!("({}, {}) {}", x, y, col.to_str())
            }
        }
    }
}
