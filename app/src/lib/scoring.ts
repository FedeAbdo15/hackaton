import { Game, PlayerScore, Turn, isTurnPlayed } from './types';

// --- Reglas de puntaje (ajustables) ---
// Buscador: suma más cuanto MENOS tarda en encontrar el Tesoro.
// Escondedor: suma más cuanto MÁS tardan en encontrar su Tesoro.
export const SCORING = {
  MAX_SECONDS: 300,        // a partir de 5 min el tiempo ya no cambia el puntaje
  SEEKER_MAX: 100,         // encontrarlo al instante
  SEEKER_MIN: 10,          // encontrarlo, aunque sea muy tarde
  HIDER_MAX: 60,           // tardaron MAX_SECONDS o más
  HIDER_NOT_FOUND: 80,     // el buscador se rindió: premio extra al escondedor
};

const clampRatio = (seconds: number) =>
  Math.min(Math.max(seconds, 0), SCORING.MAX_SECONDS) / SCORING.MAX_SECONDS;

export function seekerPointsFor(turn: Turn): number {
  if (!isTurnPlayed(turn) || !turn.found) return 0;
  const pts = SCORING.SEEKER_MAX * (1 - clampRatio(turn.seconds ?? 0));
  return Math.round(Math.max(SCORING.SEEKER_MIN, pts));
}

export function hiderPointsFor(turn: Turn): number {
  if (!isTurnPlayed(turn)) return 0;
  if (!turn.found) return SCORING.HIDER_NOT_FOUND;
  return Math.round(SCORING.HIDER_MAX * clampRatio(turn.seconds ?? 0));
}

// Scoreboard ordenado: mayor puntaje total primero.
// Desempate: mejor tiempo como buscador.
export function computeScoreboard(game: Game): PlayerScore[] {
  const scores = new Map<string, PlayerScore>(
    game.players.map((p) => [p.id, { player: p, seekerPoints: 0, hiderPoints: 0, total: 0 }]),
  );

  for (const turn of game.turns) {
    if (!isTurnPlayed(turn)) continue;
    const seeker = scores.get(turn.seekerId);
    const hider = scores.get(turn.hiderId);

    if (seeker) {
      seeker.seekerPoints += seekerPointsFor(turn);
      if (turn.found && turn.seconds !== undefined &&
          (seeker.bestSeekSeconds === undefined || turn.seconds < seeker.bestSeekSeconds)) {
        seeker.bestSeekSeconds = turn.seconds;
      }
    }
    if (hider) {
      hider.hiderPoints += hiderPointsFor(turn);
      if (turn.found && turn.seconds !== undefined &&
          (hider.bestHideSeconds === undefined || turn.seconds > hider.bestHideSeconds)) {
        hider.bestHideSeconds = turn.seconds;
      }
    }
  }

  const list = [...scores.values()];
  for (const s of list) s.total = s.seekerPoints + s.hiderPoints;

  return list.sort((a, b) =>
    b.total - a.total ||
    (a.bestSeekSeconds ?? Infinity) - (b.bestSeekSeconds ?? Infinity));
}

export function formatTime(seconds: number): string {
  const m = Math.floor(seconds / 60);
  const s = seconds % 60;
  return `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
}
