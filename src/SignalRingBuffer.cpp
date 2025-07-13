#include <iostream>
#include "SignalRingBuffer.h"
#include "TCNConstants.h"
#include "Signal.h"
#include <cstdint>

namespace srb
{

    static std::int32_t currentSignalSlot{};

    // class constructor
    SignalRingBuffer (std::int32_t ringSize) 
        {
            std::vector<signal::Signal> srb (ringSize);
            currentSignalSlot = 0;
        }

    // this returns the slot number
    std::int32_t SignalRingBuffer::nextSignalSlot()
    {
      if (currentSignalSlot >= srb::srb.capacity()) {
            currentSignalSlot = 0;
            return 0;
        }
        else { return ++currentSignalSlot; }
    }

    std::int32_t& signalRingBuffer::signalSlotRef(std::int32_t currentSignalSlot)
    {
        return srb[currentSignalSlot];
    }

    std::int23_t getSignalClock(std::int32_t& signalRef)
    {
            return signalRef -> deliveryTime;
    }
    int     SignalRingBuffer::getClock (int slot) {return *(clockBuffer + slot);}
    short   SignalRingBuffer::getSize (int slot) {return *(sizeBuffer + slot);}
    void    SignalRingBuffer::setClock (int slot, int clock) { *(clockBuffer + slot) = clock; }
    void    SignalRingBuffer::setSize (int slot, short size) { *(sizeBuffer + slot) = size; }
    int     * SignalRingBuffer::getClockOrigin () { return clockBuffer; }
short   * SignalRingBuffer::getSizeOrigin () { return sizeBuffer; }
}   // end srb namespace
