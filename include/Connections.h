#ifndef CONNECTIONS_H_INCLUDED
#define CONNECTIONS_H_INCLUDED
#include <iostream>
#include <vector>
#include "TCNConstants.h"
#include "Connection.h"
#include "Neuron.h"
#include "SignalRingBuffer.h"

// make this extern global so all can access it.

std::vector<connection::Connection> m_connPool{};
int32_t currentConnectionSlot{0};  // allocation always returns ++currentConnectionSlot - 0 is default proto
int32_t connectionPoolCapacity{};   // filled in by constructor

// other globals used by connections

extern int32_t globalNextEvent;       // Definef in Neurons.h/cpp

extern std::vector<neuron::Neuron> m_neuronPool;
extern int32_t currentNeuronSlot; // forces start @ 0
extern int32_t neuronPoolCapacity;

extern int32_t currentSignalSlot;
extern int32_t signalBufferCapacity;
extern std::vector<signal::Signal> m_srb;

extern int32_t masterClock;

namespace tconst = tcnconstants;
namespace conns

{
    signal::Signal signalRef;
    int32_t targetNeuronId;     // used when enqueueing new signals
    int32_t neuronSignaled;     // used for tracing which nodes get a connection signal
    int32_t signalEventTime;    // used for nextEvent tracking when generating signals

    srb::SignalRingBuffer srbObj = srb::SignalRingBuffer();     // used for printouts
    /**
    * 
    * @brief   Wherever possible all access and update functions for the connections
    * will be hand-crafted using direct access to pools of dependents and ref paramaters
    * to provide optimum speed.
    * Pseudo function definitions will be used to indicate the intent of the hand-crafted code.
    */
    
    /*  Connections:
    The connection object is associated with the signaling neuron.
    It records which neurons are dependent and retains the connection parameters
    This object provides the following capabilities:
    1. Slot number of the target neuron.
    2. Clock time of any new signal sent to the target neuron
    3. Learning reinforcement amplification for STP
    4. Temporal distance to target neuron for calc when signal will reach there
    5. Aging of reinforcement amplification
    6. Learning reinforcement amplification for LTP = future
    7. Aging of reinforcement amplification
    */
    /*  Connections:
    The connection object is associated with the signaling neuron.
    It records which neurons are its target and retains the connection parameters
    This object provides the following capabilities:
    1. Address of the target neuron (aka Neuron slot number)    4 bytes
    2. Clock time of any new signal sent to target neuron       4 bytes
    3. Temporal distance to target neuron                       4 bytes
    4. Learning reinforcement amplification for STP             2 bytes
    5. Aging of STP reinforcement amplification                 0 bytes compute on next signal
    6. Learning reinforcement amplification for LTP             2 bytes
    7. Aging of LTP reinforcement amplification                 0 bytes compute on next signal
    .
    
    We don't need a sourceID because that's where the connection is stored
    and target neuron never cares about the signal origin.
    
    NOTE: Because connection objects are so numerous they must be allocated on the heap.
    Otherwise they will overrun the stack.
    
    * July 2025: Cannot create vector of refs - have to use pointers to avoid making
    * copies of connections into the vector.
    
    */
    
    // define some class level variables for reuse when generating signals
    // avoids local scope build and destroy
    
    
    class Connections
    {
        
        public:
        // static short   *stp_accumulator_origin;        //stp accumulated level
        // static short   *ltp_accumulator_origin;        //ltp accumulated level
        // static int     *last_signal_clock_origin;      //used to determine how much STP & LTP decay has occurred
        // static int      next_connection_slot;          // used to allocate connections slots during
        int32_t nextSignalSlot;             
        
        // during building for the network
        // The signal size will be modified by STP and LTP and any other memory and enhancement actions
        
        // Class constructor
        Connections(std::int32_t connectionPoolSize)
        {
            #ifdef TESTING_MODE
            m_connPool.reserve(75);
            #else
            m_connPool.reserve(connectionPoolSize);
            #endif
            
            connectionPoolCapacity = m_connPool.capacity();
            
            // now fill the vector with 'blank' connection entries
            // none of the values are valid
            
            connection::Connection blankConnection {
                -1, -1, -1, -1, -1
            };
            for (int i = 0; i < connectionPoolCapacity; ++i){
                m_connPool.push_back(blankConnection);
            }

            // std::cout << "\nCreated " << std::to_string(connectionPoolCapacity) << " empty connections\n";
            
        }
        
        Connections() = default;
        
        // static int32_t     connection_count;    
        
