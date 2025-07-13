// Connections.cpp
#include <iostream>
#include <vector>
#include "TCNConstants.h"
#include "Connections.h"
//#include "FakeTCN.h"

namespace conns
{

int *   Connections::target_neurons_origin;        //32 bit pointer to the target
int *   Connections::temporal_distance_origin;      //relative clock distance to target
short * Connections::signal_size_origin;            //current size of signal for this connection/synapse - base is 1000
short * Connections::stp_accumulator_origin;        //stp accumulated level
short * Connections::ltp_accumulator_origin;        //ltp accumulated level
int   * Connections::last_signal_clock_origin;      //used to determine how much STP & LTP decay has occurred
int     Connections::connection_count;       // number allocated
int     Connections::next_connection_slot{-1};  // start with slot zero - remain across multiple calls

Connections::Connections (int count)
{
    /* allocate array for target neuron ptr, and temporal distance */
    target_neurons_origin       = new int [count];   // pointer to array of neurons
    temporal_distance_origin    = new int [count];
    signal_size_origin          = new short [count];
    stp_accumulator_origin      = new short [count];
    ltp_accumulator_origin      = new short [count];
    last_signal_clock_origin    = new int [count];
    connection_count = count;   // set static count of connections

}

int Connections::get_target_neuron(int slot)
{   return (*(target_neurons_origin + slot));  }

int Connections::get_temporal_distance(int slot)
{   return ( *(temporal_distance_origin + slot));    }

short Connections::get_signal_size(int slot)
{   return ( *(signal_size_origin + slot));   }

short Connections::get_stp_accumulator(int slot)
{   return ( *(stp_accumulator_origin + slot));   }

short Connections::get_ltp_accumulator(int slot)
{   return ( *(ltp_accumulator_origin + slot));   }

void Connections::set_target_neuron(int slot, int neuron_number)
{    * (target_neurons_origin + slot) = neuron_number;  }

void Connections::set_temporal_distance(int slot, int temp_distance)
{    * (temporal_distance_origin + slot) = temp_distance;   }

void Connections::set_signal_size(int slot, short size)
{    * (signal_size_origin + slot) = size;  }

void Connections::set_stp_accumulator(int slot, short stp)
{    * (stp_accumulator_origin + slot) = stp;   }

void Connections::set_ltp_accumulator(int slot, short ltp)
{    * (ltp_accumulator_origin + slot) = ltp;   }

int * Connections::getTemporalDistanceOrigin()
{    return target_neurons_origin;  }

short * Connections::getSignalSizeOrigin()
{    return signal_size_origin; }

short * Connections::getStpOrigin()
{    return stp_accumulator_origin; }

short * Connections::getLtpOrigin()
{    return ltp_accumulator_origin; }

int * Connections::getLastClockOrigin()
{   return last_signal_clock_origin; }

short   Connections::apply_stp(int connNumber, short signal_size) // compute and return stp enhanced signal size
{
    return (signal_size);       // do nothing for now
}
short   Connections::apply_ltp(int connNumber, short signal_size) // compute and return ltp enhance signal size
{    return (signal_size);   }  // do nothing for now

}   // end of namespace conns
