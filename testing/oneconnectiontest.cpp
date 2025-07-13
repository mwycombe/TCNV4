#include <iostream>
#include <vector>
#include <climits>
#include <cstdint>
#include "Connections.h"
#include "Signal.h"
#include "SignalRingBuffer.h"
#include "Neurons.h"

int maint ()
{
  /**
   * @brief Test that one connection can request one signal and deliver that signal to the target neuron
   * with the correct clock value set in the signal
   * 
   * @param One input pulse into the connection, as if a neuron signalled it.
   * 
   * @return  Nothing
   * 
   * @throws  Nothing
   * 
   * POP  Each participating element is instrumented using the simple logger.
   * A neuron is created to be the target of the signal.
   * A connection is created and connected to the neuron with a clock distance.
   * A signal is allocated from the srb and delivered to the connection, simulating a neuron cascade.
   * 
   * Success=:
   * The final neuron should receive and enqueue the signal with the correct amplitude and clock value.
   */
  return 0;
}