        void printConnectionFromIndex(int32_t cidx)
        {
            std::cout << "\nConnection Index:= " << std::to_string(cidx) << '\n';
            std::cout << "Target Neuron:= " << std::to_string(m_connPool[cidx].targetNeuronSlot);
            std::cout << "\nTemporalDistance:= "<< std::to_string(m_connPool[cidx].temporalDistanceToTarget);
            std::cout << "\nLast Signal:= " << std::to_string(m_connPool[cidx].lastSignalOriginTime);
            std::cout << "\nSTP Weight:= " << std::to_string(m_connPool[cidx].stpWeight);
            std::cout << "\nLRP Weight:= " << std::to_string(m_connPool[cidx].ltpWeight) << '\n';
        }
        // number of connections created
        // static int     get_target_neuron(int);          // return numer of target neuron
        // static int     get_temporal_distance(int);      // return temporal distance
        // static short   get_signal_size(int);            // get size of signal for connection
        // static short   get_stp_accumulator(int);        // return stp accumulator value
        // static short   get_ltp_accumulator(int);        // return ltp accumulator value
        // static void    set_target_neuron(int, int);     // set target neuron number for connection
        // static void    set_temporal_distance(int, int); // set the temporal distance from source to target for a connection
        // static void    set_signal_size(int, short);     // set the size of the signal for a connection
        // static void    set_stp_accumulator(int, short); // set the stp accumulator value
        // static void    set_ltp_accumulator(int, short); // set the ltp accumulator value
        // static int     * getNeuronsOrigin();            // return address of neuron pointer array
        // static int     * getTemporalDistanceOrigin();   // return address of temporal distance array
        // static short   * getSignalSizeOrigin();         // return address of signal size array
        // static short   * getStpOrigin();                // return address of stp accumulator array
        // static short   * getLtpOrigin();               // return address of ltp accumulator array
        // static short    apply_stp(int, short);
        // static short    apply_ltp(int, short);
        // static int     * getLastClockOrigin();          // return address of last clock array
        
        void printConnectionFromRef (connection::Connection &connRef)
        {
            std::cout << "Connection: Neuron:= " << std::to_string(connRef.targetNeuronSlot);
            std::cout << " TemporalDistance:= " << std::to_string(connRef.temporalDistanceToTarget) << std::endl;
            std::cout << "Last Signal: " << std::to_string(connRef.lastSignalOriginTime) << std::endl;
            std::cout << "STP Weight:= " << std::to_string(connRef.stpWeight);
            std::cout << " LTP Weight:= " << std::to_string(connRef.ltpWeight) << std::endl;
            
        }
        void printConnectionFromPointer (connection::Connection* connPtr)
        {
            std::cout << "Connection: Neuron:= " << std::to_string(connPtr->targetNeuronSlot);
            std::cout << "\nTemporalDistance:= " << std::to_string(connPtr->temporalDistanceToTarget); 
            std::cout << "\nLast signal:= " << std::to_string(connPtr->lastSignalOriginTime);
            std::cout << "\nSTP Weight:= " << std::to_string(connPtr->stpWeight);
            std::cout << "\nLTP Weight:= " << std::to_string(connPtr->ltpWeight) << std::endl;
            
        }
        
