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
    // The neuron ID is its slot number in the vector of neurons
    // which is built by the Neurons class at startup
    // Neurons do not keep track of who connects to them
    // the aggregation window is defined in the Neurons class for all neurons
    // STP and LTP are a function of the connection, not the neuron
    // When a neuron cascades it can ditch all signals older than
    // (refractory end - aggregation period) as a newer signal might jus pick up
    // some signal input from the remainder signals.
    // Neuron sends the cascade event to all connections
    // The incomingSignals queue grows and shrinks over time
    // The outgoingSignals is a largely static list of connections
    // constructed when the neuron network is built.
    // Every individual neuron will have both vector queues

    std::vector<signal::Signal> incomingSignals;
    std::vector<connection::Connection> outgoingSignals;
    int32_t oldestSignal;     // dynamically filled from signal queue
    int32_t refractoryEnd;    // dynamically set when cascade happens.

  };
}
#endif
