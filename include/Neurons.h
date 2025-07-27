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
std::int32_t currentNeuronSlot{-1};         // forces allocation to start @0. 
std::int32_t neuronPoolCapacity{};          // filled in by constructor
std::int32_t globalNextEvent{0};            // tracks the next event clock tick to process

// other pools
extern std::vector<signal::Signal> m_srb;               // common signal ring buffer pool
extern std::vector<connection::Connection> m_connPool;  // common connection pool

// external globlals
std::int32_t masterClock{0};

std::int32_t cascadeAccumulator {0};             // used to accumulate effective neuron signal.
std::int32_t aggregationDistance{0};             // used to check signal time for aggregation.
std::int32_t neuronBeingProcessed{0};            // easiest way to track neuronID (aka number)
std::int32_t youngestSignal{0};                  // used for purging incoming signals

/** POP
    All neuron references exchanged with outside classes are done using
    the neuron slot number - aka it's vector element index number. This avoids any issues if neurons or the queue
    of neurons and signals are re-provisioned on the heap.
    Signals are pushed onto the neuron signalQueue in the order they arise, which
    is not related to the future clock time when they will become active.
    Thus, all tests for signal aggregation and next lowest event clock must scan
    the entire neuron signal queue. The was determined to be a much better performer
    than sorting the signal queue vector by future clock value every time a new
    signal is added to the neuron's signal queue as sorting a vector involves multiple passes.
    Neuron keeps track of the oldest signal it has in queue as that's when it next needs
    to scan the incoming queue.
    Signals stay on the queue past their processing time until the aggregation decay
    window has elapsed when they become eligible for purging.
    Neurons are just dumb passive structs. Any activity that results from their condition is 
    handled by the processing routines in the Neurons class as it scans the neuron pool.
    The Neuron struct cannot afford the luxury of having any active components because of the memory
    that each pointer would take.
    The Connections class generates the signals on behalf of neurons that cascade but scanning
    the neuron's outGoingSignals vector where the definition for each connection for this neuron resides.
    It is the Connection that determines the actionTime for the signal plus the size of the signal
    to be created. Connections keep track of last signal time and the appropriate handling of STP and
    LTP signal augmentation.

    CRUCIAL EFFICIENCY INSIGHT: Neurons do not need to react to signals being enqueued as they will 
    always be an event in the future. We scan all neurons and their signal queues at the current 
    clock time, looking for aggregations that cause a cascade. Cascades cause a signal to be sent 
    to one or more connections for future delivery or ignoring if target is refractory.
    For efficiency of processing it is essential that we do not determine if a neuron has to react the 
    second it receives a new signal. Even if the signal event were the next clock tick the incoming signal
    enqueued for a neuron can never change the status of the neuron.
    This fact allows multiple threads to scan the network in parallel looking for events to handle. If any thread
    finds an event that causes a cascade it will never affect any of the other threads that are scanning using 
    the same clock time as cascade events cause the enqueing of a signal using a connection for a future
    time beyond the current clock tick.
    And the consequence of this ability to scan in parallel means that the network can be distributed across multiple
    cooperating computers, each with their own portion of the network, all working using the same clock time.

    EVENTs & CLOCK TIMES: The masterClock starts a zero. Older signals have smaller clock times then younger signals. When
    signals are enqueued on to a neuron they have an actionTime which represents the time when a signal is expected
    to arrive at the neuron in the future. Although a neuron is refractory, it is legitimate to enqueue signals beyond the
    refractory period as they will arrive in the future when the neuron is once again in a condition to process signals.
    Signals are not processed when they are enqueued but they are processed when the masterClock gets to their action time.
    The neuron and global nextEvent represents the clock time for the oldest signal that needs to be processed beyond any neuron's
    refractory period.

    NEURON & GLOBAL NEXT EVENT: This is among the trickiest of algorithms to get right, be efficient, and avoid
    unnecessary scans of the neuron pool.
    All neurons start with their nextEvent set to INT32_MAX, which means they will not process before the end of time.
    When a signal is generated by a connection the signal actionTime is the  masterClock + temporalDistanceToTarget
    as all Connection times are relative from the originator to the target. Upon signal generation the algorithm
    is simple; set the neuron nextEvent to the lower of it's current value or the signal actionTime as computed.
    The signal actionTime will always be greater than the masterClock and may or may not be before some prior
    signal enqueued on the target neuron. Capturing the lowest neuron nextEvent is always done at signal generation
    time and relies on the proto neurons starting out with their nextEvent set to MAX. 
    Now signals are only created during the scan of the neurons looking for signals that are ready to be examined.
    This means that the neuron scan alread set the globalNextEvnt to MAX ready to catch the next lowest neuron
    nextEvent so signal creation can always compare and update the globlNextEvent as well as the target neuron
    nextEvent even if the target neuron is prior to the neuron currently being processed. Changing the globlNextEvent
    to a lower clock value will never take it backwards in time and will not affect the processing of the current
    neuron pool scan.
    When TCN starts the masterClock is set to zero (0) and a scan of all neurons is initiated. The globalNextEvent
    is set to MAX at the start of neuron pool scan. At each neuron the test is made: is the neuron nextEvent the 
    same as the masterClock? This must be the neuron nextEvent clock that is currently being processed. 
    If not then is the neuron nextEvent less than the current globalNextEvent? If so then capture the lower neuron
    nextEvent value. 
    If the current neuron nextEvent is used to set the globalNextEvent then this neuron creates signals that are
    even earlier it doesn't matter as the signal creation from the current neuron uses the connection signal 
    generation which checks for any lower future signal actionTime values and updates the neuron nextEvent and the
    globalNextEvent values, neither of which affect the current neuron being processed.
    Their is an edge case when the neuron goes refractory. As this lasts longer than the agreggation window for
    enqueued signal that neuron should purge its signal queue plus reset its nextEvent to max so the next signal
    generated to that neuron will once again set the neuron nextEvent time. however this could lose enqueue events beyond 
    the refractory period if there is not anothe signal delivered to this neuron.

    WHY WE DON'T SORT QUEUES: To do a sort always requires at least one, if not more passes through the queue 
    elements to complete the sort. If we do sort every time we add something to a queue of signals we would 
    scan the whole queue multiple times, once for every time we added a signal.
    Therefore we just scan the queue of signals once when there are any entries in the signal queue and we are
    scanning the all the neurons for work and we record the next event time for the oldest signal (smallest
    clock time) for any of the signals on the neurons signal queue. 
    If nothing else changes this will become the next clock event time when we need to scan this neuron for work.

    The worst thing we could do is update the next event clock time and sort the signal queue every time a signal
    was added to a neuron. The whole scheme depends on the fact that new signals can never be enqued for any clock
    time prior to the one we are currently processing, even for neurons that we have previously passed over.

    It is conceivable that a signal could be enqueued to a prior neuron that would lower that neuron's event clock
    and the overall global event clock. The solution to that is whenever a signal is generated and enqueued, 
    we update the global event clock to reflect the lowest signal future clock time created.

    As an alternative we could never bother to scan the neuron incoming signal queue and just rely on every signal
    updating the global clock event and not bother to scan each neuron for a separate next event and just look
    through the neuron signal queues every time we scan all the neurons.

*   July 2025: Cannot create vector of refs - have to use pointers to avoid making
*   copies of neurons into the vector.

    July 2025: It got worse. Refs cannot be reseated and pointers to vector elements are subject to vector
    being moved in storage, without notification, causing the pointer to become compromised.
    So, back to using indexes into the pools, the presumption being that even if the vectors are moved around
    the compiler ensures the indexes always point to the current correct location in the vector no matter
    where they have been located in storage. Also, indexes are small and fast - 32 bits vs. 64 bits for pointers.

    July 2025: When we are processing signals they are handled at the current masterClock time that was set from
    globalNextEvent - which was gleaned from all the signals. 



/**
 * @brief Holder of neuron pool
 * Neuron pool is a public vector allocated on the heap.
 * For the sake of speed neurons can be accessed directly without using a getter/setter
 * 
 * @details Slot allocation is a pseudo-allocation rather than a call to any member function
 * in the Neuron class - this is done for speed and uses direct access to the global neuron pool
 * and associate control variables. Pseudo-allocation uses a global currentNeuronIndex to keep track
 * of where we are in the pool and dispenses ++currentNeuronIndex when asked to allocate a neuron.
 * This allocation of a vector element index should be vector storage location invariant.
 * 
 * Upon a scan request, goes through every neuron in the network, skipping those that are refractory,
 * those that are proto empty copies from neuron pool building at start-up and those that have a next 
 * signal processing time that is in the past.
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
    class Neurons 
    {

        // create Connections & Neurons for reference so can can call public routines
        
        conns::Connections connObject;
        srb::SignalRingBuffer signalObject;
        std::int32_t signalRequestor {};         // process requesting node for nextEvent
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
        //     static  std::int32_t signalClock;        // used to speed up signal scanning// make everything public for speed of access.
        public:

            Neurons(std::int32_t poolSize)
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
            
                std::cout << "\n>>>>>>>>>>>>>>NEURONS POOL SETUP\n\n";

                // create  class objects to support printing
                conns::Connections connObj = conns::Connections();
                srb::SignalRingBuffer srbObj = srb::SignalRingBuffer();


                // std::vector<signal::Signal*> incomingSignals;
                // incomingSignals.reserve(1);
                
                // incoming.push_back(&emptySignal);    // force vector allocation with a ptr

                // std::vector<connection::Connection*> outgoingSignals;
                // outgoingSignals.reserve(1);

                std::vector<std::int32_t> incomingSignals;
                std::vector<std::int32_t> outgoingSignals;

                incomingSignals.reserve(1);
                outgoingSignals.reserve(1);

                incomingSignals.push_back(0);   // point to proto signal
                outgoingSignals.push_back(0);   // point to proto connection

                // std::cout << "\nEmpty signal vectors created\n";

                // Both signal vectors are now initialized to 0, pointing to the proto element in the pool

                // std::cout << "Empty entries for neuron allocation:\n\n";

                // See what's been pushed into signal vectors
                std::cout << "\nWhat was pushed onto proto vectors?\n";
                srbObj.printSignalFromIndex(incomingSignals[0]);
                connObj.printConnectionFromIndex(outgoingSignals[0]);


                // std::cout << "\nincoming[0] actionTime:= " << std::to_string(incomingSignals[0]->actionTime);
                // std::cout << "\noutgoing[0] temporalDistance:= " << std::to_string(outgoingSignals[0]->temporalDistanceToTarget) << std::endl;

                std::int32_t refractoryEnd = INT32_MAX;      // These neurons should never process
                std::int32_t nextEvent = INT32_MAX;          // Ensure never yet seen signal clock
                                                        // always greater than any signal to be created or processed.

                // This shouldl populate the struct with the provided variables.
                std::cout << "\nChange how empty neuron is initialized. \n";

                // Much simpler neuron with signal vectors of int32_t's not pointers
                // which point to the appropriate slot in the srb and connpools.
                // No more vector pointer volatility
                neuron::Neuron emptyNeuron{};
                // neuron::Neuron* emptyNeuronPointer = {&emptyNeuron};
                emptyNeuron.incomingSignals = incomingSignals;
                emptyNeuron.outgoingSignals = outgoingSignals;
                emptyNeuron.nextEvent = nextEvent;
                emptyNeuron.refractoryEnd = refractoryEnd;

                // What's in the empty neuron?
                // std::cout << "\nPrint empty neuron\n";

                printNeuronFromRef(emptyNeuron);
                // std::cout << std::endl;
                // std::cout << "\nincoming[0]:= " << std::to_string(incomingSignals[0]->actionTime);
                // std::cout << "\noutgoing[0]:= " << std::to_string(outgoingSignals[0]->temporalDistanceToTarget) << std::endl;

                // std::cout << "\nWhat's in the empty neuron signals?\n";

                // std::cout << "\nFirst empty incoming signal\n";
                // std::cout << "\nFirst empty signal contents:= " << std::to_string(emptyNeuron.incomingSignals[0]);

                // srbObj.printSignalFromIndex(emptyNeuron.incomingSignals[0]);

                // std::cout << "\nFirst outgoing signal\n";
                // connObj.printConnectionFromIndex(emptyNeuron.outgoingSignals[0]);
                
                for (int i = 0; i < m_neuronPool.capacity(); ++i) {
                    m_neuronPool.push_back(emptyNeuron);
                }

                // std::cout << "\nIs empty neuron[0] still good in first neuron?\n";
                // signalObject.printSignalFromIndex(m_neuronPool[0].incomingSignals[0]);
                // std::cout << std::endl;
                // connObject.printConnectionFromIndex(m_neuronPool[0].outgoingSignals[0]);
                // std::cout << std::endl;

                // std::cout << "\nFirst empty neuron signals after building the pool\n";
                
                // std::cout << "\nDon't use ptrs to first neuron in pool\n";

                // std::cout << "\nEmpty neuron[0] signals?\n";
                // std::cout << "\nFirst incoming signal\n";
                // std::cout << "\nFirst incomingSignal index:= " << m_neuronPool[0].incomingSignals[0];
                // srbObj.printSignalFromIndex(m_neuronPool[0].incomingSignals[0]);

                // std::cout << "\nFirst outgoing signal\n";
                // std::cout << "\nFirst outgoingSginal index: } " << m_neuronPool[0].outgoingSignals[0];
                // connObj.printConnectionFromIndex(m_neuronPool[0].outgoingSignals[0]);
                // std::cout << "\nCreated " << std::to_string(poolSize) << " empty neurons.\n" << '\n';

                std::cout << "\n<<<<<<<<<<<<<END NEURON POOL SETUP\n\n";
                
 
            }

            // Default constructor
            Neurons() = default;
        
            ~Neurons()
            {
                ; // allocated vector heap will be freed when they go out of scope.
            }

            void printNeuronFromIndex(std::int32_t nidx)
            {   
                std::cout << "\nNeuron Index:= " << std::to_string(nidx);
                std::cout << "\nincomingSignals size:= " << 
                    std::to_string(m_neuronPool[nidx].incomingSignals.size());
                std::cout << "\noutgoingSignals size:= " <<
                    std::to_string(m_neuronPool[nidx].outgoingSignals.size());
                std::cout << "\nnextEvent:= " <<
                    std::to_string(m_neuronPool[nidx].nextEvent);
                std::cout << "\nrefractoryEnd:= " << std::to_string(m_neuronPool[nidx].refractoryEnd) << '\n';
            }
            void printNeuronFromRef(neuron::Neuron& neuronRef)
            {
                std::cout << "\nNeuron: incomingSignals:= " 
                        << std::to_string(neuronRef.incomingSignals.size())
                        << std::endl;
                std::cout << "outgoingSignals:= " << std::to_string(neuronRef.outgoingSignals.size())
                            << std::endl;
                std::cout << "nextEvent := " << std::to_string(neuronRef.nextEvent)
                            << std::endl;            
                std::cout << "refractoryEnd:= " << std::to_string(neuronRef.refractoryEnd)
                            << std::endl;
            }

            void printNeuronFromPointer(neuron::Neuron* neuronPtr)
            {
                std::cout << "\nNeuron: incomingSignals:= " <<
                        std::to_string(neuronPtr->incomingSignals.size()) <<
                        "\noutgoingSignals:= " << std::to_string(neuronPtr->outgoingSignals.size()) <<
                        "\nrefractoryEnd:= " << std::to_string(neuronPtr->refractoryEnd) <<
                    std::endl;
            }

            void scanNeuronsForSignals()
            {

                // Now can scan the neuron pool looing for any valid signals
                // Have to keep track of the neuron slot being processed as there is
                // no way for the range to let us know where we are

                std::int32_t  neuronBeingProcessed = -1;

                // masterClock = globalNextEvent;        // Always the next clock tick when we are asked to scan neurons.
                globalNextEvent = INT32_MAX;             // This forces capture of some lower clock event


                std::cout << "\n\n...>>>>>>>>STARTING NEURON SCAN...<<<<<<<<\n";

                for (neuron::Neuron nRef : m_neuronPool)
                {
                    //  nRef will be set to a ref to every neuron in the pool
                    //  Conditions to process a neuron when ooking for work:
                    //
                    //  Not refractory
                    //  Entries on the incomingSignals queue
                    //    Reasons to skip a neuron:
                    //    (incomingSignals isEmpty) OR 
                    //    (incomingSignals length(1) AND firstSignal actionTime is INT32_MAX) OR
                    //    (neuron nextEvent > masterClock)
                    //
                    //  NOTE: This second reason means the neuron has never received a signal
                    //        and is still in its original allocation condition which would 
                    //        indicate a neuron from whom connections might be pruned.
                    //  
                    //  First implementation will not bother with STP and LTP - just moving signals
                    //  and testing for cascading.
                    //  
                    //  If a neuron gets scanned for actionable signals and aggregation, by definition
                    //  the neuron's nextEvent must be equal to the current masterClock - otherwise it
                    //  will wait for another pass to be processed.

                    //  track neuronId which is zero based and is vector slot number in m_neuronPool

                    ++neuronBeingProcessed;     // Only way to count slots during a forEach 

                    // Check if the incomingSignals queue should be purged - which is a potentially 
                    // a very expensive operation and must be optimized.

                    
                    
                    // nRef.nextEvent = INT32_MAX;


                    if ( masterClock > nRef.refractoryEnd  && nRef.nextEvent == masterClock)
                    // Only process neurons signal queue when they have exited refractory
                    // There is never anything to be done for a refractory neuron as all incoming
                    // signals were purged when it went refracory and no new signals can enqueue 
                    // while it is refractory. Future signals within the refractory period will not enqueue. 
                    // Signals that are beyond the refractory period can still be enqueued as the have not
                    // arrived.
                    // If the neuron nextEvent isn't the current masterClock there is nothing to do.
                    // Neurons only merit attention when there is a signal due to process at the current
                    // masterClock time.
                    {                    
                        std::cout << "\nProcessing non-refractory neuron:= " << 
                            std::to_string(neuronBeingProcessed) << "\n";

                        std::cout << "\nmasterClock:= " << std::to_string(masterClock);
                        printNeuronFromRef(nRef);
 
                        std::cout << "\nincomingSignals size:= " << std::to_string(nRef.incomingSignals.size());
                        std::cout << "\nactionTime:= " << std::to_string(m_srb[nRef.incomingSignals[0]].actionTime);

                        //  The second test finds any neuron that has never received a signal and is still the way
                        //  it was initialized.
                        if ( ((nRef.incomingSignals.empty()) ||
                                (   nRef.incomingSignals.size() == 1 &&
                                    m_srb[nRef.incomingSignals[0]].actionTime == INT32_MAX))  )
                        {
                            // Skip proto signals or empty incoming queues that got purged
                            std::cout << "\nSkip proto signal\n";   // skip the proto signal
                            // At this point, if it's worth doing, we could reset the signal queue 
                            // to empty using the swap trick.
                            ;
                        }
                        else
                        {
                            // Step through aggregation signals and see if we cascade/
                            // Remember neuron vectors hold index to signal srb slot
                            // Always capture the current nextEvent for the neuron being processed
                            // Leave globalNextEvent alone if it's already lower i.e. older
                            // globalNextEvent = (globalNextEvent <= m_neuronPool[neuronBeingProcessed].nextEvent) ?
                            //     globalNextEvent : m_neuronPool[neuronBeingProcessed].nextEvent;

                            std::cout << "\nStart cascade accumulation....\n";
                            cascadeAccumulator = 0;
                            for (std::int32_t sRef : nRef.incomingSignals)
                            {
                                // Just use the simple signal size for now - without  stp/ltp
                                // We aggregate existing prior signals for aggretation window width
                                // Have to scan the complete signal queue as they are not sorted by time of action

                                std::cout << "\nSignal action time: " << std::to_string(m_srb[sRef].actionTime);

                                // At this point, as we step through all of the incoming signals, we should
                                // be able to capture the next oldest signal beyond the current masterClock
                                //
                                // Only aggregate signals that are <= current master_clock and within the
                                // aggregation window. 
                                // Make sure we skip proto signals with INT_MIN action times.

                                std::cout << "\nMaster clock & actionTime:= " << std::to_string(masterClock) <<
                                        " : " << std::to_string(m_srb[sRef].actionTime);

                                // Processing note: all those that are at the same temporal distance inside
                                // the aggregation window will all receive the same degradation.
                                // Two signals @ -2 are as powerful as four signal @ -4
                                // The test against INT32_MIN is to skip over proto signals
                                // Ownership test is to guard against SRB wrap.

                                if (m_srb[sRef].actionTime > INT32_MIN &&           // skip proto signals
                                    m_srb[sRef].actionTime <= masterClock &&        // skip future signals
                                    m_srb[sRef].actionTime > masterClock - 5 &&     // skip older than aggregation window
                                    m_srb[sRef].owner == neuronBeingProcessed)      // skip signals we don't own
                                {
                                    aggregationDistance = masterClock - m_srb[sRef].actionTime;
                                    std::cout << "\nAggregation distance:= " << std::to_string(aggregationDistance);

                                    switch (aggregationDistance)
                                    {
                                        case -5:    cascadeAccumulator += m_srb[sRef].amplitude / 32;
                                            break;
                                        case -4:    cascadeAccumulator += m_srb[sRef].amplitude / 16;
                                            break;
                                        case -3:    cascadeAccumulator += m_srb[sRef].amplitude / 8;
                                            break;
                                        case -2:    cascadeAccumulator += m_srb[sRef].amplitude / 4;
                                            break;
                                        case -1:    cascadeAccumulator += m_srb[sRef].amplitude / 2;
                                            break;
                                        case 0:     cascadeAccumulator += m_srb[sRef].amplitude;
                                            break;                  
                                    }
                                }
                            }
                            if (cascadeAccumulator >= tconst::cascadeThreshold)
                            {
                                // neuron cascades and broadcasts it's own signal
                                std::cout << "\nNeuron cascades with accumulator:= " << std::to_string(cascadeAccumulator);
                                signalRequestor = connObject.generateOutGoingSignals(neuronBeingProcessed);

                                // Now that neuron has cascaded and generated its signals it should be put
                                // into refractory and have all of its signals purged.
                                nRef.refractoryEnd = tconst::refractoryWidth + masterClock;

                                // This is the right place to examine neurons for purging signals
                                // After the neuron has processed, any signals up thru the aggregation window
                                // will have been used.
                                // There may still be future signals enqueued for later processing after refractory
                                // and these should not be purged.

                                // Purge old signals
                                if (nRef.incomingSignals.size() > tconst::purgeThreshold)
                                {
                                    // Only make the call if there enough signals to bother with
                                    purgeOldSignals(nRef);
                                }
                            }
                        }
                    }
                    else  if (masterClock <= nRef.refractoryEnd)
                    {           // this is the process for refractory neurons
                        purgeOldSignals(nRef);
                    }  
                    else if (nRef.nextEvent == INT32_MAX)
                    {   
                        purgeOldSignals(nRef);  // check for old signals to purge
                    }

                    // Test to purge neuron signal after proceesing before we move on

                    // pseudo purgeOldSignals(neuronBeingProcessed);

                    // This is another candidate to merge inline and avoid an expensive
                    // method call for every neuron processed. Again, done for speed.

                    //     // ++neuronBeingProcessed; // step to next neuron slot
                    //     // This is already incremented at the start of the for-loop

                    // This is the end of processing for each neuron.
                    // Now is the time to scan incomingSignals and update all neuron neuron nextEvent
                    // nextEvent will only bt INT32_MAX if no new signals were enqueued.
                    
                    if (!(nRef.incomingSignals.empty()) && nRef.nextEvent == INT32_MAX)
                    {
                        // We are only going to scan for nextEvent updates if needed
                        for (std::int32_t sRef : nRef.incomingSignals)
                        {
                            // pick lower of current nextEvent or this signal's actionTime
                            // Avoid updating neuron nextEvent where signal actionTime is less than
                            // current masterClock. This edge case can occur when purge has left intact
                            // old signals that might still be eligible for aggregation but are older
                            // than the current masterClock.

                            (nRef.nextEvent < m_srb[sRef].actionTime && 
                                m_srb[sRef].actionTime > masterClock) ?
                                    nRef.nextEvent :    // Leave it alone
                                        nRef.nextEvent = m_srb[sRef].actionTime;
                        }
                    }
                }   // end of neuron forEach loop
                
                std::cout << "\nLast Neuron processed:= " << std::to_string(neuronBeingProcessed) << std::endl;
                std::cout << "End of neuron scan \n";



      
            }

            void purgeOldSignals (neuron::Neuron nRef)
            {

            /**
             * @brief   Purge old signals by dropping their reference from the incomingSignals vector. 
             * They are just dropped and left in the SRB to be reused by someone else.
             * 
             * @details Purging is a relatively expensive operation as the vector is shifted potentially
             * multiple times. We only purge if the number of signals > purgeThreshold but are careful
             * to leave in queue any future signals.
             * The only reason to purge is if the incomingSignal queue gets bloated with old signals. When the 
             * SRB (SignalRingBuffer) wraps, unpurged signals will have the old signal information in them inluding the owner. 
             * It's important that when neurons process the incomingSignal queue they make sure they are the owner
             * of the signal as really old signals that didn't get purged might have been reused after an SRB wrap.
             * 
             * When we purge a neuron during refractory we can purge all signals up to and including the refractory end,
             * including any that would normally fall into the aggregation window as they should never become enqueued or,
             * if they were in the past, they will be unusable while the neuron is refractory. This means the purge
             * routine does not have to reach back to preserve aggregation window signals.
             * 
             * @cond When to purge? If the incomingSignal vector get's larger than the purgeThreshold otherwise
             * just let them lay there for now.
             * The only reason to purge is to save memory in the incomingSignals ref queue. SRB ownership and SRB
             * wrap takes care of reusing the actual signals and neuron signal processing ignores and potentially
             * purges older signals. Otherwise, old signal refs just stay fallow in their respective neuron
             * incomingSignal queues - and they are just indexes not the actual signals themselves, which remain in 
             * the SRB. 
             * 
             * Efficient algorithm: We purge when we are in refractory. Any signals older than current masterClock
             * can be dropped, even those that contributed to the current aggregation.
             * As the incomingSignals vector is not sorted there could be earlier signals enqueued with
             * an actionTime beyond the masterClock so they have to be left intact.
             * There is another simple test that can be made when every neuron is examined in the scan. If the
             * neuron nextEvent is less than the masterClock then the time has passed for this neuron to cascade so
             * we can drop all of the old signals in the incomingSignals queue as they will never again contribute to
             * a clock time thas has passed.
             * This immediate purge can be done in-line.
             */
            
             // Callers should make purge threshold test to avoid unnecessary calls

                std::int32_t oldestSignal = 0;


                // Start with the simplest test:
                if (nRef.nextEvent == INT32_MAX)
                {
                    // This means there are no future signals that have been found so we can purge all
                    std::vector<std::int32_t>().swap(nRef.incomingSignals);
                    return;     // early out
                }

                // Use c++ forEach for cleaner code
                // oldestSignal mean smallest clock time

                for (std::int32_t sRef : nRef.incomingSignals)
                {
                    oldestSignal = (m_srb[sRef].actionTime < oldestSignal &&
                                      m_srb[sRef].actionTime > masterClock) ?
                                        m_srb[sRef].actionTime :
                                            oldestSignal;
                }

                // Upon exit oldestSignal should have the smallest future clock value from incoming signals
                // that is bigger than the current masterClock
                if (oldestSignal < (masterClock - aggregationDistance))
                {
                    // no signals that can ever contribute to cascade within the next clock tick - so purge all
                    // m_neuronPool[neuronBeingProcessed].incomingSignals.clear();
                    // Clear does no reduce capacity; use the swap trick

                    std::vector<std::int32_t>().swap(nRef.incomingSignals);

                    // **** But reinstate incomingSignals[] to point to srb[0] 
                    // So that subsequent pushes always leave incoming[0] pointing to proto signal
                    m_neuronPool[neuronBeingProcessed].incomingSignals.push_back(0);
                }
                else
                {
                    // oldest signal we found in signals is older then current neuron nextEvent
                    nRef.nextEvent = (nRef.nextEvent < oldestSignal) ?
                                        nRef.nextEvent : 
                                            oldestSignal;
                }
            }
        

    };

} // end neurons namespace

#endif // NEURONS_H_INCLUDED
