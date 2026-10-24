import { ReactNode } from 'react';
import { Pressable, StyleSheet, Text, View, ViewStyle } from 'react-native';

export const colors = {
  bg: '#0f1720',
  card: '#1b2633',
  border: '#2c3a4a',
  text: '#f1f5f9',
  muted: '#94a3b8',
  gold: '#f5c542',
  silver: '#cbd5e1',
  bronze: '#d08b4c',
  accent: '#f5c542',
  danger: '#ef4444',
  seeker: '#38bdf8',
  hider: '#a78bfa',
};

export function Button({
  title, onPress, variant = 'primary', disabled, style,
}: {
  title: string;
  onPress: () => void;
  variant?: 'primary' | 'secondary' | 'danger';
  disabled?: boolean;
  style?: ViewStyle;
}) {
  return (
    <Pressable
      onPress={onPress}
      disabled={disabled}
      style={({ pressed }) => [
        styles.button,
        variant === 'primary' && styles.buttonPrimary,
        variant === 'secondary' && styles.buttonSecondary,
        variant === 'danger' && styles.buttonDanger,
        (pressed || disabled) && { opacity: disabled ? 0.4 : 0.75 },
        style,
      ]}>
      <Text style={[styles.buttonText, variant === 'primary' && { color: colors.bg }]}>{title}</Text>
    </Pressable>
  );
}

export function Card({ children, style }: { children: ReactNode; style?: ViewStyle }) {
  return <View style={[styles.card, style]}>{children}</View>;
}

export const medalColor = (index: number): string =>
  [colors.gold, colors.silver, colors.bronze][index] ?? colors.muted;

export const medalEmoji = (index: number): string => ['🥇', '🥈', '🥉'][index] ?? `${index + 1}.`;

export const ui = StyleSheet.create({
  screen: { flex: 1, backgroundColor: colors.bg },
  content: { padding: 16, gap: 12 },
  title: { color: colors.text, fontSize: 24, fontWeight: '800' },
  subtitle: { color: colors.muted, fontSize: 14 },
  label: { color: colors.muted, fontSize: 13, fontWeight: '600', textTransform: 'uppercase' },
  text: { color: colors.text, fontSize: 16 },
  input: {
    backgroundColor: colors.card,
    borderColor: colors.border,
    borderWidth: 1,
    borderRadius: 10,
    color: colors.text,
    fontSize: 16,
    paddingHorizontal: 12,
    paddingVertical: 10,
  },
  row: { flexDirection: 'row', alignItems: 'center', gap: 8 },
});

const styles = StyleSheet.create({
  button: { borderRadius: 12, paddingVertical: 14, paddingHorizontal: 16, alignItems: 'center' },
  buttonPrimary: { backgroundColor: colors.accent },
  buttonSecondary: { backgroundColor: colors.card, borderWidth: 1, borderColor: colors.border },
  buttonDanger: { backgroundColor: colors.danger },
  buttonText: { color: colors.text, fontSize: 16, fontWeight: '700' },
  card: {
    backgroundColor: colors.card,
    borderColor: colors.border,
    borderWidth: 1,
    borderRadius: 14,
    padding: 14,
    gap: 6,
  },
});
