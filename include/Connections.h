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
int32_t currentConnectionSlot{-1};  // allocation always returns ++currentConnectionSlot - no wrap like srb
int32_t connectionPoolCapacity{};   // filled in by constructor

// other globals used by connections

extern std::vector<neuron::Neuron> m_neuronPool;
extern std::int32_t currentNeuronSlot; // forces start @ 0
extern std::int32_t neuronPoolCapacity;

extern std::int32_t currentSignalSlot;
extern std::int32_t signalBufferCapacity;
extern std::vector<signal::Signal> m_srb;

namespace tconst = tcnconstants;
namespace conns
{
    signal::Signal signalRef;
    int32_t targetNeuronId; // used when enqueueing new signals
    int32_t nodeSignaled;   // used for tracing which nodes get a connection signal
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
        int32_t nextSignalSlot;             // not a member variable

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

        }

        Connections() = default;

        // static int32_t     connection_count;                
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

        void printConnection (connection::Connection &connRef)
        {
            std::cout << "Connection: Neuron:= " << std::to_string(connRef.targetNeuronSlot);
            std::cout << " TemporalDistance:= " << std::to_string(connRef.temporalDistanceToTarget) << std::endl;
            std::cout << "Last Signal: " << std::to_string(connRef.lastSignalOriginTime) << std::endl;
            std::cout << "STP Weight:= " << std::to_string(connRef.stpWeight);
            std::cout << " LTP Weight:= " << std::to_string(connRef.ltpWeight) << std::endl;

        }
        void printConnectionFromPointer (connection::Connection* cptr)
        {
            std::cout << "Connection: Neuron:= " << std::to_string(cptr->targetNeuronSlot);
            std::cout << " TemporalDistance:= " << std::to_string(cptr->temporalDistanceToTarget) << std::endl;
            std::cout << "STP Weight:= " << std::to_string(cptr->stpWeight);
            std::cout << " LTP Weight:= " << std::to_string(cptr->ltpWeight) << std::endl;

        }

        int32_t generateOutGoingSignals(int32_t neuronId) 
        {
            /**
             * @brief step through the outgoing signals for the specified neuron
             * and generate a signal for every valid connection
             * 
             * @param   slot number for neuron to generate signals for
             * 
             * @return  slot number we are processing for tracking
             * 
             * @details For every connection found in the outgoingSignals for neuonId
             * a signal is generated using the parameter found in the connection object.
             * 
             * The only neurons that are called to generate signals are those that have 
             * cascaded as indicated by the refractory end being set to a future clock.
             * 
             * All other neurons have no signals to be created.
             * 
             * The caller is responsible for only requesting signal generation for 
             * qualifying neurons.
             * 
             */
            // 
            // Loop through all of the connections for a neuron and generate and
            // enqueue a signal to the target neuron for each of the connections.
            // There will be as many signals generate as there are valid connections
            // in the outgoingSignals queue.
            for (connection::Connection *ptr : m_neuronPool[neuronId].outgoingSignals)
            {
                if (ptr->targetNeuronSlot >= 0)
                {
                    // now check if target is refractory - ergo accept no signal for refractory period
                    if ( ptr->temporalDistanceToTarget >
                        m_neuronPool[ptr->targetNeuronSlot].refractoryEnd )      
                    {
                        // only generate a signal if connection clock is beyond refractory end
                        // otherwise no point in generating a signal
                        nodeSignaled = generateASignal(ptr);
                    }
                }
        
                // if (connectionIsNotFiller(ptr))
                // {
                //     generateASignal(ptr);
                // }
            }
            return neuronId;
        }

        bool connectionIsNotFiller(connection::Connection *ptr)
        {
            // never called; used as example of required test
            // make sure we don't process a filler connection left over from constructor
            // where targets were init'd to -1
            return ptr->targetNeuronSlot >= 0;
        }

        int32_t generateASignal(connection::Connection *ptr)
        {
        /**
         * @brief For each valid connection, generate a signal using the connection
         * parameters and return the slot number of the targetted neuron - for debugging
         * 
         * @param   ptr to the connection retrieved from the neuron's outgoingSignals vector
         * that defines the connection which will be used to generate the signal and enqueued
         * to the target neuron's incomingSignals queue.
         * 
         * @details first allocate a signal slot; then fill it's values; then enqueue to target
         * neuron incomingSignals queue vector
         * 
         * @return slot number of the target neuron - used for tracing
         * 
         */
            // SRB is different as it can wrap  
            if (currentSignalSlot >= signalBufferCapacity) {
                currentSignalSlot = 0;
                nextSignalSlot = 0;
            }
            else { 
                nextSignalSlot = ++currentSignalSlot; 
            }

            // srb is a vector of signal::Signal structs - returns ref to the struct
            signalRef = m_srb[nextSignalSlot];
            
            // fill in the signal values before enqueueing to the target
            signalRef.actionTime = ptr->temporalDistanceToTarget;   // fixed distance
            signalRef.amplitude = ptr->stpWeight + ptr->ltpWeight;  // moderated amplitudes
            signalRef.owner = ptr->targetNeuronSlot;                // target is the signal owner
            
            // targetNeuronId = ptr->targetNeuronSlot; // this is where to enqueue the signal
            // m_neuronPool[targetNeuronId].incomingSignals.push_back(&signalRef);
            // skip the extra assignment
            // incomingSignals is a vector of pointers to Signal structs
            
            m_neuronPool[ptr->targetNeuronSlot].incomingSignals.push_back(&signalRef);


            return ptr->targetNeuronSlot;
        }
        // static int *target_neurons_origin;    // 32 bit pointer to the target
        // static int *temporal_distance_origin; // relative clock distance to target
        // static short *signal_size_origin;     // current size of signal for this connection/synapse - base is 1000
        // static short *stp_accumulator_origin; // stp accumulated level
        // static short *ltp_accumulator_origin; // ltp accumulated level
        // static int *last_signal_clock_origin; // used to determine how much STP & LTP decay has occurred
        // static int next_connection_slot;      // used to allocate connections slots during building of the
        //                                       // neuron connection networks
        // he signal size will be modified by STP and LTP and any other memory and enhancement actions
        // static int connection_count;                 // number of connections created

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
