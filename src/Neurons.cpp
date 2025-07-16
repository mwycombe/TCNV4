// Neuron.cpp
#include "Neurons.h"
#include "TCNConstants.h"
#include "SignalRingBuffer.h"
#include "Connections.h"
#include <vector>
#include <limits.h>
//#include "FakeTCN.h"


namespace neurons
{


//    static const int neuron_count {tcn_params -> arrayOfTCNs[tcnet][tcn_neurons]}; // avoid repeat long expression
//     refractoryEndOrigin = new int32_t[neuron_count];
//     signalQueueOrigin = new v_i_ptr [neuron_count];  // array of vector pointers
//     dependentsOrigin = new v_i_ptr  [neuron_count];  // array of vector pointers
//     necvOrigin = new int32_t[neuron_count];
//     for (int32_t i=0; i < neuron_count; i++)
//     {
//         *(refractoryEndOrigin + i) = 0; // all refractoryEnd start at zero
//         *(necvOrigin + i) = INT32_t_MAX;        // necv start out at max value
//         *(signalQueueOrigin + i) =  nullptr;    // not signal vectors yet
//         *(dependentsOrigin + i)  =  nullptr;    // no connections yet
//     }
//     // set up the aggregation window reduction
//     for (int32_t i=0; i < aggregation_window; i++)
//     {
//         relative_aggregation_array[i] = -(i+1);
//         window_reduction_factors[i] = 2^(i+1);
//     }
// }

// int32_t  * Neurons::get_refractoryEndOrigin()
// {    return refractoryEndOrigin;    }

// v_i_ptr  *  Neurons::get_signalQueue_Origin()
// {    return signalQueueOrigin; }

// v_i_ptr * Neurons::get_dependentsOrigin()
// {       return dependentsOrigin;    }

// int32_t     Neurons::get_necv(int32_t slot)
// {   return *(necvOrigin + slot);}

// int32_t     Neurons::get_refractoryEnd(int32_t slot)
// {    return *(refractoryEndOrigin + slot);  }

// void    Neurons::set_refractoryEnd(int32_t slot, int32_t clock)
// {    *(refractoryEndOrigin + slot) = clock; }

// int32_t Neurons::get_nextNeuronSlot()
// {      return allocateNeurons(1);   }

// int32_t Neurons::allocateNeurons(int32_t count)
// {
//     int32_t origin{nextNeuronSlot};
//     nextNeuronSlot += count;
//     return origin;
// }
//  void   Neurons::add_dependent(int32_t slot, int32_t connection)
//  {   // slot is the index of the neuron to which the connection is a dependent.
//      // connection is the index of the connection
//      (*(dependentsOrigin + slot))->push_back(connection); }

// void    Neurons::enqueueIncomingSignal(int32_t nslot, int32_t signalNumber)
// {
//     if  ( *(refractoryEndOrigin + nslot) >= *(SignalRingBuffer::clockBuffer + signalNumber) )
//          {
//              // just drop the signal from the signal queue
//              SignalRingBuffer::count_signal_drops++;      // for statistics
//              return;  // don't enqueue if we are in refractory period
//          }
//     if ( *(signalQueueOrigin + nslot) == nullptr)
//     {
//         *(signalQueueOrigin + nslot) = new vector<int32_t>;
//     }
//     (*(signalQueueOrigin + nslot)) -> push_back(signalNumber);
//     if ( *(SignalRingBuffer::clockBuffer + signalNumber) < *(necvOrigin + nslot))

//     // This can never, ever be true. Any incoming signal must be at least
//     // one tick beyond current masterClock as there are no zero temporal distances
//     // for any connection.
// //    // check to see if we need to possibly aggregate
// //    if ( *(clockBuffer * signalNumber) <= masterClock)
// //    {   // only possible chance this signal can cause a cascade
// //        aggregate_queued_signals(nslot);
// //    }
//     return;
// }
// void    Neurons::deleteEmptySignalQueue(int32_t nslot)
// {   delete *(signalQueueOrigin + nslot); }

// void    Neurons::notify_dependents(int32_t nslot, int32_t clock)
// {   // run through dependent vector and send them a signal
//     auto connIterator = (*(dependentsOrigin + nslot))->begin();
//     while ( connIterator != (*(dependentsOrigin + nslot))->end() )
//     {
//         signal_connection( (* connIterator), clock );
//     }
//     set_refractoryEnd(nslot, clock +tcnconstants::refractory_period );
//     // we cannot purge the signals as they are spread throughout the vector
//     // and we are part way through aggregating when we cascade
//     // signals will get purged on next aggregation scan
// //    purge_signals(nslot);   // purge signals through refractory end period
//     return;
// }
// void    Neurons::signal_connection(int32_t connNumber, int32_t clock)
// {
//     // provision a signal and send it to the target neuron - for future handling
//     conn_clock = clock + *(Connections::temporal_distance_origin + connNumber);  // temporal distance
//     conn_size = *(Connections::signal_size_origin + connNumber);
//     conn_size = Connections::apply_stp(connNumber, conn_size);
//     conn_size = Connections::apply_ltp(connNumber, conn_size);

//     return;
// }

// void    Neurons::purge_signals(int32_t nslot)
// {   // perform a complete rescan following a cascade
//     auto signalIterator = (*(signalQueueOrigin + nslot))->begin();
//     int32_t refractoryEnd = (*(refractoryEndOrigin + nslot));
//     int32_t necv = int32_t_MAX;
//     while (signalIterator != (*(signalQueueOrigin + nslot))->end())
//     {
//         if ( *(SignalRingBuffer::clockBuffer + (*signalIterator)) <= refractoryEnd)
//         {
//             signalIterator = (*(signalQueueOrigin + nslot))->erase(signalIterator);
//             SignalRingBuffer::count_signal_drops++; // for statistics
//             continue;       // continue to examine next signalQueue slot
//         }
//         necv = (*(necvOrigin) + nslot) < necv ? (*(necvOrigin) + nslot) : necv;
//         signalIterator ++;
//     }
//     if ( (*(signalQueueOrigin + nslot))->empty())
//     {
//         delete( *(signalQueueOrigin + nslot) );  // toss the empty vencto<int32_t> object
//         (*(signalQueueOrigin + nslot)) = nullptr;  // show signal queue unoccupied
//     }
//     (*(necvOrigin + nslot)) = necv;
// }

// void   Neurons::aggregate_queued_signals(int32_t nslot, int32_t clock)
// {
//     // any incoming signal will be for the future
//     // TODO: determine how and when to look through enqueued signals to avoid overhead of scanning
//     // loop through all of the signals and determine if neuron cascades - they are not sorted

//     auto signalIterator = (*(signalQueueOrigin + nslot))->begin();
//     // signals are shorts; 1,000 is basic value; 12,000 is cascade value
//     // signals older than aggregation window are ignored; they halve each tick.
//     short signalAccumulator{0};    // used to accumulate signals towards cascade
//     // pre-compute clock comparisons for speed
//     for (int32_t i=0; i < aggregation_window; i++)
//     {   // build aggregation window clock range for fast comparison - relative array is negative numbers
//         // [0] = -1; [1] = -2; [2] = -3 and so on
//         absolute_aggregation_array[i] = clock + relative_aggregation_array[i];
//     }
//     // next event must happen after current clock value.
//     *(necvOrigin + nslot) = (*(necvOrigin + nslot) <= clock ) ? int32_t_MAX : *(necvOrigin + nslot);
//     while (signalIterator != (*(signalQueueOrigin + nslot))->end())
//     {   // capture the clock value for the current signal
//         signalClock = ( *(SignalRingBuffer::clockBuffer + *signalIterator));
//         // eliminate any signals that are too old
//         if ( signalClock  < absolute_aggregation_array[aggregation_window - 1] )
//         {
//             signalIterator = (*(signalQueueOrigin + nslot))->erase(signalIterator);
//             SignalRingBuffer::count_signal_drops++;     // for statistics
//             // signalIterator is advance to next slot by erase()
//             continue;
//         }
//         if ( signalClock > clock)   // is signal beyond current clock tick? Cannot cascade
//         {
//             *(necvOrigin + nslot) = (signalClock < *(necvOrigin + nslot)) ? signalClock : *(necvOrigin + nslot);
//         }
//         else
//         {
//             for (int32_t i = 0; i < aggregation_window; i++)
//             {
//                 if ( (*(SignalRingBuffer::clockBuffer + *(signalIterator))) ==
//                         absolute_aggregation_array[i])
//                 {
//                     signalAccumulator +=
//                     (*(SignalRingBuffer::sizeBuffer + (*signalIterator))/window_reduction_factors[i]);

//                     if ( signalAccumulator >= cascade_threshold )
//                     {
//                         notify_dependents(nslot, clock);
//                         signalIterator++;
//                         // purge will just do a complete rescan of the signal queue and purge any signals less than refractory
//                         // also, will recapture the lowest necv
//                         purge_signals( nslot );
//                         break;  // and exit the current scan after a cascade.
//                     }
//                 }
//             }
//         }
//         signalIterator++;
//     }
//     return ;


} // end of neurons namespace
