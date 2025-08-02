#include <iostream>
#include <vector>

#include <cstddef>

#include "TCNConstants.h"

namespace tconst = tcnconstants;

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

  int32_t IT_capacity{};
  int32_t layers_capacity{};
  int32_t columns_capacity{};
  int32_t micros_capacity{};
  // vector order: IT < Levels < Columns < Microcolumns
  // We size from the right...
  ITLayer.reserve(1);
  std::cout << "ITLayer capacity:= " << std::to_string(ITLayer.capacity()) << "\n";
  for (auto& layers : ITLayer)
  {
    layers.reserve(tconst::layer_count);
    for(auto&  columns : layers)
    {
      columns.reserve(tconst::column_capacity);
      for (auto&  micros : columns)
      {
        micros.reserve(tconst::micro_capacity);
      }
    }


  } 





}