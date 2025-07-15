#include <iostream>
#include <vector>
#include <climits>
#include <cstdint>
#include "Connections.h"
#include "Connection.h"
#include "Signal.h"
#include "SignalRingBuffer.h"
#include "Neurons.h"
#include "Neuron.h"

extern std::vector<connection::Connection> m_connPool;
extern int32_t currentConnectionSlot;
extern int32_t connectionPoolCapacity;

/* other externs used by connection*/
extern std::vector<signal::Signal> m_srb;
extern int32_t currentSignalSlot;
extern int32_t signalBufferCapacity;

extern std::vector<neuron::Neuron> m_neuronPool;
extern int32_t currentNeuronSlot;
extern int32_t neuronPoolCapacity;



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


   std::cout << "Connections pool:= " << std::to_string(connectionPoolCapacity) << '\n';
   std::cout << "Neuron pool: " << std::to_string(neuronPoolCapacity) << '\n';
   std::cout << "SRB pool: " << std::to_string(signalBufferCapacity) << '\n';


   // connect the first connection to the first neuron
   // get ref to first connection
   // get ref to first neuron
   // connection connection to neuron
   // get first signal and poke connection
   // check if signal was delivered to neuron


  connection::Connection connRef;
  neuron::Neuron neuronRef;
  signal::Signal signalRef;



  // pseudo slotAllocationRoutines
  connRef = m_connPool[++currentConnectionSlot];
  neuronRef = m_neuronPool[++currentNeuronSlot];

  // SRB is different as it can wrap
  
  int32_t nextSignalSlot;

  if (currentSignalSlot >= signalBufferCapacity) {
    currentSignalSlot = 0;
    nextSignalSlot = 0;
  }
  else { 
    nextSignalSlot = ++currentSignalSlot; 
  }
  signalRef = m_srb[nextSignalSlot];

  // all three elements are now addressed by a Ref from their respective pools


  std::cout << "End of oneconnection test..." << std::endl;

  return 0;
}

