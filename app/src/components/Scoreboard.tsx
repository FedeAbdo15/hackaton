import { StyleSheet, Text, View } from 'react-native';
import { formatTime } from '../lib/scoring';
import { PlayerScore } from '../lib/types';
import { Card, colors, medalColor, medalEmoji } from './ui';

export function Scoreboard({ scores }: { scores: PlayerScore[] }) {
  return (
    <View style={{ gap: 8 }}>
      {scores.map((s, i) => (
        <Card key={s.player.id} style={i < 3 ? { borderColor: medalColor(i) } : undefined}>
          <View style={styles.header}>
            <Text style={styles.position}>{medalEmoji(i)}</Text>
            <Text style={styles.name} numberOfLines={1}>{s.player.name}</Text>
            <Text style={[styles.total, { color: medalColor(i) }]}>{s.total} pts</Text>
          </View>
          <View style={styles.details}>
            <Text style={[styles.detail, { color: colors.seeker }]}>🔍 Buscador: {s.seekerPoints}</Text>
            <Text style={[styles.detail, { color: colors.hider }]}>🏴‍☠️ Escondedor: {s.hiderPoints}</Text>
          </View>
          <View style={styles.details}>
            <Text style={styles.stat}>
              Mejor búsqueda: {s.bestSeekSeconds !== undefined ? formatTime(s.bestSeekSeconds) : '--'}
            </Text>
            <Text style={styles.stat}>
              Mejor escondite: {s.bestHideSeconds !== undefined ? formatTime(s.bestHideSeconds) : '--'}
            </Text>
          </View>
        </Card>
      ))}
    </View>
  );
}

const styles = StyleSheet.create({
  header: { flexDirection: 'row', alignItems: 'center', gap: 10 },
  position: { fontSize: 22, minWidth: 30, color: colors.text },
  name: { flex: 1, color: colors.text, fontSize: 18, fontWeight: '700' },
  total: { fontSize: 20, fontWeight: '800' },
  details: { flexDirection: 'row', justifyContent: 'space-between', flexWrap: 'wrap', gap: 4 },
  detail: { fontSize: 14, fontWeight: '600' },
  stat: { color: colors.muted, fontSize: 12 },
});
