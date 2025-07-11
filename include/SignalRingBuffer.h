#ifndef SIGNALRINGBUFFER_H_INCLUDED
#define SIGNALRINGBUFFER_H_INCLUDED
/** \brief SignalRingBuffer
 *
 * \param size of buffer
 * \param
 * \return Signal clock
 * \return Signal size
 * \return slot number
 * \return int clock array origin address
 * \return float size array origin address
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
    struct Signal {
        int32_t deliveryTime {};    // signal origin time + neuron temporal distance
        int16_t amplitude {};       // connection modified signal ampliude
        int16_t reserved {};        // reserved - rounding
    };
    class SignalRingBuffer {
        /**
         * The SRB class will be used to allocate, deallocate the vector of signals.
         * SRB also tracks where the next slot is for dispensing a usable signal slot.
         * The requestor for a signal is responsible for initializing the signal fields.
         */
        public:
        // static int *clockBuffer;
        // static short *sizeBuffer;
        // static int bufferLength;
        std::vector<Signal> srb;                    // forward declaration of srb.
        int32_t nextSignalSlot();               // returns next vacant slot number
        int32_t& signalSlotRef(int32_t);            // ref to a slot
        int32_t getSignalClock (int32_t&);           // return clock in slot
        int16_t getSignalWeight (int16_t&);          // return weight in slot
        void setSignalClock (int32_t&);              // set signal clock value
        void setSignalWeight (int32_t&, int16_t&_);       // set signal weight
        static int count_signal_drops;              // statistics
    
    };
}  // end srb namespace
#endif // SIGNALRINGBUFFER_H_INCLUDED

