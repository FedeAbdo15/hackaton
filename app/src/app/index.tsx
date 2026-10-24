import { router, useFocusEffect } from 'expo-router';
import { useCallback, useState } from 'react';
import { ScrollView, Text } from 'react-native';
import { Button, Card, colors, ui } from '../components/ui';
import { loadGames } from '../lib/storage';
import { Game, isTurnPlayed } from '../lib/types';

export default function Home() {
  const [games, setGames] = useState<Game[]>([]);

  useFocusEffect(
    useCallback(() => {
      loadGames().then(setGames);
    }, []),
  );

  const inProgress = games.filter((g) => g.status === 'in_progress');
  const finishedCount = games.length - inProgress.length;

  return (
    <ScrollView style={ui.screen} contentContainerStyle={ui.content}>
      <Text style={{ fontSize: 56, textAlign: 'center' }}>🏴‍☠️</Text>
      <Text style={[ui.title, { textAlign: 'center' }]}>Búsqueda del Tesoro</Text>
      <Text style={[ui.subtitle, { textAlign: 'center', marginBottom: 8 }]}>
        Armá la partida, rotá los roles y llevá el puntaje de cada ronda.
      </Text>

      <Button title="Nueva partida" onPress={() => router.push('/new')} />
      <Button
        title={`Partidas anteriores (${finishedCount})`}
        variant="secondary"
        onPress={() => router.push('/history')}
      />

      {inProgress.length > 0 && <Text style={[ui.label, { marginTop: 12 }]}>Continuar</Text>}
      {inProgress.map((g) => {
        const played = g.turns.filter(isTurnPlayed).length;
        return (
          <Card key={g.id}>
            <Text style={ui.text}>{g.players.map((p) => p.name).join(', ')}</Text>
            <Text style={{ color: colors.muted }}>
              Turno {Math.min(played + 1, g.turns.length)} de {g.turns.length} · {g.rounds} ronda(s)
            </Text>
            <Button
              title="Continuar partida"
              variant="secondary"
              onPress={() => router.push({ pathname: '/game/[id]', params: { id: g.id } })}
            />
          </Card>
        );
      })}
    </ScrollView>
  );
}
