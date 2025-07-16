#ifndef CONNECTION_H_DEF
#define CONNECTION_H_DEF
#include <cstdint>

namespace connection

{
  struct Connection {
      int32_t targetNeuronSlot {};            // where to enqueue the signal
      int32_t lastSignalOriginTime {};        // last time a signal was issued on this connection - used for STP/LTP
      int32_t temporalDistanceToTarget{};     // time to deliver to target
      int16_t stpWeight{};                    // current weight for stp amplification
      int16_t ltpWeight{};                    // current weight for ltp amplification
  };

}   // end of connection namespace
#endif