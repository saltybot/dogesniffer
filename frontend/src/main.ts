import init, { Game } from 'dogesniffer-core';

// Dynamically loaded only inside Tauri
type TauriWindow = import('@tauri-apps/api/window').Window;
let tauriWindow: TauriWindow | null = null;

async function initTauri() {
  if (!('__TAURI_INTERNALS__' in window)) return;
  const { getCurrentWindow } = await import('@tauri-apps/api/window');
  tauriWindow = getCurrentWindow();
  document.body.classList.add('tauri');
}

async function resizeToFit() {
  if (!tauriWindow) return;
  // Wait one frame so the DOM has fully laid out the new board
  await new Promise<void>(r => requestAnimationFrame(() => r()));
  const app = document.getElementById('app')!;
  const { LogicalSize } = await import('@tauri-apps/api/dpi');
  const isTauri = document.body.classList.contains('tauri');
  const padding = isTauri ? 0 : 32;
  const w = app.offsetWidth  + padding;
  const h = app.offsetHeight + padding;
  await tauriWindow.setSize(new LogicalSize(w, h));
}

type CellKind = 'hidden' | 'revealed' | 'flagged' | 'poop' | 'wrong_flag' | 'hit_poop';
interface CellView { kind: CellKind; adjacent: number; }

const DIFFICULTIES = {
  beginner:     { width: 9,  height: 9,  poops: 10 },
  intermediate: { width: 16, height: 16, poops: 40 },
  expert:       { width: 30, height: 16, poops: 99 },
} as const;

type Difficulty = keyof typeof DIFFICULTIES;

// Classic minesweeper number colours
const NUM_COLORS = ['', '#0000FF', '#008000', '#FF0000', '#000080', '#800000', '#008080', '#000000', '#808080'];

const DOGE_PHRASES = [
  'wow', 'such win', 'much skill', 'very amaze', 'so fast',
  'many flag', 'such brain', 'very clear', 'much safe',
  'so win', 'such pro', 'very doge', 'much victory',
  'so clean', 'such genius', 'many poop avoid', 'very victory',
  'so brave', 'much clear', 'such sniff',
];
const DOGE_COLORS = ['#FF4444', '#FF8C00', '#FFD700', '#00CC44', '#00BBFF', '#CC44FF', '#FF69B4'];

const WIN_PHRASE_COUNT: Record<Difficulty, number> = {
  beginner:     6,
  intermediate: 9,
  expert:       12,
};

const CELL_SIZE: Record<Difficulty, number> = {
  beginner:     36,
  intermediate: 32,
  expert:       32,
};

let game: Game;
let difficulty: Difficulty = 'beginner';
let timerInterval: ReturnType<typeof setInterval> | null = null;
let startTime = 0;
let gameStarted = false;

// ── DOM helpers ──────────────────────────────────────────────────────────────
const $  = (id: string) => document.getElementById(id) as HTMLElement;
const boardEl  = () => $('board') as HTMLDivElement;
const counterEl = () => $('poop-counter');
const timerEl  = () => $('timer');
const dogeBtn  = () => $('reset-btn') as HTMLButtonElement;
const dogeImg  = () => $('doge-img') as HTMLImageElement;

function lcdFormat(n: number): string {
  const clamped = Math.max(-99, Math.min(999, n));
  return clamped < 0
    ? '-' + String(Math.abs(clamped)).padStart(2, '0')
    : String(clamped).padStart(3, '0');
}

function setDogeState(state: 'normal' | 'win' | 'dead') {
  const img = dogeImg();
  if (state === 'win') {
    img.src = new URL('./doge_win.png', import.meta.url).href;
  } else if (state === 'dead') {
    img.src = new URL('./doge_lose.png', import.meta.url).href;
  } else {
    img.src = new URL('./doge.png', import.meta.url).href;
  }
  img.style.filter = '';
  img.style.transform = '';
}

// ── Game lifecycle ───────────────────────────────────────────────────────────
function newGame() {
  stopTimer();
  clearDogeWin();
  gameStarted = false;
  const cfg = DIFFICULTIES[difficulty];
  game = new Game(cfg.width, cfg.height, cfg.poops);

  counterEl().textContent = lcdFormat(cfg.poops);
  timerEl().textContent = '000';
  setDogeState('normal');
  buildBoard(cfg.width, cfg.height);
  resizeToFit();
}

