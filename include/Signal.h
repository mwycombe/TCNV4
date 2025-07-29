#ifndef SIGNAL_H_INCLUDED
#define SIGNAL_H_INCLUDED
#include <iostream>
#include <cstdint>

#include "TCNConstants.h"

/**
 * @brief:  Used to carry a signal event from a cascading neuron to all of its targets.
 * 
 * @details: actionTime: is relative; masterClock is always added to get true clock time.
 * owner: used to make sure signal belongs to the target to guard against srb wrap
 * sourceConnId:  used to allow group strengthening when there is repeat cascades for the target
 * amplitude:     size of the signal as determined by the delivering connection
 * testId:        used for testing to trace things
 */
namespace signal
{
  struct Signal {
    std::int32_t actionTime{};     // time when neuron needs to process this signal
    std::int32_t owner{};          // used when srb wraps and signal changes ownership
    std::int32_t sourceConnId{};   // used for group strengthening for STP/LTP 
    std::int16_t amplitude{};      // signal size modified by connection weight at origination.
    std::int16_t testId{};         // used for tracking test signals.
  };
} // end of namespace

#endif
