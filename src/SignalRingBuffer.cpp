#include <iostream>
#include "SignalRingBuffer.h"
#include "TCNConstants.h"
#include "Signal.h"
#include <cstdint>



namespace srb
{


    // class constructor
    // SignalRingBuffer (std::int32_t ringSize) 
    //     {
    //         SignalRingBuffer::m_srb.reserve (ringSize);
    //         signalBufferCapacity = SignalRingBuffer::m_srb.capacity();
    //         currentSignalSlot = 0;
    //     }

    // this returns the slot number
    // std::int32_t SignalRingBuffer::nextSignalSlot()
    // {
    //   if (currentSignalSlot >= srb::srb.capacity()) {
    //         currentSignalSlot = 0;
    //         return 0;
    //     }
    //     else { return ++currentSignalSlot; }
    // }

    // std::int32_t& signalRingBuffer::signalSlotRef(std::int32_t currentSignalSlot)
    // {
    //     /**
    //      * @brief; Not needed - just reach into the public m_srb vector with the slot number
    //      * and you get the reference. 
    //      * Saves yet another function call
    //      */
    //     return srb[currentSignalSlot];
    // }

    // std::int23_t getSignalClock(std::int32_t& signalRef)
    // /**
    //  * @brief Replace this with direct access to public variable inside SRB
    //  */
    // {
    //         return signalRef -> deliveryTime;
    // }

    // short   * SignalRingBuffer::getSizeOrigin () { return sizeBuffer; }
}   // end srb namespace
