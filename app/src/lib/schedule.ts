import { Player, Turn } from './types';

function shuffle<T>(items: T[]): T[] {
  const copy = [...items];
  for (let i = copy.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [copy[i], copy[j]] = [copy[j], copy[i]];
  }
  return copy;
}

// Genera el orden de turnos de toda la partida.
// En cada ronda cada jugador esconde 1 vez y busca 1 vez.
// El "desfase" entre escondedor y buscador cambia en cada ronda, así las duplas
// van rotando y nadie busca siempre el tesoro del mismo compañero.
export function generateTurns(players: Player[], rounds: number): Turn[] {
  const order = shuffle(players);
  const n = order.length;
  const turns: Turn[] = [];

  for (let round = 1; round <= rounds; round++) {
    const offset = 1 + ((round - 1) % (n - 1));
    for (let i = 0; i < n; i++) {
      turns.push({
        round,
        hiderId: order[i].id,
        seekerId: order[(i + offset) % n].id,
      });
    }
  }
  return turns;
}
