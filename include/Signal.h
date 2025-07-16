#ifndef SIGNAL_H_INCLUDED
#define SIGNAL_H_INCLUDED
#include <iostream>
#include <cstdint>

#include "TCNConstants.h"

namespace signal
{
  struct Signal {
    std::int32_t actionTime{};     // time when neuron needs to process this signal
    std::int32_t owner{};          // used when srb wraps and signal changes ownership
    std::int16_t amplitude{};      // signal size modified by connection weight at origination.
    std::int16_t testId{};         // used for tracking test signals.
  };
} // end of namespace

#endif
