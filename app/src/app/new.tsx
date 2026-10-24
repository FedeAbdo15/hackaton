import { router } from 'expo-router';
import { useState } from 'react';
import { Alert, Pressable, ScrollView, Text, TextInput, View } from 'react-native';
import { Button, Card, colors, ui } from '../components/ui';
import { generateTurns } from '../lib/schedule';
import { newId, saveGame } from '../lib/storage';
import { Game, Player } from '../lib/types';

const MIN_PLAYERS = 2;
const MAX_ROUNDS = 10;

export default function NewGame() {
  const [players, setPlayers] = useState<Player[]>([]);
  const [name, setName] = useState('');
  const [rounds, setRounds] = useState(1);

  const addPlayer = () => {
    const trimmed = name.trim();
    if (!trimmed) return;
    if (players.some((p) => p.name.toLowerCase() === trimmed.toLowerCase())) {
      Alert.alert('Nombre repetido', `"${trimmed}" ya está en la partida.`);
      return;
    }
    setPlayers([...players, { id: newId(), name: trimmed }]);
    setName('');
  };

  const removePlayer = (id: string) => setPlayers(players.filter((p) => p.id !== id));

  const start = async () => {
    const game: Game = {
      id: newId(),
      createdAt: Date.now(),
      players,
      rounds,
      turns: generateTurns(players, rounds),
      status: 'in_progress',
    };
    await saveGame(game);
    router.replace({ pathname: '/game/[id]', params: { id: game.id } });
  };

  const totalTurns = players.length * rounds;

  return (
    <ScrollView style={ui.screen} contentContainerStyle={ui.content} keyboardShouldPersistTaps="handled">
      <Text style={ui.label}>Participantes</Text>
      <View style={ui.row}>
        <TextInput
          style={[ui.input, { flex: 1 }]}
          placeholder="Nombre del jugador"
          placeholderTextColor={colors.muted}
          value={name}
          onChangeText={setName}
          onSubmitEditing={addPlayer}
          returnKeyType="done"
          submitBehavior="submit"
        />
        <Button title="Agregar" onPress={addPlayer} disabled={!name.trim()} />
      </View>

      {players.map((p, i) => (
        <Card key={p.id} style={{ flexDirection: 'row', alignItems: 'center' }}>
          <Text style={[ui.text, { flex: 1 }]}>{i + 1}. {p.name}</Text>
          <Pressable onPress={() => removePlayer(p.id)} hitSlop={10}>
            <Text style={{ color: colors.danger, fontWeight: '700' }}>Quitar</Text>
          </Pressable>
        </Card>
      ))}
      {players.length < MIN_PLAYERS && (
        <Text style={ui.subtitle}>Agregá al menos {MIN_PLAYERS} jugadores.</Text>
      )}

      <Text style={[ui.label, { marginTop: 8 }]}>Rondas</Text>
      <View style={[ui.row, { justifyContent: 'center', gap: 20 }]}>
        <Button title="−" variant="secondary" onPress={() => setRounds(Math.max(1, rounds - 1))} />
        <Text style={[ui.title, { minWidth: 40, textAlign: 'center' }]}>{rounds}</Text>
        <Button title="+" variant="secondary" onPress={() => setRounds(Math.min(MAX_ROUNDS, rounds + 1))} />
      </View>
      <Text style={[ui.subtitle, { textAlign: 'center' }]}>
        En cada ronda todos esconden el Tesoro 1 vez y buscan 1 vez.
        {players.length >= MIN_PLAYERS ? ` Total: ${totalTurns} turnos.` : ''}
      </Text>

      <Button
        title="Sortear orden y empezar"
        onPress={start}
        disabled={players.length < MIN_PLAYERS}
        style={{ marginTop: 12 }}
      />
    </ScrollView>
  );
}
