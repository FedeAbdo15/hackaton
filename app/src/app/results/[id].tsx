import { router, useFocusEffect, useLocalSearchParams } from 'expo-router';
import { useCallback, useState } from 'react';
import { ActivityIndicator, ScrollView, Text, View } from 'react-native';
import { Scoreboard } from '../../components/Scoreboard';
import { Button, Card, colors, ui } from '../../components/ui';
import { computeScoreboard, formatTime, hiderPointsFor, seekerPointsFor } from '../../lib/scoring';
import { getGame } from '../../lib/storage';
import { Game, isTurnPlayed } from '../../lib/types';

export default function Results() {
  const { id } = useLocalSearchParams<{ id: string }>();
  const [game, setGame] = useState<Game>();

  useFocusEffect(
    useCallback(() => {
      getGame(id).then(setGame);
    }, [id]),
  );

  if (!game) {
    return (
      <View style={[ui.screen, { justifyContent: 'center' }]}>
        <ActivityIndicator color={colors.accent} />
      </View>
    );
  }

  const nameOf = (playerId: string) => game.players.find((p) => p.id === playerId)?.name ?? '?';
  const scores = computeScoreboard(game);
  const finished = game.status === 'finished';
  const played = game.turns.filter(isTurnPlayed);

  return (
    <ScrollView style={ui.screen} contentContainerStyle={ui.content}>
      {finished ? (
        <>
          <Text style={{ fontSize: 48, textAlign: 'center' }}>🏆</Text>
          <Text style={[ui.title, { textAlign: 'center' }]}>¡Ganó {scores[0]?.player.name}!</Text>
          <Text style={[ui.subtitle, { textAlign: 'center' }]}>
            {new Date(game.finishedAt ?? game.createdAt).toLocaleString()} · {game.rounds} ronda(s)
          </Text>
        </>
      ) : (
        <Text style={ui.subtitle}>
          Puntaje parcial · {played.length} de {game.turns.length} turnos jugados
        </Text>
      )}

      <Scoreboard scores={scores} />

      {played.length > 0 && <Text style={[ui.label, { marginTop: 8 }]}>Detalle de turnos</Text>}
      {played.map((t, i) => (
        <Card key={i}>
          <Text style={ui.subtitle}>Ronda {t.round}</Text>
          <Text style={ui.text}>
            🔍 {nameOf(t.seekerId)} → 🏴‍☠️ {nameOf(t.hiderId)}:{' '}
            {t.found ? formatTime(t.seconds ?? 0) : 'no encontrado'}
          </Text>
          <Text style={{ color: colors.muted }}>
            Buscador +{seekerPointsFor(t)} · Escondedor +{hiderPointsFor(t)}
          </Text>
        </Card>
      ))}

      {finished ? (
        <Button title="Volver al inicio" onPress={() => router.dismissTo('/')} />
      ) : (
        <Button title="Volver a la partida" onPress={() => router.back()} />
      )}
    </ScrollView>
  );
}
