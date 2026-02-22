# Dogesniffer

Minesweeper clone where mines are replaced with 💩 and the smiley face is a Doge. Available as a web app and a cross-platform desktop app (Tauri).

## Architecture

```
dogesniffer/
├── core/           # Rust game logic — compiled to WebAssembly via wasm-pack
├── frontend/       # Vanilla TypeScript + Vite UI (shared by web and desktop)
└── desktop/        # Tauri wrapper (uses the same frontend build)
```

The Rust core is compiled to WASM and consumed by the frontend as an npm package (`dogesniffer-core`). Tauri wraps the same frontend build for the desktop app, so there is a single UI codebase.

## Prerequisites

- [Rust + Cargo](https://rustup.rs/)
- [wasm-pack](https://rustwasm.github.io/wasm-pack/installer/)
- [Node.js](https://nodejs.org/) (v18+)
- For desktop: [Tauri prerequisites](https://tauri.app/start/prerequisites/) (platform-specific system deps)

## First-time setup

```bash
# 1. Compile the Rust core to WASM (generates core/pkg/)
wasm-pack build core --target web

# 2. Install frontend npm dependencies (picks up the local core/pkg package)
cd frontend && npm install
```

Or use the root helper:
```bash
npm run setup
```

## Development

### Web
```bash
npm run dev:web          # http://localhost:5173
```

### Desktop
```bash
npm run dev:desktop      # opens Tauri window (also starts the Vite dev server)
```

> **Note:** every time you change `core/src/lib.rs` you must re-run `wasm-pack build core --target web` before the frontend picks up the changes.

## Building for production

```bash
npm run build:web        # output in frontend/dist/
npm run build:desktop    # output in desktop/src-tauri/target/release/bundle/
```

## Game rules

Standard minesweeper rules, with:
- 💩 instead of mines
- 🚩 right-click to flag
- Double-click a revealed number to chord (reveal unflagged neighbours when flag count matches)
- Doge face at the top — click to start a new game

## Tech stack

| Layer | Technology |
|---|---|
| Game logic | Rust → WASM (`wasm-bindgen`, `wasm-pack`) |
| Frontend UI | Vanilla TypeScript + Vite |
| Desktop wrapper | Tauri v2 |
