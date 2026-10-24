import { router, useFocusEffect } from 'expo-router';
import { useCallback, useState } from 'react';
import { Alert, Pressable, ScrollView, Text } from 'react-native';
import { Card, colors, ui } from '../components/ui';
import { computeScoreboard } from '../lib/scoring';
import { deleteGame, loadGames } from '../lib/storage';
import { Game } from '../lib/types';

export default function History() {
  const [games, setGames] = useState<Game[]>([]);

  const reload = useCallback(() => {
    loadGames().then((all) => setGames(all.filter((g) => g.status === 'finished')));
  }, []);

  useFocusEffect(reload);

  const confirmDelete = (game: Game) =>
    Alert.alert('Borrar partida', '¿Seguro que querés borrar esta partida?', [
      { text: 'Cancelar', style: 'cancel' },
      { text: 'Borrar', style: 'destructive', onPress: () => deleteGame(game.id).then(reload) },
    ]);

  return (
    <ScrollView style={ui.screen} contentContainerStyle={ui.content}>
      {games.length === 0 && (
        <Text style={[ui.subtitle, { textAlign: 'center', marginTop: 24 }]}>
          Todavía no hay partidas terminadas.
        </Text>
      )}
      {games.map((g) => {
        const winner = computeScoreboard(g)[0];
        return (
          <Pressable
            key={g.id}
            onPress={() => router.push({ pathname: '/results/[id]', params: { id: g.id } })}
            onLongPress={() => confirmDelete(g)}>
            <Card>
              <Text style={ui.subtitle}>{new Date(g.finishedAt ?? g.createdAt).toLocaleString()}</Text>
              <Text style={ui.text}>🏆 {winner?.player.name} · {winner?.total} pts</Text>
              <Text style={{ color: colors.muted }}>
                {g.players.map((p) => p.name).join(', ')} · {g.rounds} ronda(s)
              </Text>
            </Card>
          </Pressable>
        );
      })}
      {games.length > 0 && (
        <Text style={[ui.subtitle, { textAlign: 'center' }]}>Mantené presionada una partida para borrarla.</Text>
      )}
    </ScrollView>
  );
}
