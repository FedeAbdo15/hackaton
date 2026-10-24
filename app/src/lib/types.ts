export interface Player {
  id: string;
  name: string;
}

// Un turno = una búsqueda: un jugador esconde el Tesoro y otro lo busca.
export interface Turn {
  round: number;        // 1..rounds
  hiderId: string;      // esconde el Tesoro (BOTÓN 1)
  seekerId: string;     // busca con el Buscador (BOTÓN 2)
  seconds?: number;     // tiempo que mostró la OLED del Tesoro al ser encontrado
  found?: boolean;      // false = el buscador se rindió
}

export type GameStatus = 'in_progress' | 'finished';

export interface Game {
  id: string;
  createdAt: number;
  finishedAt?: number;
  players: Player[];
  rounds: number;
  turns: Turn[];
  status: GameStatus;
}

export interface PlayerScore {
  player: Player;
  seekerPoints: number;
  hiderPoints: number;
  total: number;
  bestSeekSeconds?: number;  // mejor tiempo encontrando
  bestHideSeconds?: number;  // escondite que más tardaron en encontrar
}

export const isTurnPlayed = (turn: Turn): boolean => turn.found !== undefined;
