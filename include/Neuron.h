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
    // the neuron ID is its slot number in the vector of neurons
    // which is built by the Neurons class at startup

    // neurons do not keep track of who connects to them
    // the aggregation window is defined in TCNConstats for all neurons
    // STP and LTP are a function of the connection, not the neuron
    // when a neuron cascades it can ditch all signals older than
    // refractory end - aggregation period
    // neuron sends the cascade event to all connections
    // the incomingSignals queue grows and shrinks over time
    // the outgoingSignals is a largely static list of connections
    // constructed when the neuron network is built.
    // every individual neuron will have both vector queues

    std::vector<signal::Signal> incomingSignals;
    std::vector<connection::Connection> outgoingSignals;
    int32_t oldestSignal;     // dynamically filled from signal queue
    int32_t refractoryEnd;    // dynamically set when cascade happens.

  };
}
#endif