function startTimer() {
  startTime = Date.now();
  timerInterval = setInterval(() => {
    const secs = Math.min(999, Math.floor((Date.now() - startTime) / 1000));
    timerEl().textContent = String(secs).padStart(3, '0');
  }, 200);
}

function stopTimer() {
  if (timerInterval !== null) {
    clearInterval(timerInterval);
    timerInterval = null;
  }
}

// ── Board rendering ──────────────────────────────────────────────────────────
function buildBoard(width: number, height: number) {
  const el = boardEl();
  const cellPx = CELL_SIZE[difficulty];
  el.style.setProperty('--cell-size', `${cellPx}px`);
  el.style.gridTemplateColumns = `repeat(${width}, ${cellPx}px)`;
  el.innerHTML = '';

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      const cell = document.createElement('div');
      cell.className = 'cell hidden';
      cell.dataset.x = String(x);
      cell.dataset.y = String(y);
      el.appendChild(cell);
    }
  }

  // Single delegated listeners on the board container
  el.addEventListener('click', onCellClick);
  el.addEventListener('contextmenu', onCellRightClick);
  el.addEventListener('dblclick', onCellDblClick);
  el.addEventListener('auxclick', onCellMiddleClick);
  el.addEventListener('touchstart', onCellTouchStart, { passive: true });
  el.addEventListener('touchend', onCellTouchEnd);
  el.addEventListener('touchmove', onCellTouchMove, { passive: true });
}

function cellFromEvent(e: MouseEvent): HTMLElement | null {
  return (e.target as HTMLElement).closest<HTMLElement>('.cell');
}

function coords(el: HTMLElement): [number, number] {
  return [parseInt(el.dataset.x!), parseInt(el.dataset.y!)];
}

// ── Input handlers ───────────────────────────────────────────────────────────
function onCellClick(e: MouseEvent) {
  const cell = cellFromEvent(e);
  if (!cell || game.is_game_over()) return;
  if (!gameStarted) { gameStarted = true; startTimer(); }
  const [x, y] = coords(cell);
  game.reveal(x, y);
  syncBoard();
}

function onCellRightClick(e: MouseEvent) {
  e.preventDefault();
  const cell = cellFromEvent(e);
  if (!cell || game.is_game_over()) return;
  const [x, y] = coords(cell);
  game.flag(x, y);
  syncBoard();
}

function onCellMiddleClick(e: MouseEvent) {
  if (e.button !== 1) return;
  e.preventDefault();
  const cell = cellFromEvent(e);
  if (!cell || game.is_game_over()) return;
  const [x, y] = coords(cell);
  game.chord(x, y);
  syncBoard();
}

function onCellDblClick(e: MouseEvent) {
  e.preventDefault();
  const cell = cellFromEvent(e);
  if (!cell || game.is_game_over()) return;
  const [x, y] = coords(cell);
  game.chord(x, y);
  syncBoard();
}

// ── Long-press (mobile flag) ─────────────────────────────────────────────────
let longPressTimer: ReturnType<typeof setTimeout> | null = null;
let longPressFired = false;

function onCellTouchStart(e: TouchEvent) {
  longPressFired = false;
  const cell = (e.target as HTMLElement).closest<HTMLElement>('.cell');
  if (!cell || game.is_game_over()) return;
  longPressTimer = setTimeout(() => {
    longPressFired = true;
    const [x, y] = coords(cell);
    game.flag(x, y);
    syncBoard();
  }, 500);
}

function onCellTouchMove() {
  if (longPressTimer !== null) {
    clearTimeout(longPressTimer);
    longPressTimer = null;
  }
}

function onCellTouchEnd(e: TouchEvent) {
  if (longPressTimer !== null) {
    clearTimeout(longPressTimer);
    longPressTimer = null;
  }
  // Suppress the synthetic click that follows a long press
  if (longPressFired) {
    e.preventDefault();
    longPressFired = false;
  }
}

