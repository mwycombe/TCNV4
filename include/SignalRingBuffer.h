#ifndef SIGNALRINGBUFFER_H_INCLUDED
#define SIGNALRINGBUFFER_H_INCLUDED

#include "Signal.h"
#include <cstdint>
#include <vector>
#include <climits>

/**
 * @brief SignalRingBuffer
 *
 * @param size of buffer
 * 
 * 
 * @return next available slot number
 *
 
 */

 #include <iostream>
 #include <vector>

 #include "TCNConstants.h"
 namespace srb
 {
    /**
     * The SignalRingBuffer class creates a large circular vector of signals.
     * 
     */

    std::int32_t currentSignalSlot {INT32_MAX}; // ensure we start at 0 slot
    std::int32_t signalBufferCapacity {};       // init'd for performance
    class SignalRingBuffer {
        /**
         * The SRB class will be used to allocate, deallocate the vector of signals.
         * SRB also tracks where the next slot is for dispensing a usable signal slot.
         * The requestor for a signal is responsible for initializing the signal fields.
         */
        public:

        // class constructor
        SignalRingBuffer (std::int32_t ringSize) 
        {
            #ifdef TESTING_MODE
            // just reserve 75 srb slots
                m_srb.reserve(75);
            #else
                m_srb.reserve(ringSize);
            #endif 

            signalBufferCapacity = m_srb.capacity();

            // init m_srb with empty signals to allocate heap
            // regardless of size
            // create an empty signal with impossible clock value

            signal::Signal emptySignal{ INT32_MAX,1000,0}; // default values

            for (int i = 0; i < m_srb.capacity(); ++i) {
                #ifdef TESTING_MODE
                    emptySignal.testId = i;
                #endif
                m_srb.push_back(emptySignal);
            }
        }

        ~SignalRingBuffer ()
        {   
            ;   // when allcoated vectors go out of scope their heap usage is released
        }

        std::int32_t getCurrentSignalSlot() { return currentSignalSlot; }
        signal::Signal getSlotRef(int slot) { return m_srb[currentSignalSlot]; }
        std::int32_t allocateSignalSlot()
        /**
         * @brief returns the next avaible srb slot
         * 
         * When the srb gets to the last slot it just wraps, regardless.
         * It never changes the signal contents; that's up to the connection
         * that asked for the signal slot to be dispensed.
         * 
         * The class uses a static to keep track of the current slot.
         * 
         * Future: srb will track oldest dispensed slot and keep statistics
         * on how many times we wrapped; used for future tuning of the srb
         * capacity.
         * 
         * @param   None
         * 
         * @return  The signal slot number for the next to be used.
         * 
         * @throws  Nothing
         */
        {
            if (currentSignalSlot < signalBufferCapacity) {
                // this will be the most frequent path
                return ++currentSignalSlot;
            }
            else {
                // srb has wrapped
                currentSignalSlot = 0;
                return currentSignalSlot;
            }
        }

        // make this public so all can access it
        std::vector<signal::Signal> m_srb;


    };
}  // end srb namespace

#endif // SIGNALRINGBUFFER_H_INCLUDED

