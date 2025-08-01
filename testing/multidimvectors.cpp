#include <iostream>
#include <vector>

#include <cstddef>

#include "TCNConstants.h"

extern int32_t  micro_capacity;

int main ()
{
  /**
   * @brief Test allocating multi dimensional vectors to hold V layers and six packs of neurons.
   * 
   * @details Use coord system of microcolumns, columns and layers to represent six packs
   * Then assign six packs to IT, V4, V2, and V1.
   * 
   */
  std::vector<std::vector<std::vector<std::vector<int32_t>>>> ITLayer;

  // vector order: IT < Levels < Columns < Microcolumns
  // We size from the right...
  ITLayer.reserve(25);
  for (int i = 0; i< ITLayer.capacity() ; i++ )
  {
    ITLayer[i].reserve(micro_capacity);
  }







}