#include <iostream>
#include <vector>
#include <cstdint>

#include "Signal.h"
#include "SignalRingBuffer.h"
#include "Logger.h"

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

    srb::SignalRingBuffer SRB = srb::SignalRingBuffer(75);
    Logger logger(INFO, "srbwrap.log");

    std::int32_t nextSlot;

    for (int i = 0; i<75; ++i){
      nextSlot = SRB.allocateSignalSlot();
      //std::cout << "Allocate:" << nextSlot << " ">>;
      // now for some logging
      // std::cout << SRB.getSlotRef(nextSlot).testId << " ";
      std::string msg = "Slot idx: " + std::to_string(nextSlot) + " SlotId: " + std::to_string(SRB.getSlotRef(nextSlot).testId);
      logger.log(INFO,msg  );
    }

 }