// ── Board sync ───────────────────────────────────────────────────────────────
function syncBoard() {
  const boardData = game.get_board() as CellView[][];

  boardEl().querySelectorAll<HTMLElement>('.cell').forEach(el => {
    const x = parseInt(el.dataset.x!);
    const y = parseInt(el.dataset.y!);
    const cell = boardData[y][x];

    el.className = 'cell';
    el.textContent = '';
    el.style.color = '';

    switch (cell.kind) {
      case 'hidden':
        el.classList.add('hidden');
        break;
      case 'revealed':
        el.classList.add('revealed');
        if (cell.adjacent > 0) {
          el.textContent = String(cell.adjacent);
          el.style.color = NUM_COLORS[cell.adjacent];
        }
        break;
      case 'flagged':
        el.classList.add('hidden', 'flagged');
        el.textContent = '🚩';
        break;
      case 'poop':
        el.classList.add('poop');
        el.textContent = '💩';
        break;
      case 'hit_poop':
        el.classList.add('poop', 'hit');
        el.textContent = '💩';
        break;
      case 'wrong_flag':
        el.classList.add('revealed', 'wrong-flag');
        el.textContent = '❌';
        break;
    }
  });

  counterEl().textContent = lcdFormat(game.flags_remaining());

  if (game.is_game_over()) {
    stopTimer();
    setDogeState(game.is_won() ? 'win' : 'dead');
    if (game.is_won()) {
      const elapsed = Math.floor((Date.now() - startTime) / 1000);
      const store = lbLoad();
      const best = (store[difficulty] ?? [])[0];
      if (!best || elapsed < best.time) {
        promptName(elapsed);
      }
      showDogeWin();
    }
  }
}

// ── Doge win overlay ─────────────────────────────────────────────────────────
function showDogeWin() {
  const board = boardEl();
  const parent = board.offsetParent as HTMLElement;

  const overlay = document.createElement('div');
  overlay.id = 'doge-win-overlay';
  overlay.style.cssText = `
    position:absolute;
    left:${board.offsetLeft}px;
    top:${board.offsetTop}px;
    width:${board.offsetWidth}px;
    height:${board.offsetHeight}px;
    pointer-events:none;
    overflow:hidden;
  `;
  parent.appendChild(overlay);

  const shuffled = [...DOGE_PHRASES].sort(() => Math.random() - 0.5).slice(0, WIN_PHRASE_COUNT[difficulty]);
  const placed: DOMRect[] = [];
  let visibleIndex = 0;

  for (const phrase of shuffled) {
    const el = document.createElement('span');
    el.textContent = phrase;
    const size = 18 + Math.floor(Math.random() * 11);
    const rotate = -25 + Math.floor(Math.random() * 50);
    const color = DOGE_COLORS[Math.floor(Math.random() * DOGE_COLORS.length)];
    el.style.cssText = `
      position:absolute;
      font-family:'Comic Sans MS','Chalkboard SE',cursive;
      font-size:${size}px;
      font-weight:bold;
      color:${color};
      text-shadow:1px 1px 0 #fff,-1px -1px 0 #fff,1px -1px 0 #fff,-1px 1px 0 #fff;
      transform:rotate(${rotate}deg);
      opacity:0;
      white-space:nowrap;
      transition:none;
    `;
    overlay.appendChild(el);

    // Try random positions; keep the first that doesn't overlap and is fully inside the overlay
    let fits = false;
    const oRect = overlay.getBoundingClientRect();
    for (let attempt = 0; attempt < 30; attempt++) {
      el.style.left = (Math.random() * 78) + '%';
      el.style.top  = (Math.random() * 83) + '%';

      const r = el.getBoundingClientRect();
      const pad = 6;
      // Must be fully within overlay bounds
      if (r.left < oRect.left || r.right > oRect.right ||
          r.top  < oRect.top  || r.bottom > oRect.bottom) continue;
      const overlaps = placed.some(p =>
        r.left - pad < p.right && r.right + pad > p.left &&
        r.top  - pad < p.bottom && r.bottom + pad > p.top
      );
      if (!overlaps) {
        placed.push(r);
        fits = true;
        break;
      }
    }

    if (fits) {
      const delay = visibleIndex * 120;
      el.style.transition = `opacity 0.35s ease ${delay}ms`;
      requestAnimationFrame(() => requestAnimationFrame(() => { el.style.opacity = '1'; }));
      visibleIndex++;
    } else {
      el.remove(); // no room — skip this phrase
    }
  }
}

function clearDogeWin() {
  document.getElementById('doge-win-overlay')?.remove();
}

// ── Leaderboard ───────────────────────────────────────────────────────────────
const LB_KEY = 'dogesniffer_leaderboard';
const LB_NAME_KEY = 'dogesniffer_last_name';
const LB_MAX = 1;

