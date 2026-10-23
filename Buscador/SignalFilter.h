#pragma once

// Media móvil exponencial (EMA): suaviza el ruido del RSSI sin guardar historial.
class SignalFilter {
public:
  explicit SignalFilter(float alpha) : alpha_(alpha) {}

  float update(int sample) {
    value_ = initialized_ ? alpha_ * sample + (1.0f - alpha_) * value_
                          : static_cast<float>(sample);
    initialized_ = true;
    return value_;
  }

  float value() const { return value_; }
  bool hasValue() const { return initialized_; }

  void reset() {
    value_ = 0.0f;
    initialized_ = false;
  }

private:
  const float alpha_;
  float value_ = 0.0f;
  bool initialized_ = false;
};
