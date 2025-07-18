#ifndef NEURONS_H_INCLUDED
#define NEURONS_H_INCLUDED
#include <cstddef>
#include <vector>
#include "SignalRingBuffer.h"
#include "Neuron.h"
#include "Tcnconstants.h"
#include "Connections.h"
#include "Signal.h"

// definitions are global
std::vector<neuron::Neuron> m_neuronPool{}; // allocated by constructor
std::int32_t currentNeuronSlot{-1};         // forces start @ 0
std::int32_t neuronPoolCapacity{};          // filled in by constructor

// external globlals
extern int32_t masterClock;

int32_t cascadeAccumulator {0};             // used to accumulate effective neuron signal.
int32_t aggregationDistance{0};             // used to check signal time for aggregation.
int32_t neuronBeingProcessed{0};            // easiest way to track neuronID (aka number)
int32_t youngestSignal{0};                  // used for purging incoming signals

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

* July 2025: Cannot create vector of refs - have to use pointers to avoid making
* copies of neurons into the vector.
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

namespace tconst = tcnconstants;
namespace neurons
{
    class Neurons {

        // create Connections & Neurons for reference  so can can call public routines
        
        conns::Connections connObject;
        srb::SignalRingBuffer signalObject;

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
            
                std::vector<signal::Signal*> incoming;
                incoming.reserve(1);
                
                signal::Signal emptySignal{INT32_MAX,1000,0};
                incoming.push_back(&emptySignal);    // force vector allocation with a ptr

                std::vector<connection::Connection*> outgoing;
                outgoing.reserve(1);

                connection::Connection emptyConnection 
                {
                    INT32_MAX, INT32_MAX,INT32_MAX,0,0
                };
                outgoing.push_back(&emptyConnection);   // Force vector allocation wih a ptr

                int32_t refractoryEnd = INT32_MAX;      // They should never process

                neuron::Neuron emptyNeuron 
                {
                    incoming,       // length one vector
                    outgoing,       // length one vector
                    refractoryEnd   // refractory end is now - neuron will process and enqueue
                                    // set to some future clock when neuron cascades
                };

                for (int i = 0; i < m_neuronPool.capacity(); ++i) {
                    m_neuronPool.push_back(emptyNeuron);
                }
            }

            // Default constructor
            Neurons() = default;
        
            ~Neurons()
            {
                ; // allocated vector heap will be freed when they go out of scope.
            }

            void printNeuron(neuron::Neuron& neuronRef)
            {
                std::cout << "Neuron: incomingSignals:= " 
                        << std::to_string(neuronRef.incomingSignals.capacity())
                        << std::endl;
                std::cout << "outgoingSignals:= " << std::to_string(neuronRef.outgoingSignals.capacity())
                            << std::endl;
                std::cout << "refractoryEnd:= " << std::to_string(neuronRef.refractoryEnd)
                            << std::endl;
            }

