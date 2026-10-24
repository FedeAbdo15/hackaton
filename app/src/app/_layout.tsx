import { Stack } from 'expo-router';
import { StatusBar } from 'expo-status-bar';
import { colors } from '../components/ui';

export default function RootLayout() {
  return (
    <>
      <StatusBar style="light" />
      <Stack
        screenOptions={{
          headerStyle: { backgroundColor: colors.bg },
          headerTintColor: colors.text,
          headerTitleStyle: { fontWeight: '700' },
          contentStyle: { backgroundColor: colors.bg },
        }}>
        <Stack.Screen name="index" options={{ title: 'Búsqueda del Tesoro' }} />
        <Stack.Screen name="new" options={{ title: 'Nueva partida' }} />
        <Stack.Screen name="game/[id]" options={{ title: 'Partida en curso' }} />
        <Stack.Screen name="results/[id]" options={{ title: 'Scoreboard' }} />
        <Stack.Screen name="history" options={{ title: 'Partidas anteriores' }} />
      </Stack>
    </>
  );
}
