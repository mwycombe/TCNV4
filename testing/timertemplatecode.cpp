#include <chrono> // for std::chrono functions
#include <iostream>
#include <vector>
#include <cstdint>

#include "Signal.h"
#include "SignalRingBuffer.h"
#include "Logger.h"

class Timer
{
private:
	// Type aliases to make accessing nested type easier
	using Clock = std::chrono::steady_clock;
	using Second = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<Clock> m_beg { Clock::now() };

public:
	void reset()
	{
		m_beg = Clock::now();
	}

	double elapsed() const
	{
		return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
	}
};

#include <iostream>

int main()
{
    Timer t;

    // Code to time goes here
     srb::SignalRingBuffer SRB = srb::SignalRingBuffer(3000000);
    Logger logger(false, INFO, "srbwrap.log");

    std::cout << "Testing with: " << signalBufferCapacity << " signals: \n";

    std::int32_t nextSlot;

    for (int i = 0; i < signalBufferCapacity; ++i){
      // nextSlot = SRB.allocateSignalSlot();
      /**
       * @brief Replace this call to the SRB.allocateSignalSlot() function
       * with direct access and manipulation of the static slot management.
       * As the SRB always wraps this call never fails.
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
      //std::string msg = "Slot idx: " + std::to_string(nextSlot) + " SlotId: " + std::to_string(SRB.getSlotRef(nextSlot).testId);
      //logger.log(INFO,msg  );
    }


    std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

    return 0;
}