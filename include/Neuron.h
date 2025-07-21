#ifndef NEURON_H_INCLUDED
#define NEURON_H_INCLUDED
#include <iostream>
#include <cstdint>
#include <vector>

#include "TCNConstants.h"
#include "Signal.h"
#include "Connection.h"

namespace neuron
{
  struct Neuron {

    /**
     * @brief Neurons are passive entities that form the nodes between which
     * connections run. The connections are the active elements. Neurons just
     * cascade when they reach their threshold.
     * 
     * The neuron ID is its slot number in the vector of neurons
     * which is built by the Neurons class at startup
     * Neurons do not keep track of who connects to them
     * the aggregation window is defined in the Neurons class for all neurons
     * STP and LTP are a function of the connection, not the neuron
     * When a neuron cascades it can ditch all signals older than
     * (refractory end - aggregation period) as a newer signal might jus pick up
     * some signal input from the remainder signals.
     * Neuron sends the cascade event to all connections
     * The incomingSignals queue grows and shrinks over time
     * The outgoingSignals is a largely static list of connections
     * constructed when the neuron network is built.
     * Every individual neuron will have both vector queues.
     * There is no need to remember the oldest threshold for a neuron;
     * by definition it is (Now - aggregationWindow) whenever the neuron
     * is processed by the sweep of the TCN.
     * Any signals older than that are no longer relevant.
     * 
     * July 2025 Vectors are changed to pointers to the signals and connections.
     * If we don't so this, every addition to the vectors makes a copy of the 
     * incoming structure. Refs will not work with vector so have to use pointers.
     * 
     * July 2025 incoming/outgoing signal pointers too volatile - use index number
     * into the srb and connpools as the remain resolvable even if vectors move around
     * in the heap.
     */
    



    std::vector<int32_t> incomingSignals;
    std::vector<int32_t> outgoingSignals;
    int32_t refractoryEnd;    // dynamically set when cascade happens.

  };
}
#endif
