// TCNConstants.h
#ifndef TCNCONSTANTS_H_INCLUDED
#define TCNCONSTANTS_H_INCLUDED
#include <iostream>
#include <climits>
#include <cstdint>

/**
 * TESTING_MODE is defined to enable testing features.
 * It can be used to conditionally compile test-specific code.
 * This should be defined in the build system or at the top of the main testing file.
 */

#define TESTING_MODE
#undef TESTING_MODE

/**
 * July 2025
 * As of C++17, inline constexpr with have external linkage.
 * Compiler will treat all inline definitions as a single reference to de-dupe.
 * All files that include this will reference a singular instance of each global.
 * 
 * Users must use tcnconstants:: scope reference.
 */
namespace tcnconstants
{
    /**
     *  Any constants used in TCN should be defined here.
     *  Any global items should use inline (not static) per C++17 specifications for hdr files.
     *  
     */
    // create inline variable in header file which makes it a singleton C++17 feature


    // none of these can be made global - forbidden for constexpr which are compile-time constants */

    inline constexpr int32_t IT{5};         // this is the fan-out specification between layers
    inline constexpr int32_t V4{IT * IT};
    inline constexpr int32_t V2{V4 * IT};
    inline constexpr int32_t V1{V2 * IT};
    inline constexpr int32_t L1_width{6};
    inline constexpr int32_t L1_depth{2};
    inline constexpr int32_t Lx_width{6};
    inline constexpr int32_t Lx_depth{4};
    inline constexpr int32_t sixpack_size{L1_width * L1_depth + 5*(Lx_width * Lx_depth)};
    inline constexpr int32_t neuron_count{sixpack_size * (IT + V4 + V2 + V1)};

    // the following are guesstimates for the size of the TCN
    inline constexpr int32_t neuron_signal_ratio{10};                                  // average count of signals/neuron
    inline constexpr int32_t signal_count{neuron_count * neuron_signal_ratio};         // average of 10 per neuron
    inline constexpr int32_t neuron_connection_ratio{30};                              // average count of connections per neuron
    inline constexpr int32_t connection_count{neuron_count * neuron_connection_ratio}; // thirty connections per neuron

    // density and inhibition ratios
    inline constexpr float VIT_density{0.3};
    inline constexpr float V4_density{0.3};
    inline constexpr float V2_density{0.3};
    inline constexpr float V1_density{0.3};
    inline constexpr float L6_density{0.3};
    inline constexpr float VIT_inhibition_ratio{0.4};
    inline constexpr float V4_inhibition_ratio{0.4};
    inline constexpr float V2_inhibition_ratio{0.4};
    inline constexpr float V1_inhibition_ratio{0.4};
    inline constexpr float L6_inhibition_ratio{0.4};

    // What is this used for?
    // enum tcnLabel
    // {
    //     tcn_IT,
    //     tcn_V4,
    //     tcn_V2,
    //     tcn_V1,
    //     sp_L1_w,
    //     sp_L1_d,
    //     sp_Lx_w,
    //     sp_Lx_d,
    //     sp_size,
    //     tcn_neurons,
    //     tcn_sig_ratio,
    //     tcn_signals,
    //     tcn_conn_ratio,
    //     tcn_connections,
    //     tcn_max
    // };

    /**
     * July 2025
     * Need to revalidate these parameters, especially the aggregation aging period.
     * Can a neuron really recieve enough signals within 2 msec?
     * Tetanic pulse sizes and how they are appied.
     * A series of high speed pulses such as when a muscle should contract.
     *  Must come from multiple different neurons because of neuron refractor period.
     *  Frequency should be 20-50 stimuli per second.
     *  80 msec at 35 Hz is unfused; 50-100 Hz is fused tetanus.
     *  Lower freq. is twitching; higher freq. is sustained contraction without relaxation.
     *  With this freq. repeated tetanus could contribute to muscle memory.
     */

    /*
    [Neuron section]
    */

    inline constexpr int32_t aggregatorWidth{2};     // period when signals must arrive to be counted
    inline constexpr int32_t refractoryWidth{5};     // refractory period width
    inline constexpr int32_t purgeThreshold{10};     // don't purge signals below purgeThreshold - perf.

    inline constexpr int16_t cascadeThreshold{12000}; // value to cause neuron to cascade aka 12 mv.
    inline constexpr int32_t msecs_refractory_period{5};     // 5 msecs; 1 spike + 4 recovery
    inline constexpr int32_t msecs_aggregation_window{5};    // 5 msec to aggregate incoming signals
    inline constexpr int32_t aggregation_decay_factor{2};   // 1/32 after 5 msecs
    inline constexpr int32_t ticks_per_msec{1};     // millisec clock rate
                                                    // all msec measurements will use this scaling factor

    /*
    [ STP section ]
    */
    inline constexpr   int16_t base_signal_size {1000};       // 1000 to one - 8x fits in a int16_t for ltp
    inline constexpr   int16_t stp_signal_limit {2 * base_signal_size};     // limited to doubling
    inline constexpr   int16_t stp_tetanic_pulse_size {10};     // tetanic pulse size
   
    // compute how many signal boosts we get per tetanic pules spike
    inline constexpr   int16_t stp_units_per_tetanic_pulse {(stp_signal_limit - base_signal_size)/stp_tetanic_pulse_size};
    inline constexpr   int32_t stp_decay_time {20 * 60 * 1000};   // 20 minutes in millisecs
   
    // each decay interval reduces stp value by one unit - approx 120 msecs
    inline constexpr   int16_t stp_unit_decay_interval {stp_decay_time / (stp_signal_limit - base_signal_size)};

    /*
    [ LTP section [
    */
    inline constexpr   int16_t ltp_signal_limit {8 * base_signal_size};  // limited to octupling
    inline constexpr   int32_t ltp_decay_time {10 * 60 * 60 * 1000};    // 10 hours in millisecs

    // each decay interval reduces ltp value by one unit - 1800 msecs
    inline constexpr   int16_t ltp_unit_decay_interval {ltp_decay_time / (ltp_signal_limit - base_signal_size)};


 

    class TCNConstants
    {
        public:

        // rely on default constructor
    };
}   // end of tcnconstants namespace
#endif // TCNCONSTANTS_H_INCLUDED
