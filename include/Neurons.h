#ifndef NEURONS_H_INCLUDED
#define NEURONS_H_INCLUDED
#include <cstddef>
#include <vector>
#include "SignalRingBuffer.h"
#include "Neuron.h"
#include "Tcnconstants.h"
#include "Connections.h"
#include "Signal.h"

/** POP
    All neuron references exchanged with outside classes are done using
    the neuron slot number. This avoids any issues if neurons or the queue
    of neurons and signals are re-provisioned on the heap.
    Signals are pushed onto the neuron signalQueue in the order they arise, which
    is not related to the future clock time when they will become active.
    Thus, all tests for signal aggregation and next lowest even clock must scan
    the entire signal queue. The was determined to be a much better performer
    than sorting the signal queue vector by future clock value every time a new
    signal is added to the neuron's signal queue.
    Neuron keeps track of the oldest signal it has in queue as that's when it next needs
    to scan the incoming queue.
    Signals stay on the queue past their processing time until the aggregation decay
    window has elapsed.
*/

using namespace tcnconstants;

/**
 * @brief Holder of neuron pool
 * Neuron pool is a public vector allocated on the heap.
 * For the sake of speed neurons can be accessed directly without using a getter/setter
 * 
 * @param   size of neuron pool
 * 
 * @return  next neuron slot for building neuron network
 *          not used after network has been built
 * 
 */

namespace neurons
{
    std::int32_t currentNeuronSlot {INT32_MIN}; // forces start @ 0
    std::int32_t neuronPoolCapacity {};
    class Neurons {
        /**
         * The Neurons class is responsible for creating the pool of neurons which
         * is computed in the TCNConstants.h header.
         * 
         * When neurons cascade they go through the list of connections and engage
         * each connection to procure and generate and appropriate signal.
         */
        // make everything public for speed of access.
        // public:
        //     static int * refractoryEndOrigin;
        //     static v_i_ptr * signalQueueOrigin; // vectors of enqueued signals
        //     static v_i_ptr * dependentsOrigin;  // vectors of target neurons
        //     static  int * necvOrigin;          // next event clock value from signal queue on Neuron
        //     static vector<int>::const_iterator int_citer;
        //     static  int signalClock;        // used to speed up signal scanning// make everything public for speed of access.
        public:

            Neurons (std::int32_t poolSize)
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

            // make this public so all can access it
            std::vector<neuron::Neuron> m_neuronPool;
    };

} // end neurons namespace

#endif // NEURONS_H_INCLUDED
