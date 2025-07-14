#ifndef CONNECTIONS_H_INCLUDED
#define CONNECTIONS_H_INCLUDED
#include <iostream>
#include <vector>
#include <queue>
#include "TCNConstants.h"
#include "Connection.h"
#include "Neurons.h"
#include "SignalRingBuffer.h"

// make this extern global so all can access it.

std::vector<connection::Connection> m_connPool{};
int32_t currentConnectionSlot{-1};  // allocation always returns ++currentConnectionSlot - no wrap like srb
int32_t connectionPoolCapacity{};   // filled in by constructor

// other globals used by connections

extern std::vector<neuron::Neuron> m_neuronPool;
extern std::int32_t currentNeuronSlot; // forces start @ 0
extern std::int32_t neuronPoolCapacity;

std::int32_t currentSignalSlot{INT32_MAX};
std::int32_t signalBufferCapacity{};
std::vector<signal::Signal> m_srb{};


namespace conns
{
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

    */
    class Connections
    {

    public:
        // static short   *stp_accumulator_origin;        //stp accumulated level
        // static short   *ltp_accumulator_origin;        //ltp accumulated level
        // static int     *last_signal_clock_origin;      //used to determine how much STP & LTP decay has occurred
        // static int      next_connection_slot;          // used to allocate connections slots during

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

        }  // how many connections to create

        // static int32_t     connection_count;                // number of connections created
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


        public:
            // static int *target_neurons_origin;    // 32 bit pointer to the target
            // static int *temporal_distance_origin; // relative clock distance to target
            // static short *signal_size_origin;     // current size of signal for this connection/synapse - base is 1000
            // static short *stp_accumulator_origin; // stp accumulated level
            // static short *ltp_accumulator_origin; // ltp accumulated level
            // static int *last_signal_clock_origin; // used to determine how much STP & LTP decay has occurred
            // static int next_connection_slot;      // used to allocate connections slots during building of the
            //                                       // neuron connection networks
            // he signal size will be modified by STP and LTP and any other memory and enhancement actions


            static int connection_count;                 // number of connections created

            /**
             * @brief   Given that all pools and elements are public we should be able to avoid
             * using these getter/setter functions - speed is the goal and avoiding function
             * calls is done wherever possible.
             */

            // get the target neuron slot for the given connection
            int32_t get_target_neuron(int32_t connectionSlot, int32_t neuronSlot)
            {
                return 0;   // return neuron target slot for the connection
            }    

            // return the temporal clock distance for the connection slot
            int32_t get_temporal_distance(int32_t connectionSlot)
            {
                return 0;// return temporal distance
            }    

            int16_t get_signal_size(int32_t connectionSlot)
            {
                return 0;   // return the weight for the current connection
            }             
            int16_t get_stp_accumulator(int32_t connnectionSlot)
            {
                return 0;   
            };       
            int16_t get_ltp_accumulator(int32_t connectionSlot)
            {
                return 0;   // return ltp accumulator value
            };       

            // set the target neuron slot for the connection slot
            void set_target_neuron(int32_t connectionSlot, int32_t neuronslot)
            {
                ;    // set target neuron number for connection

            }
            void set_temporal_distance(int32_t connectionSlot, int32_t temporalDistanc)
            {
                ;   // set temporal distance to target neuron for the connection
            }
            void set_signal_size(int32_t signalSlot, int16_t )
            {
                ;   // set the signal weigth into the signal 
            }     // set the size for the signal
            void set_stp_accumulator(int32_t connectionSlot, int16_t stpWeight)
            {
        
            } // set the stp accumulator value
            void set_ltp_accumulator(int32_t connectionslot, int16_t ltpWeight)
            {
                ;
            } 
            void apply_stp(int32_t connectionSlot, int16_t stpValue)
            {
                ;   // set stp result value in connection
            }
            void apply_ltp(int32_t connectionSlot, int16_t ltpValue)
            {
                ;   // set ltp result value in connection
            }
        
            ~Connections()
            {
                ;   // when allocated vectors go out of scope their heap usage is released.
            }


    };

} // end of conns namespace scope
#endif // CONNECTIONS_H_INCLUDED
