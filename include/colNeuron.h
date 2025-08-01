#ifndef COLNEURON_H
#define COLNEURON_H

#include <iostream>
#include <cstddef>
#include "TCNConstants.h"

extern const int32_t micro_col_size;
extern const int32_t layer_col_size;
extern const int32_t six_pack_size;


/**
 * @brief This is the coordinate system for addressing neuron columns and mapping the neuron
 * column coordinate system onto the linear neuron pool.
 * 
 * @details The only user of this is the network initialization routines that create all of
 * the connections between the neurons. Once they have been made there is no further need to
 * reference the neurons by the columnar coordinate system as all processing is by linear
 * passes through the neuron pool.
 * 
 */

struct colNeuron 
{
  int32_t nNumber {};     // neuron number in the microcolumn - zero based
  int32_t colNumber {};   // column number in the sixpack layer - zero based
  int32_t layerNumber {}; // layer number in the sixpack - zero based
  // vNumber assignment will occur after we have proven six pack number works as expected
  // int32_t vNumber {};     // visual layer number in IT thru V1
};

int32_t colToLinearMap (colNeuron colCoord)
{
  /**
   * @brief Take incoming col coord spec for a neuron and return the mapping into the linear
   * neuron pool space.
   * 
   * @details The only user of this routine is the TCN connections builder. There is no back-conversion
   * available or possible without burdening every neuron in the neuron pool with it's origina col coords.
   * 
   */
  return colCoord.nNumber + (colCoord.colNumber + 1) * (micro_col_size * micro_col_size) +
         (colCoord.layerNumber + 1) * (layer_col_size) * (micro_col_size);
}


#endif