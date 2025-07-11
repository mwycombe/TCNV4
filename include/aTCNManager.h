#ifndef ATCNMANAGER_H
#define ATCNMANAGER_H

#include <iostream>
#include <limits>
#include "aTCN.h"
#include <vector>

#include "TCNConstants.h"

using namespace tcn;
using namespace std;
using namespace neurons;
using namespace srb;
using namespace tcnconstants;

namespace tcnmanager
{
    /*
        Manages the allocation of any TCN creation, manages the global clock.
        Provides addressability via a class method for anyone who wants
        to locate the tcn
        We age older signals on the signal queue for every neuron we publish.
        The signal reduction factor is designed to reduce signal to 1/32nd
        of it's value before being discarded.

        #########################################################
        # Table of aggregation width vs. signal reduction factor
        #   agw     srdf
        #   5       0.5
        #   6       0.565
        #   7       0.61
        #   8       0.65
        #   9       0.68
        #   10      0.71
        ########################################################
    */
    /*  Global clock management
        This is the current clock value all tcn's should be processing
        It should be the lowest l_f_c from any neuron in any tcn.
    */

    //  These are global; anyone can reference or set them //
    int master_clock{0}; // starts out at zero
    int l_f_c{0};        // lowest future clock
    int n_l_f_c{0};      // next lowest future clock

    class aTCNManager
    {
    public:
        aTCN *firstTCN;
        // tcn be processed
        // for now this is a global for a single tcn. In the future it will become a local for multi-thread support
        static aTCN current_tcn;

        /*  these are all moved to Tcnconstants as they are truly global
            for all TCNs

            // tcn global clock parameters
            static  int tpm = 5;        // ticks per millisecond for tcn's
            static  int agw = 5;        // how many clock ticks are eligible for signal aggregation
            static  float sdf = 0.5;   // signal decay factor - reduces to 1/32 over 5 ticks

            // threshold to cascade a node
            static  float cascade = 125000;

        */
        /*
            // connection STP aging factors
            static  float   STP_decay   =   0.41;       // exponential decay factor
            static  float   STP_boost   =   0.25;       // repeat signal lift
            static  float   max_signal  =   220;        // limit to signal boost
            static  float   min_signal  =   100;        // base value for a signal

            static  int STP_aging_interval  =   60*1000*tpm;    // in clock tick units ( one minute )
            static  int STP_intervals       =   15;             // intervals over which to age STP

            // When we process the tcns, just look for the tcn that has the least l_f_c lowest_future_clock
        */
        //    aTCNManager ();

        void select(int tcn_no);
        /**
         * @brief   Selects the next TCN to be processed by the TCNManager
         *          4/8/2025: Just picks the only one for now
         *
         */
        aTCNManager(tcn_no);

        ~aTCNManager()
        /**
         * @brief   Destroy the TCNManager and all TCNs freeing up the heap
         *
         */
        {
            // tcn being processed
            // Currently this is a singular global.
            // It will have to become a multi-valued local when we go multi-thread
            tcn_list.pop_back();
            delete firstTCN;
        }

    private:
        // array of tcns to be managed - for now just one
        vector<aTCN *> tcn_list;
    };

} // end tcnmanager namespace
#endif // ATCNMANAGER_H
