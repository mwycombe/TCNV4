#undef TESTING_MODE

#include <iostream>
#include <vector>
#include <cstdint>

#include "Signal.h"
#include "SignalRingBuffer.h"
#include "Logger.h"

extern std::int32_t currentSignalSlot;
extern std::int32_t signalBufferCapacity;
extern std::vector<signal::Signal> m_srb;

/**
 * @brief Run multiple allocations to ensure that a small srb wraps (currently capacity of 50).
 * Each empty signal has a unique id to track correct allocation.
 * Appends to the srbwrap.log for the run.   
 * 
 * For a fresh log, delete the old log by hand.
 *        
 * @param None
 * 
 * @return  0 if ok; else non-zero
 */

 int main () {
  // create class instance for this run

    srb::SignalRingBuffer SRB = srb::SignalRingBuffer(300000);
    Logger logger(false, INFO, "srbwrap.log");

    std::cout << "Testing with: " << signalBufferCapacity << " signals: \n";

    std::int32_t nextSlot;

    for (int i = 0; i < signalBufferCapacity; ++i){
      // nextSlot = SRB.allocateSignalSlot();
      /**
       * @brief Replace this call to the SRB.allocateSignalSlot() function
       * with direct access and manipulation of the static slot management.
       * As the SRB always wraps this call never fails.
       * 
       */

       /**
        * @brief  Pseudo-allocateSignalSlot
        */

        if (currentSignalSlot >= signalBufferCapacity) {
            currentSignalSlot = 0;
            nextSlot = 0;
        }
        else { nextSlot = ++currentSignalSlot; }
        
      // std::cout << "Allocate:" << nextSlot << '\n';
      // now for some logging

      // std::cout << SRB.getSlotRef(nextSlot).testId << " ";
      std::string msg = "Slot idx: " + std::to_string(nextSlot) + " SlotId: " + std::to_string(SRB.getSlotRef(nextSlot).testId);
      logger.log(INFO,msg  );
    }

 }