#ifndef NEURONS_H_INCLUDED
#define NEURONS_H_INCLUDED
#include <cstddef>
#include <vector>
#include "SignalRingBuffer.h"
#include "Neuron.h"
#include "Tcnconstants.h"
#include "Connections.h"
#include "Signal.h"


extern std::vector<neuron::Neuron> m_neuronPool;
extern std::int32_t currentNeuronSlot;              // forces start @ 0
extern std::int32_t neuronPoolCapacity;

/** POP
    All neuron references exchanged with outside classes are done using
    the neuron slot number - aka it's vector index number. This avoids any issues if neurons or the queue
    of neurons and signals are re-provisioned on the heap.
    Signals are pushed onto the neuron signalQueue in the order they arise, which
    is not related to the future clock time when they will become active.
    Thus, all tests for signal aggregation and next lowest event clock must scan
    the entire neuron signal queue. The was determined to be a much better performer
    than sorting the signal queue vector by future clock value every time a new
    signal is added to the neuron's signal queue.
    Neuron keeps track of the oldest signal it has in queue as that's when it next needs
    to scan the incoming queue.
    Signals stay on the queue past their processing time until the aggregation decay
    window has elapsed.
    Neurons are just dumb passive structs. Any activity that results from their condition is 
    handled by the processing routines in the Neurons class as it scans the neuron pool.
    The Neuron struct cannot afford the luxury of having any active components because of the memory
    that each pointer would take.

    CRUCIAL EFFICIENCY INSIGHT: Neurons do not need to react to signals being enqueued as they will 
    always be an event in the future. We scan all neurons and their signal queues at the current 
    clock time, looking for aggregations that cause a cascade. Cascades cause a signal to be sent 
    to one or more connections for future delivery or ignoring if target is refractory.
    For efficiency of processing it is essential that we do not determine if a neuron has to react the 
    second it receives a new signal. Even if the signal event were the next clock tick the incoming signal
    enqueued for a neuron can never change the status of the neuron.
    This fact allows multiple threads to scan the network in parallel looking for events to handle. If any thread
    finds and even that causes a cascade it will never affect any of the other threads that are scanning using 
    the same clock time.
    And the consequence of this ability to scan in parallel means that the network can be distributed across multiple
    cooperating computers, each with their own portion of the network, all working using the same clock time.
*/



/**
 * @brief Holder of neuron pool
 * Neuron pool is a public vector allocated on the heap.
 * For the sake of speed neurons can be accessed directly without using a getter/setter
 * 
 * @details Slot allocation a pseudo-allocation rather than a call to any member function
 * in the Neuron class - this is done for speed and uses direct access to the global neuron pool
 * and associate control variables.
 * 
 * @param   size of neuron pool
 * 
 * @return  next neuron slot for building neuron network
 *          not used after network has been built
 * 
 */

namespace neurons
{
    class Neurons {
        /**
         * The Neurons class is responsible for creating the pool of neurons which
         * size is computed in the TCNConstants.h header.
         * 
         * When neurons cascade they go through the list of connections and engage
         * each connection to procure and generate an appropriate signal.
         */
        // make everything public for speed of access.
        // public:
        //     static int * refractoryEndOrigin;
        //     static  int * necvOrigin;          // next event clock value from signal queue on Neuron
        //     static vector<int>::const_iterator int_citer;
        //     static  int32_t signalClock;        // used to speed up signal scanning// make everything public for speed of access.
        public:

        Neurons(int32_t poolSize)
        {

            #ifdef TESTING_MODE
            // just reserve 75 neurons for initial testing
                m_neuronPool.reserve(75);
            #else
                m_neuronPool.reserve(poolSize);
            #endif

            neuronPoolCapacity = m_neuronPool.capacity();

            // init m_neuronPool with empty neurons to allocate heap
            // regardless of the pool size, even if testing, we need 
            // to push some values to trigger heap allocation.

            // create an empty neuron - do NOT use a struct constructor 
            // otherwise every neuron will bear the overhead.

            // first create minimal signal queue and connection queue vectors
            // use impossible values that will never be processed
        
            std::vector<signal::Signal> incoming;
            incoming.reserve(1);
            signal::Signal emptySignal{INT32_MAX,1000,0};
            incoming.push_back(emptySignal);    // force vector allocation

            std::vector<connection::Connection> outgoing;
            outgoing.reserve(1);
            connection::Connection emptyConnection 
            {
                INT32_MAX, INT32_MAX,INT32_MAX,0,0
            };
            outgoing.push_back(emptyConnection);    // force vector allocation


            neuron::Neuron emptyNeuron 
            {
                incoming,        // length one vector
                outgoing,    // length one vector
                INT32_MAX,
                INT32_MAX 
            };

            for (int i = 0; i < m_neuronPool.capacity(); ++i) {
                m_neuronPool.push_back(emptyNeuron);
            }
        }
        
        ~Neurons()
        {
            ; // allocated vector heap will be freed when they go out of scope.
        }

        private:
            int32_t aggregatorWidth{2};     // period when signals must arrive to be counted
            int32_t refractoryWidth{5};     // refractory period width
    };

} // end neurons namespace

#endif // NEURONS_H_INCLUDED
