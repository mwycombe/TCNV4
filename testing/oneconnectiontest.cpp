#include <iostream>
#include <vector>
#include <climits>
#include <cstdint>
#include "Connections.h"
#include "Connection.h"
#include "Signal.h"
#include "SignalRingBuffer.h"
#include "Neurons.h"

int main ()
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
   * A signal is allocated from the srb and delivered to a source neuron.
   * The source neuron immediately cascades and sends a pulse to its target connection.
   * The connection in turn delivers the pulse as a signal to its target neuron.
   * 
   * Success=:
   * The final neuron should receive and enqueue the signal with the correct amplitude and clock value.
   */

    // 75 is the default pools capacity for testing.

   conns::Connections connections = conns::Connections(75); 
   neurons::Neurons neurons = neurons::Neurons(75);
   srb::SignalRingBuffer srb = srb::SignalRingBuffer(75);

   // connect the first connection to the first neuron
   // get ref to first connection
   // get ref to first neuron
   // connection connection to neuron
   // get first signal and poke connection
   // check if signal was delivered to neuron

  int32_t cSlot = conns::currentConnectionSlot;
  int32_t nSlot = neurons::currentNeuronSlot;

   std::cout << "End of oneconnection test..." << std::endl;

  return 0;
}

