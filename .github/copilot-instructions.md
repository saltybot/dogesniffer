# DogeSniff 2 — Copilot Instructions

## Project overview

Minesweeper clone with 💩 replacing mines and a Doge SVG replacing the smiley face. Dual-target: web app and cross-platform desktop app (Tauri). Game logic is written in Rust and compiled to WebAssembly; the same compiled WASM module and the same Vanilla TypeScript UI are used by both targets.

## Repo layout

```
core/                   Rust crate — all game logic (→ compiled to WASM)
  src/lib.rs            Single file; exposes `Game` via wasm-bindgen
  Cargo.toml            crate-type = ["cdylib","rlib"]

frontend/               Vanilla TypeScript + Vite (shared by web + desktop)
  src/main.ts           Entire UI: WASM init, board rendering, event handling
  src/style.css         Classic Win95-style minesweeper look
  src/doge-*.svg        Three SVG doge faces: normal / win / dead
  index.html
  vite.config.ts        Uses vite-plugin-wasm + vite-plugin-top-level-await

desktop/
  package.json          scripts: tauri dev / tauri build
  src-tauri/            Minimal Tauri v2 shell (no custom commands — logic lives in WASM)
    tauri.conf.json     Points frontendDist at ../../frontend/dist
    src/lib.rs + main.rs

Cargo.toml              Workspace: members = ["core", "desktop/src-tauri"]
package.json            Root convenience scripts (build:core, setup, dev:web, etc.)
```

## Build commands

```bash
# Compile Rust core to WASM (must re-run after any change to core/src/lib.rs)
wasm-pack build core --target web

# First-time setup (builds core + installs frontend npm deps)
npm run setup

# Web dev server
npm run dev:web          # http://localhost:5173

# Desktop dev (also starts Vite)
npm run dev:desktop

# Production web build  →  frontend/dist/
npm run build:web

# Production desktop build  →  desktop/src-tauri/target/release/bundle/
npm run build:desktop
```

The frontend imports the core as a local npm package (`"dogesniffer2-core": "file:../core/pkg"`). Run `cd frontend && npm install` once after every `wasm-pack build` to update the symlink if the pkg/ directory was recreated.

## Key conventions

### Rust ↔ TypeScript boundary
- `core/src/lib.rs` exposes exactly one `#[wasm_bindgen]` struct: `Game`.
- `get_board()` returns a `JsValue` (via `serde_wasm_bindgen::to_value`) containing a `CellView[][]` (row-major: `[y][x]`).
- Cell kinds: `"hidden"`, `"revealed"`, `"flagged"`, `"poop"`, `"hit_poop"`, `"wrong_flag"`. The frontend switches on these string literals.
- `adjacent` in `CellView` is non-zero only for `"revealed"` cells.

### Game logic rules (core)
- Board is **not initialized until first click** (`initialized: bool` flag). First click and all its neighbours are guaranteed safe.
- Flood-fill reveal uses BFS through zero-adjacent cells.
- `chord(x, y)` reveals hidden neighbours only when flagged-neighbour count equals `adjacent`.
- On game-over, `get_board()` changes Hidden+poop→`"poop"`, Flagged+no-poop→`"wrong_flag"`.

### Frontend patterns
- All DOM event listeners on the board use **event delegation** (single listener on `#board`, `closest('.cell')` to find target).
- Timer starts on first left-click, stops on game-over. Displayed in `#timer` as 3-digit zero-padded string (capped at 999).
- Poop counter (`#poop-counter`) = `flags_remaining()` from WASM, formatted via `lcdFormat()` (supports negative, capped at −99/999).
- Doge image src is set via `new URL('./doge-${state}.svg', import.meta.url).href` so Vite resolves the asset correctly in both dev and production builds.
- Double-click fires `chord`; right-click fires `flag`; left-click fires `reveal`.

### Random number generation
- WASM target uses `getrandom` with the `js` feature (cfg-gated in `core/Cargo.toml`).
- Native target (desktop/src-tauri build) uses `rand::thread_rng()` with getrandom's OS backend automatically.

### Tauri setup
- The desktop app adds **no custom Tauri commands** — all logic runs in the WebView via WASM.
- `tauri.conf.json` `beforeDevCommand` starts the Vite dev server; paths are relative to `desktop/` (the Tauri CLI invocation dir), so `frontendDist` is `../frontend/dist`.
- No icons are bundled yet; add them to `desktop/src-tauri/icons/` and list them in `tauri.conf.json` `bundle.icon`.