        int32_t generateOutGoingSignals(int32_t neuronId) 
        {
            // This should only be invoked by a neuron that cascades

            /**
            * @brief step through the outgoing signals for the specified neuron
            * and generate a signal for every valid connection
            * 
            * @param   slot number for neuron to generate signals for
            * 
            * @return  slot number we are processing for tracking
            * 
            * @details For every connection found in the outgoingSignals for neuronId
            * a signal is generated using the parameter found in the connection object.
            * 
            * The only neurons that are called to generate signals are those that have 
            * cascaded as indicated by the refractory end being set to a future clock.
            * 
            * All other neurons have no signals to be created.
            * 
            * The caller is responsible for only requesting signal generation for 
            * qualifying neurons that cascade. The magnitude of the cascade causing
            * event is of no concern to the signal generator.
            * 
            */
            // 
            // Loop through all of the connections for a neuron and generate and
            // enqueue a signal to the target neuron for each of the connections.
            // There will be as many signals generate as there are valid connections
            // in the outgoingSignals queue.

            std::cout << "\nGenerate signals called with neuronId:= " << std::to_string(neuronId);

            for (int32_t connIdx : m_neuronPool[neuronId].outgoingSignals)
            {
                std::cout << "\noutgoing targetNeuronSlot:= " << std::to_string(m_connPool[connIdx].targetNeuronSlot);
                if (m_connPool[connIdx].targetNeuronSlot >= 0)      // not an empty proto connection
                {
                    std::cout << "\nTrue distance vs. refractoryEnd:= " << 
                        std::to_string(m_connPool[connIdx].temporalDistanceToTarget + masterClock) << " vs. " <<
                        std::to_string(m_neuronPool[neuronId].refractoryEnd);

                    // now check if target is refractory - ergo accept no signal for refractory period
                    // Have to add masterClock to get actual real clock time as connection temporalDistance is always relative

                    if ( m_connPool[connIdx].temporalDistanceToTarget + masterClock >
                        m_neuronPool[neuronId].refractoryEnd )      
                        {
                            // only generate a signal if connection real clock is beyond refractory end
                            // otherwise no point in generating a signal.
                            // These are future post-refractory signals have not yet arrived.

                            signalEventTime = generateASignal(connIdx); // receive signal event time back

                            // Every time we push a signal to a neuron we have to check the target neuron nextEvent.
                            // But this would become very, very expensive if there were lots of signals pushed to the same
                            // neuron.
                            // Not True! Each signal push just has to check itself against the nextEvent to
                            // see if this is older and update neuron nextEvent and globalNextEvent accordingly.
                            // These should be very cheap tests                            

                            m_neuronPool[neuronId].nextEvent = 
                                    (signalEventTime >= m_neuronPool[neuronId].nextEvent) ?
                                        m_neuronPool[neuronId].nextEvent : signalEventTime;

                            // Save the next actionTime if it's smaller than the current globalNextEvent.
                            // This should avoid bothering with the scanning the incoming signals queue of every neuron
                            // to set their nextEvent and just run through any non-empty incoming signals queue.


                            globalNextEvent = (signalEventTime >= globalNextEvent) ?
                                                    globalNextEvent : signalEventTime;


                            std::cout << "\nSignal generated to neuron:= " << std::to_string(neuronSignaled);
                        }
                    }
                    
                    // if (connectionIsNotFiller(ptr))
                    // {
                    //     generateASignal(ptr);
                    // }
                }
                return neuronId;
            }
            
          int32_t generateASignal(int32_t connIdx)
            {
                /**
                * @brief For each valid connection, generate a signal using the connection
                * parameters and return the slot number of the targetted neuron - for debugging
                * 
                * @param   index to the connection retrieved from the neuron's outgoingSignals vector
                * that defines the connection which will be used to generate the signal and enqueued
                * to the target neuron's incomingSignals queue.
                * 
                * @details first allocate a signal slot; then fill it's values; then enqueue to target
                * neuron incomingSignals queue vector
                * Any signal being created must be at least greater than the current masterClock and
                * the current masterClock was set from the globalNextEvent - and also the nextEvent for 
                * neuron for whom we are generating the signal...so we should adjust the nextEvent for
                * the targetNeuron to the generated signal action time plus we should upodate the
                * globalNextEvent if this signal is older i.e. clock time is less. 
                * We should also update the nextEvent time for the targetNeuron if the new signal is less
                * than the current nextEvent time. If this is the first signal this will work as the 
                * default nextEvent time is INT32_MAX.
                * And the adjust globalNextEvent if necesary.
                * 
                * July 2025:    Add new sourceConnId to signal for group STP/LTP processing 
                * 
                * @return event time of signal generated for nextEvent tracking
                * 
                */

                std::cout << "\nGenerate a signal for connection:= " << std::to_string(connIdx);
                printConnectionFromIndex(connIdx);

                // SRB is different as it can wrap  
                if (currentSignalSlot >= signalBufferCapacity) {
                    currentSignalSlot = 0;
                    nextSignalSlot = 0;
                }
                else { 
                    nextSignalSlot = ++currentSignalSlot; 
                }
                
                // srb is a vector of signal::Signal structs 
                // Use return of index to next srb slot - pseudo_allocation.
                 
                // fill in the signal values before enqueueing to the target
                m_srb[nextSignalSlot].actionTime = masterClock + m_connPool[connIdx].temporalDistanceToTarget;   // relative distance
                m_srb[nextSignalSlot].amplitude = m_connPool[connIdx].stpWeight + m_connPool[connIdx].ltpWeight;  // moderated amplitudes
                m_srb[nextSignalSlot].owner = m_connPool[connIdx].targetNeuronSlot;     // target is the signal owner
                m_srb[nextSignalSlot].sourceConnId = connIdx;   // source is the generating connnection

                // Update the last signal time for this connection - used for stp/ltp aging.
                // No comparison needed as any prior signals would have been older
                m_connPool[connIdx].lastSignalOriginTime = masterClock;

                std::cout << "\nCreated this signal:.... for nextSignalSlot:= " << std::to_string(nextSignalSlot);
                srbObj.printSignalFromIndex(nextSignalSlot);

                signal::Signal sigRef = m_srb[nextSignalSlot];  // this should point to the signal we are building
                // srbObj.printSignalFromRef(signalRef);        // This should ref the same signal
                
                // targetNeuronId = ptr->targetNeuronSlot; // this is where to enqueue the signal
                // m_neuronPool[targetNeuronId].incomingSignals.push_back(&signalRef);
                // incomingSignals is a vector of pointers to Signal structs
                
                // m_neuronPool[ptr->targetNeuronSlot].incomingSignals.push_back(&signalRef);
                // Just push the srb index into the target's incomingSignal queue.

                std::cout << "\nAbout to push signal to targetNode: = " << std::to_string(m_connPool[connIdx].targetNeuronSlot);

                // incomingSignals if a vector of indexes into the srb buffer
                m_neuronPool[m_connPool[connIdx].targetNeuronSlot].incomingSignals.push_back(nextSignalSlot);

                // Now we have to update the neuron nextEvent and the globalNextEvent to the absolute future time

                m_neuronPool[m_connPool[connIdx].targetNeuronSlot].nextEvent =
                    (m_neuronPool[m_connPool[connIdx].targetNeuronSlot].nextEvent <= sigRef.actionTime + masterClock) ?
                       m_neuronPool[m_connPool[connIdx].targetNeuronSlot].nextEvent  :  // Leave it alone
                         sigRef.actionTime + masterClock;                               // else update it new lower value 

                // At this point the nextEvent for the neuron we pushed to should be update for the actionTime
                // in the signal just pushed. This is a better alternative than scanning the signals.

                // And now check globalNextEvent
                (globalNextEvent < sigRef.actionTime + masterClock) ? globalNextEvent : sigRef.actionTime + masterClock;

                std::cout << "\nPrint incoming signals for targetNode:= " << std::to_string(m_connPool[connIdx].targetNeuronSlot);

                for (int32_t idx : m_neuronPool[m_connPool[connIdx].targetNeuronSlot].incomingSignals)
                {
                    std::cout << "\nsrb index:= " << std::to_string(idx);
                    srbObj.printSignalFromIndex(idx);
                }
                
                
                return m_srb[nextSignalSlot].actionTime;    // this is the time for this signal event
            }

