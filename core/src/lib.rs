use wasm_bindgen::prelude::*;
use serde::Serialize;
use rand::seq::SliceRandom;
use rand::thread_rng;
use std::collections::{HashSet, VecDeque};

#[derive(Clone, PartialEq)]
enum CellState {
    Hidden,
    Revealed,
    Flagged,
}

#[derive(Clone)]
struct Cell {
    has_poop: bool,
    state: CellState,
    adjacent: u8,
}

impl Cell {
    fn blank() -> Self {
        Cell { has_poop: false, state: CellState::Hidden, adjacent: 0 }
    }
}

#[derive(Serialize)]
struct CellView {
    kind: String,
    adjacent: u8,
}

#[wasm_bindgen]
pub struct Game {
    width: usize,
    height: usize,
    num_poops: usize,
    cells: Vec<Cell>,
    initialized: bool,
    game_over: bool,
    won: bool,
    revealed_count: usize,
    flags_placed: usize,
}

#[wasm_bindgen]
impl Game {
    #[wasm_bindgen(constructor)]
    pub fn new(width: u32, height: u32, num_poops: u32) -> Game {
        let size = (width as usize) * (height as usize);
        Game {
            width: width as usize,
            height: height as usize,
            num_poops: num_poops as usize,
            cells: vec![Cell::blank(); size],
            initialized: false,
            game_over: false,
            won: false,
            revealed_count: 0,
            flags_placed: 0,
        }
    }

    fn idx(&self, x: usize, y: usize) -> usize {
        y * self.width + x
    }

    fn neighbors(&self, x: usize, y: usize) -> Vec<(usize, usize)> {
        let mut result = Vec::new();
        for dy in -1i32..=1 {
            for dx in -1i32..=1 {
                if dx == 0 && dy == 0 { continue; }
                let nx = x as i32 + dx;
                let ny = y as i32 + dy;
                if nx >= 0 && nx < self.width as i32 && ny >= 0 && ny < self.height as i32 {
                    result.push((nx as usize, ny as usize));
                }
            }
        }
        result
    }

    fn initialize(&mut self, safe_x: usize, safe_y: usize) {
        // Guarantee first click and all its neighbors are safe
        let safe_idx = self.idx(safe_x, safe_y);
        let safe_zone: HashSet<usize> = std::iter::once(safe_idx)
            .chain(
                self.neighbors(safe_x, safe_y)
                    .into_iter()
                    .map(|(nx, ny)| self.idx(nx, ny)),
            )
            .collect();

        let mut candidates: Vec<usize> = (0..self.cells.len())
            .filter(|i| !safe_zone.contains(i))
            .collect();

        candidates.shuffle(&mut thread_rng());

        let poop_count = self.num_poops.min(candidates.len());
        for &i in &candidates[..poop_count] {
            self.cells[i].has_poop = true;
        }

        // Compute adjacent counts for all cells
        for y in 0..self.height {
            for x in 0..self.width {
                let count = self.neighbors(x, y)
                    .iter()
                    .filter(|&&(nx, ny)| self.cells[self.idx(nx, ny)].has_poop)
                    .count() as u8;
                let i = self.idx(x, y);
                self.cells[i].adjacent = count;
            }
        }

        self.initialized = true;
    }

    pub fn reveal(&mut self, x: u32, y: u32) {
        if self.game_over { return; }
        let x = x as usize;
        let y = y as usize;
        if x >= self.width || y >= self.height { return; }

        if !self.initialized {
            self.initialize(x, y);
        }

        let idx = self.idx(x, y);
        if self.cells[idx].state != CellState::Hidden { return; }

        if self.cells[idx].has_poop {
            self.cells[idx].state = CellState::Revealed;
            self.game_over = true;
            return;
        }

        // BFS flood-fill reveal
        let mut queue: VecDeque<(usize, usize)> = VecDeque::new();
        queue.push_back((x, y));
        while let Some((cx, cy)) = queue.pop_front() {
            let ci = self.idx(cx, cy);
            if self.cells[ci].state != CellState::Hidden { continue; }
            self.cells[ci].state = CellState::Revealed;
            self.revealed_count += 1;
            // Flood through blank (zero-adjacent) cells
            if self.cells[ci].adjacent == 0 {
                for (nx, ny) in self.neighbors(cx, cy) {
                    let ni = self.idx(nx, ny);
                    if self.cells[ni].state == CellState::Hidden && !self.cells[ni].has_poop {
                        queue.push_back((nx, ny));
                    }
                }
            }
        }

        if self.revealed_count >= self.cells.len() - self.num_poops {
            self.won = true;
            self.game_over = true;
        }
    }