            void scanNeuronsForSignals()
            {
                // Now can scan the neuron pool looing for any valid signals
                // Have to keep track of the neuron slot being processed as there is
                // no way for the range to let us know where we are

                neuronBeingProcessed = -1;

                std::cout << "Starting neuron scan...\n";

                for (neuron::Neuron nRef : m_neuronPool)
                {
                    //  nRef will be set to a ref to every neuron in the pool
                    //  Conditions to process a neuron when ooking for work:
                    //
                    //  Not refractory
                    //  Entries on the incomingSignals queue
                    //    Reasons to skip a neuron:
                    //    (incomingSignals isEmpty) OR 
                    //    (incomingSignals length(1) AND firstSignal actionTime is INT32_MAX)
                    //
                    //  NOTE: This second reason means the neuron has never received a signal
                    //        and is still in its original allocation condition which would 
                    //        indicate a neuron from whom connections might be pruned.
                    //  
                    //  First implementation will not bother with STP and LTP - just moving signals
                    //  and testing for cascading.
                    //  

                    //  track neuronId which is zero based and is vector slot number in m_neuronPool

                    ++neuronBeingProcessed;

                    // std::cout << "Scanning neuron:= " << std::to_string(neuronBeingProcessed) << "\n";

                    nRef = m_neuronPool[neuronBeingProcessed];

                    if ( masterClock > nRef.refractoryEnd )
                    // only process neurons when they have exited refractory
                    {                    
                        std::cout << " Processing non-refractory neuron:= " << std::to_string(neuronBeingProcessed) << "\n";
                        if ( ((nRef.incomingSignals.empty()) ||
                                ( nRef.incomingSignals.size() == 1 && 
                                    nRef.incomingSignals[0]->actionTime == INT32_MAX))  )
                        {
                            ;   // skip the neuron
                        }
                        else
                        {
                            // Step through the incoming signals and broadcast them
                            // Remember neuron vectors hold pointer to signal
                            cascadeAccumulator = 0;
                            for (signal::Signal* sRef : nRef.incomingSignals)
                            {
                                // Just use the simple signal size for now - without  stp/ltp
                                // We aggregate existing prior signals for aggretation window width
                                // Have to scan the complete signal queue as they are not sorted by time of action
                                aggregationDistance = masterClock - sRef->actionTime;
                                switch (aggregationDistance)
                                {
                                    case -5:    cascadeAccumulator += sRef->amplitude / 32;
                                        break;
                                    case -4:    cascadeAccumulator += sRef->amplitude / 16;
                                        break;
                                    case -3:    cascadeAccumulator += sRef->amplitude / 8;
                                        break;
                                    case -2:    cascadeAccumulator += sRef->amplitude / 4;
                                        break;
                                    case -1:    cascadeAccumulator += sRef->amplitude / 2;
                                        break;
                                    case 0:     cascadeAccumulator += sRef->amplitude;
                                        break;                  
                                }
                            }
                            if (cascadeAccumulator >= tconst::cascadeThreshold)
                            {
                                // neuron cascades and broadcasts it's own signal
                                connObject.generateOutGoingSignals(neuronBeingProcessed);
                            }
                        }
                    }

                    // Test to purge neuron signal after proceesing

                    // pseudo purgeOldSignals(neuronBeingProcessed);

                    // This is another candidate to merge inline and avoid an expensive
                    // method call for every neuron processed. Again, done for speed.
                    {
                        // Only purger if there is enough signals to bother with
                        if (m_neuronPool[neuronBeingProcessed].incomingSignals.size() > tconst::purgeThreshold)
                        {
                            youngestSignal = 0;     // youngest is the signal wih the biggest clock value

                            for (int32_t i = 0; i < m_neuronPool[neuronBeingProcessed].incomingSignals.size(); ++i)
                            {
                                // Capture the largest clock value we can find on the incoming queue of the neuron
                                youngestSignal = 
                                        (m_neuronPool[neuronBeingProcessed].incomingSignals[i]->actionTime > youngestSignal) ? 
                                        m_neuronPool[neuronBeingProcessed].incomingSignals[i]->actionTime : youngestSignal;
                            }

                            if (youngestSignal < (masterClock - aggregationDistance))
                            {
                                // no signals that can ever contribute to cascade - so purge all
                                m_neuronPool[neuronBeingProcessed].incomingSignals.clear();
                            }
                        }
                    }
                    ++neuronBeingProcessed; // step to next neuron slot
                }
                std::cout << "Last Neuron processed:= " << std::to_string(neuronBeingProcessed) << std::endl;
                std::cout << "End of neuron scan \n";
            }





            void purgeOldSignals (int32_t neuronBeingProcessed)

            /**
             * @brief   Purge old signals by dropping their reference from the incomingSignals vector. 
             * They are just dropped and left in the SRB to be reused by someone else.
             * 
             * @details Purging is a relatively expensive operation as the vector is shifted potentially
             * multiple times. We only purge if the number of stale signals > purgeThreshold but are careful
             * to leave in queue any signals that might contribute to aggregate window.
             * The only reason to purge is if the incomingSignal queue gets bloated with old signals. When the 
             * SRB (SignalRingBuffer) wraps, unpurged signals will have the old signal information in them inluding the owner. 
             * It's important that when neurons process the incomingSignal queue they make sure they are the owner
             * of the signal as really old signals that didn't get purged might have been reused after an SRB wrap.
             * 
             * @cond When to purge? If the incomingSignal vector get's larger than the purgeThreshold and the 
             * signals are older than (now - aggretationThreshold) - which is the maximum time aggregation of an
             * incoming signal can reach back in time to combine signals.
             * The only reason to purge is to save memory in the incomingSignals ref queue. SRB ownership and SRB
             * wrap takes care of reusing the actual signals and neuron signal processing ignores and potentially
             * purges older signals. Otherwise, old signal refs just stay fallow in their respective neuron
             * incomingSignal queues - and they are just refs, not the actual signals themselves, which remain in 
             * the SRB. 
             * 
             * Efficient algorithm: Check for any signals that are still in the aggregation window.
             * If there are none then clear the vector - the quickest cheapest way to purge signals.
             */
            {
                if (m_neuronPool[neuronBeingProcessed].incomingSignals.size() > tconst::purgeThreshold)
                {
                    youngestSignal = 0;
                    for (int32_t i = 0; i<m_neuronPool[neuronBeingProcessed].incomingSignals.size(); ++i)
                    {
                        (m_neuronPool[neuronBeingProcessed].incomingSignals[i]->actionTime > youngestSignal) ? 
                                m_neuronPool[neuronBeingProcessed].incomingSignals[i]->actionTime : youngestSignal;
                    }

                    if (youngestSignal > (masterClock - aggregationDistance))
                    {
                        // no signals that can ever contribute to cascade - so purge all
                        m_neuronPool[neuronBeingProcessed].incomingSignals.clear();
                    }
                }
            }
        

    };

} // end neurons namespace

#endif // NEURONS_H_INCLUDED
