import AsyncStorage from '@react-native-async-storage/async-storage';
import { Game } from './types';

// Todas las partidas se guardan localmente en el dispositivo (sin backend).
const GAMES_KEY = 'tesoro:games:v1';

export async function loadGames(): Promise<Game[]> {
  const raw = await AsyncStorage.getItem(GAMES_KEY);
  if (!raw) return [];
  try {
    const games = JSON.parse(raw) as Game[];
    return games.sort((a, b) => b.createdAt - a.createdAt);
  } catch {
    return [];
  }
}

async function writeGames(games: Game[]): Promise<void> {
  await AsyncStorage.setItem(GAMES_KEY, JSON.stringify(games));
}

export async function getGame(id: string): Promise<Game | undefined> {
  const games = await loadGames();
  return games.find((g) => g.id === id);
}

export async function saveGame(game: Game): Promise<void> {
  const games = await loadGames();
  const index = games.findIndex((g) => g.id === game.id);
  if (index >= 0) {
    games[index] = game;
  } else {
    games.push(game);
  }
  await writeGames(games);
}

export async function deleteGame(id: string): Promise<void> {
  const games = await loadGames();
  await writeGames(games.filter((g) => g.id !== id));
}

export const newId = (): string =>
  Date.now().toString(36) + Math.random().toString(36).slice(2, 8);
