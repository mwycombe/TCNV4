#ifdef ndef SIGNAL_H_INCLUDED
#define SIGNAL_H_INCLUDED
#include <iostream>

#include "TCNConstants.h"

namespace signal
{
  struct Signal {
    int32_t actionTime{};     // time when neuron needs to process this signal
    int16_t amplitude{};      // signal size modified by connection weight at origination.
    int16_t unused{};         // available for future use; pads signal to 8 bytes/2 words
  }
} // end of namespace

#endif