            void strengthen (std::int32_t connId)
            /**
             * @brief:  Called by cascading neuron for group strenthening
             * 
             * @details:generateASignal adds to signal the sourceId of the connection that raised the signal.
             *          This is to support strenthening of a group of connections that contribute to a 
             *          neuron cascade and will be used in the future to support learning.
             */
            {
                // do nothing for now
                ;
            }

            /**
            * @brief   Given that all pools and elements are public we should be able to avoid
            * using these getter/setter functions - speed is the goal and avoiding function
            * calls is done wherever possible.
            */
            
            // get the target neuron slot for the given connection
            // int32_t get_target_neuron(int32_t connectionSlot, int32_t neuronSlot)
            // {
            //     return 0;   // return neuron target slot for the connection
            // }    
            
            // // return the temporal clock distance for the connection slot
            // int32_t get_temporal_distance(int32_t connectionSlot)
            // {
            //     return 0;// return temporal distance
            // }    
            
            // int16_t get_signal_size(int32_t connectionSlot)
            // {
            //     return 0;   // return the weight for the current connection
            // }             
            // int16_t get_stp_accumulator(int32_t connnectionSlot)
            // {
            //     return 0;   
            // };       
            // int16_t get_ltp_accumulator(int32_t connectionSlot)
            // {
            //     return 0;   // return ltp accumulator value
            // };       
            
            // // set the target neuron slot for the connection slot
            // void set_target_neuron(int32_t connectionSlot, int32_t neuronslot)
            // {
            //     ;    // set target neuron number for connection
            
            // }
            // void set_temporal_distance(int32_t connectionSlot, int32_t temporalDistance)
            // {
            //     ;   // set temporal distance to target neuron for the connection
            // }
            // void set_signal_size(int32_t signalSlot, int16_t )
            // {
            //     ;   // set the signal weigth into the signal 
            // }     // set the size for the signal
            // void set_stp_accumulator(int32_t connectionSlot, int16_t stpWeight)
            // {
            
            // } // set the stp accumulator value
            // void set_ltp_accumulator(int32_t connectionslot, int16_t ltpWeight)
            // {
            //     ;
            // } 
            // void apply_stp(int32_t connectionSlot, int16_t stpValue)
            // {
            //     ;   // set stp result value in connection
            // }
            // void apply_ltp(int32_t connectionSlot, int16_t ltpValue)
            // {
            //     ;   // set ltp result value in connection
            // }
            
            ~Connections()
            {
                ;   // when allocated vectors go out of scope their heap usage is released.
            }
            
            
        };
        
    } // end of conns namespace scope
    #endif // CONNECTIONS_H_INCLUDED
    