#pragma once

#include <Arduino.h>

enum class Proximity : uint8_t {
  SinSenal,
  Frio,
  Tibio,
  Caliente,
  Encontrado
};

inline const char* toString(Proximity proximity) {
  switch (proximity) {
    case Proximity::SinSenal:   return "SIN SENAL";
    case Proximity::Frio:       return "FRIO";
    case Proximity::Tibio:      return "TIBIO";
    case Proximity::Caliente:   return "CALIENTE";
    case Proximity::Encontrado: return "ENCONTRADO";
  }
  return "?";
}
