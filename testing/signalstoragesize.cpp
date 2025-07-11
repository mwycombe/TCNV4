#include <iostream>
#include <vector>
#include <cstdint>
#include <typeinfo>
#include <array>

struct Signal {
    int32_t actionTime{};     // time when neuron needs to process this signal
    int16_t amplitude{};      // signal size modified by connection weight at origination.
    int16_t unused{};         // available for future use; pads signal to 8 bytes/2 words
  };

int main ()
  {
    std::vector<Signal> signalVector; 
    signalVector.reserve(1000000000);   // capacity for 1,000,000,000 signals
    // test allocation of Signal vector
    auto begin{signalVector.begin()};
    auto end{signalVector.end()};

    // fill the vector elements
    Signal aSignal = {1, 1000};

    for (int i=1; i < signalVector.capacity(); ++i) {
      signalVector.push_back(aSignal);
    }

    // for (auto& s: signalVector) {
    //   s.actionTime = 1;
    //   s.amplitude = 1000;
    // }
    // print signal element
     std::cout << "Signal 1. actionTime: " << aSignal.actionTime << " aSignal.amplitude: " << aSignal.amplitude << std::endl;
     std::cout << "Vector [0]: actionTime: " << signalVector[0].actionTime << " amplitude: " << signalVector[0].amplitude << std::endl;

    size_t vector_object_size = sizeof(signalVector);
    size_t element_memory = signalVector.capacity() * sizeof(Signal);
    std::cout << "Struct size: " << sizeof(Signal) << std:: endl;
    std::cout << "Size of vector object: " << sizeof(signalVector) << std::endl;
    std::cout << "Element storage: " << element_memory << std::endl;
    std::cout << "Total storage: " << element_memory + vector_object_size << std::endl;
    std::cout << "Press any key: ";
    int wait{};
    std::cin >> wait;   // wait for any key

    return 0;
  }

