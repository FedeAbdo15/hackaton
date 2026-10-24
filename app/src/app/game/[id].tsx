import { router, useLocalSearchParams } from 'expo-router';
import { useEffect, useState } from 'react';
import { ActivityIndicator, Alert, ScrollView, Text, TextInput, View } from 'react-native';
import { Button, Card, colors, ui } from '../../components/ui';
import { formatTime, hiderPointsFor, seekerPointsFor } from '../../lib/scoring';
import { getGame, saveGame } from '../../lib/storage';
import { Game, isTurnPlayed } from '../../lib/types';

export default function GameScreen() {
  const { id } = useLocalSearchParams<{ id: string }>();
  const [game, setGame] = useState<Game>();
  const [minutes, setMinutes] = useState('');
  const [seconds, setSeconds] = useState('');

  useEffect(() => {
    getGame(id).then(setGame);
  }, [id]);

  if (!game) {
    return (
      <View style={[ui.screen, { justifyContent: 'center' }]}>
        <ActivityIndicator color={colors.accent} />
      </View>
    );
  }

  const nameOf = (playerId: string) => game.players.find((p) => p.id === playerId)?.name ?? '?';
  const currentIndex = game.turns.findIndex((t) => !isTurnPlayed(t));
  const turn = game.turns[currentIndex];
  const lastPlayed = game.turns.filter(isTurnPlayed).at(-1);

  const recordTurn = async (found: boolean) => {
    let total = 0;
    if (found) {
      const m = parseInt(minutes || '0', 10);
      const s = parseInt(seconds || '0', 10);
      if (Number.isNaN(m) || Number.isNaN(s) || s > 59 || m < 0 || s < 0) {
        Alert.alert('Tiempo inválido', 'Ingresá el tiempo de la pantalla del Tesoro (MM:SS).');
        return;
      }
      total = m * 60 + s;
    }

    const turns = [...game.turns];
    turns[currentIndex] = { ...turn, found, seconds: found ? total : undefined };
    const finished = turns.every(isTurnPlayed);
    const updated: Game = {
      ...game,
      turns,
      status: finished ? 'finished' : 'in_progress',
      finishedAt: finished ? Date.now() : undefined,
    };

    await saveGame(updated);
    setMinutes('');
    setSeconds('');

    if (finished) {
      router.replace({ pathname: '/results/[id]', params: { id: game.id } });
    } else {
      setGame(updated);
    }
  };

  const confirmGiveUp = () =>
    Alert.alert('¿Se rindió?', `${nameOf(turn.seekerId)} no suma puntos y ${nameOf(turn.hiderId)} gana el bonus de escondedor.`, [
      { text: 'Cancelar', style: 'cancel' },
      { text: 'Sí, se rindió', style: 'destructive', onPress: () => recordTurn(false) },
    ]);

  if (!turn) {
    return (
      <View style={[ui.screen, ui.content]}>
        <Button title="Ver scoreboard" onPress={() => router.replace({ pathname: '/results/[id]', params: { id: game.id } })} />
      </View>
    );
  }

  return (
    <ScrollView style={ui.screen} contentContainerStyle={ui.content} keyboardShouldPersistTaps="handled">
      <Text style={ui.subtitle}>
        Ronda {turn.round} de {game.rounds} · Turno {currentIndex + 1} de {game.turns.length}
      </Text>

      <Card style={{ borderColor: colors.hider }}>
        <Text style={[ui.label, { color: colors.hider }]}>🏴‍☠️ Esconde el Tesoro</Text>
        <Text style={ui.title}>{nameOf(turn.hiderId)}</Text>
        <Text style={ui.subtitle}>Escondé la placa Tesoro y presioná el BOTÓN 1.</Text>
      </Card>

      <Card style={{ borderColor: colors.seeker }}>
        <Text style={[ui.label, { color: colors.seeker }]}>🔍 Busca</Text>
        <Text style={ui.title}>{nameOf(turn.seekerId)}</Text>
        <Text style={ui.subtitle}>Presioná el BOTÓN 2 en el Buscador para empezar.</Text>
      </Card>

      <Text style={[ui.label, { marginTop: 8 }]}>Tiempo en la pantalla del Tesoro</Text>
      <View style={[ui.row, { justifyContent: 'center' }]}>
        <TextInput
          style={[ui.input, { width: 80, textAlign: 'center', fontSize: 28 }]}
          keyboardType="number-pad"
          placeholder="MM"
          placeholderTextColor={colors.muted}
          maxLength={3}
          value={minutes}
          onChangeText={setMinutes}
        />
        <Text style={ui.title}>:</Text>
        <TextInput
          style={[ui.input, { width: 80, textAlign: 'center', fontSize: 28 }]}
          keyboardType="number-pad"
          placeholder="SS"
          placeholderTextColor={colors.muted}
          maxLength={2}
          value={seconds}
          onChangeText={setSeconds}
        />
      </View>

      <Button title="¡Lo encontró! Guardar tiempo" onPress={() => recordTurn(true)} disabled={!minutes && !seconds} />
      <Button title="Se rindió" variant="secondary" onPress={confirmGiveUp} />

      {lastPlayed && (
        <Card style={{ marginTop: 8 }}>
          <Text style={ui.label}>Turno anterior</Text>
          <Text style={ui.text}>
            {nameOf(lastPlayed.seekerId)} {lastPlayed.found ? `encontró el tesoro de ${nameOf(lastPlayed.hiderId)} en ${formatTime(lastPlayed.seconds ?? 0)}` : `no encontró el tesoro de ${nameOf(lastPlayed.hiderId)}`}
          </Text>
          <Text style={{ color: colors.seeker }}>🔍 +{seekerPointsFor(lastPlayed)}  <Text style={{ color: colors.hider }}>🏴‍☠️ +{hiderPointsFor(lastPlayed)}</Text></Text>
        </Card>
      )}

      <Button
        title="Ver puntaje parcial"
        variant="secondary"
        onPress={() => router.push({ pathname: '/results/[id]', params: { id: game.id } })}
      />
    </ScrollView>
  );
}