    pub fn flag(&mut self, x: u32, y: u32) {
        if self.game_over { return; }
        let x = x as usize;
        let y = y as usize;
        if x >= self.width || y >= self.height { return; }
        let i = self.idx(x, y);
        match self.cells[i].state {
            CellState::Hidden => {
                self.cells[i].state = CellState::Flagged;
                self.flags_placed += 1;
            }
            CellState::Flagged => {
                self.cells[i].state = CellState::Hidden;
                self.flags_placed -= 1;
            }
            CellState::Revealed => {}
        }
    }

    /// Reveal all non-poop cells instantly (cheat win).
    pub fn cheat_win(&mut self) {
        if self.game_over { return; }
        if !self.initialized {
            // Pick a safe cell to initialize from
            self.initialize(0, 0);
        }
        for i in 0..self.cells.len() {
            if !self.cells[i].has_poop && self.cells[i].state == CellState::Hidden {
                self.cells[i].state = CellState::Revealed;
                self.revealed_count += 1;
            }
        }
        self.won = true;
        self.game_over = true;
    }

    /// Chord: reveal all hidden neighbours when flagged-neighbour count matches the cell number.
    pub fn chord(&mut self, x: u32, y: u32) {
        if self.game_over { return; }
        let x = x as usize;
        let y = y as usize;
        if x >= self.width || y >= self.height { return; }
        let i = self.idx(x, y);
        if self.cells[i].state != CellState::Revealed { return; }

        let adj = self.cells[i].adjacent;
        let neighbors = self.neighbors(x, y);
        let flag_count = neighbors
            .iter()
            .filter(|&&(nx, ny)| self.cells[self.idx(nx, ny)].state == CellState::Flagged)
            .count();

        if flag_count == adj as usize {
            let to_reveal: Vec<(usize, usize)> = neighbors
                .into_iter()
                .filter(|&(nx, ny)| {
                    let ni = self.idx(nx, ny);
                    self.cells[ni].state == CellState::Hidden && !self.cells[ni].has_poop
                })
                .collect();
            for (nx, ny) in to_reveal {
                self.reveal(nx as u32, ny as u32);
            }
        }
    }

    /// Returns the board as a 2-D array of CellView objects (row-major: [y][x]).
    pub fn get_board(&self) -> JsValue {
        let mut rows: Vec<Vec<CellView>> = Vec::with_capacity(self.height);
        for y in 0..self.height {
            let mut row = Vec::with_capacity(self.width);
            for x in 0..self.width {
                let i = self.idx(x, y);
                let cell = &self.cells[i];
                let (kind, adjacent) = match &cell.state {
                    CellState::Revealed => {
                        if cell.has_poop { ("hit_poop", 0) } else { ("revealed", cell.adjacent) }
                    }
                    CellState::Flagged => {
                        if self.game_over && !cell.has_poop { ("wrong_flag", 0) } else { ("flagged", 0) }
                    }
                    CellState::Hidden => {
                        if self.game_over && cell.has_poop { ("poop", 0) } else { ("hidden", 0) }
                    }
                };
                row.push(CellView { kind: kind.to_string(), adjacent });
            }
            rows.push(row);
        }
        serde_wasm_bindgen::to_value(&rows).unwrap()
    }

    pub fn is_game_over(&self) -> bool { self.game_over }
    pub fn is_won(&self) -> bool { self.won }

    /// Number of poops minus flags placed (shown in the poop counter, can go negative).
    pub fn flags_remaining(&self) -> i32 {
        self.num_poops as i32 - self.flags_placed as i32
    }

    pub fn width(&self) -> u32 { self.width as u32 }
    pub fn height(&self) -> u32 { self.height as u32 }
}
