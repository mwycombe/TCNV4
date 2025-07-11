#include <iostream>
#include "SignalRingBuffer.h"

namespace srb
{

    static int32_t currentSignalSlot{};

    // class constructor
    SignalRingBuffer (tcnconstants::signal_count)
    {
        std::vector<Signal>srb (ringSize);
        currentSignalSlot = 0;
    }

    // this returns the slot number
    int32_t SignalRingBuffer::nextSignalSlot()
    {
      if (currentSignalSlot >= srb::srb.capacity()) {
            currentSignalSlot = 0;
            return 0;
        }
        else { return ++currentSignalSlot; }
    }

    int32_t& signalRingBuffer::signalSlotRef(int32_t currentSignalSlot)
    {
        return srb[currentSignalSlot];
    }

    int23_t getSignalClock(int32_t& signalRef)
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