type LBEntry = { time: number; name: string };
type LBStore = Record<Difficulty, LBEntry[]>;

function lbLoad(): LBStore {
  try { return JSON.parse(localStorage.getItem(LB_KEY) || '{}'); } catch { return {} as LBStore; }
}

function lbSave(store: LBStore) {
  localStorage.setItem(LB_KEY, JSON.stringify(store));
}

function lbRecord(diff: Difficulty, time: number, name: string) {
  const store = lbLoad();
  const entries: LBEntry[] = store[diff] ?? [];
  entries.push({ time, name: name.trim() || 'Anonymous' });
  entries.sort((a, b) => a.time - b.time);
  store[diff] = entries.slice(0, LB_MAX);
  lbSave(store);
}

function lbRender() {
  const store = lbLoad();
  const tbody = document.querySelector('#lb-table tbody')!;
  tbody.innerHTML = '';
  const diffs: Difficulty[] = ['beginner', 'intermediate', 'expert'];
  for (const diff of diffs) {
    const entries = store[diff] ?? [];
    if (entries.length === 0) {
      const tr = document.createElement('tr');
      tr.innerHTML = `<td>${diff}</td><td>—</td><td>—</td>`;
      tbody.appendChild(tr);
    } else {
      const e = entries[0];
      const tr = document.createElement('tr');
      tr.innerHTML = `<td>${diff}</td><td>${e.name}</td><td>${e.time}s</td>`;
      tbody.appendChild(tr);
    }
  }
}

function promptName(time: number) {
  const modal = document.getElementById('name-modal')!;
  const input = document.getElementById('name-input') as HTMLInputElement;
  const submit = document.getElementById('name-submit')!;

  // Pre-fill with last used name
  input.value = localStorage.getItem(LB_NAME_KEY) ?? '';
  modal.classList.remove('modal-hidden');
  input.focus();
  input.select();

  const finish = () => {
    const name = input.value.trim() || 'Anonymous';
    localStorage.setItem(LB_NAME_KEY, name);
    lbRecord(difficulty, time, name);
    modal.classList.add('modal-hidden');
    submit.removeEventListener('click', finish);
    input.removeEventListener('keydown', onKey);
    lbShow();
  };
  const onKey = (e: KeyboardEvent) => { if (e.key === 'Enter') finish(); };
  submit.addEventListener('click', finish);
  input.addEventListener('keydown', onKey);
}

function lbShow() { lbRender(); document.getElementById('lb-modal')!.classList.remove('modal-hidden'); }
function lbHide() { document.getElementById('lb-modal')!.classList.add('modal-hidden'); }

// ── Bootstrap ────────────────────────────────────────────────────────────────
async function main() {
  await init();
  await initTauri();

  document.querySelectorAll<HTMLButtonElement>('[data-diff]').forEach(btn => {
    btn.addEventListener('click', () => {
      difficulty = btn.dataset.diff as Difficulty;
      document.querySelectorAll('[data-diff]').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      newGame();
    });
  });

  dogeBtn().addEventListener('click', newGame);

  // Mirror doge toward mouse during gameplay
  document.addEventListener('mousemove', (e) => {
    if (game?.is_game_over()) return;
    const img = dogeImg();
    const rect = img.getBoundingClientRect();
    const dogeCenterX = rect.left + rect.width / 2;
    img.style.transform = e.clientX < dogeCenterX ? 'scaleX(-1)' : '';
  });

  document.getElementById('lb-btn')!.addEventListener('click', lbShow);
  document.getElementById('lb-close')!.addEventListener('click', lbHide);
  document.getElementById('lb-clear')!.addEventListener('click', () => {
    localStorage.removeItem(LB_KEY);
    lbRender();
  });
  document.getElementById('lb-modal')!.addEventListener('click', (e) => {
    if (e.target === e.currentTarget) lbHide();
  });

  // Cheat code
  const CHEAT = 'much cheat';
  let cheatBuffer = '';
  document.addEventListener('keydown', (e) => {
    cheatBuffer = (cheatBuffer + e.key).slice(-CHEAT.length);
    if (cheatBuffer === CHEAT && !game.is_game_over()) {
      game.cheat_win();
      syncBoard();
    }
  });

  newGame();
}

main();
