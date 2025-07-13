#ifndef ATCN_H
#define ATCN_H
#include <climits>

#include "Neurons.h"
#include "SixPack.h"
#include "Tcnconstants.h"
#include "LVIT.h"
#include "LV4.h"
#include "LV2.h"
#include "LV1.h"

// using in hdr files is frowned up; use specific references.



namespace tcn
{

    /*      aTCN is the structure that holds all of the neurons, connections, and signals for a given
            Temporal Connection Network. It is supervised by the TCNManager which may instantiate and synchronize one or more TCNs.
            The TCNManager is the keeper of the global clock. At the end of a process sweep a TCN informs the TCNManager
            of the next clock click when useful work can be actioned by the TCN.

            JULY 2025 update:

            x64 ptrs are now 8 bytes, not 4. This increases some storge rqmts.
            Neurons and others can use C++ structs as they are just members without
            constructors or member functions.
            Neuron slot numbers and clock times use int32_t for now to limit storage needs.
            Vectors are a class of array container and are created on the heap.
            Ergo, neurons, connections, signals are all created as vectors of structs.
            The signal vector is used as a ring buffer; when we get to the end, we wrap to the beginning.

            Each connection remembers the last time it created a signal so it can perform the 
            correct STP and LTP aging when computing the strength of the signal delivered. 
            If the cascade threshold is not reached all current and past signals are dropped from
            the neuron signal queue - but they remain in the signal ring buffer for reuse in the future.

            

            Each neuron keeps track of its oldest signal. The TCN keeps track of the oldest signal
            from all of the neurons and his forms the next time to scan the signal queues.

            UPDATED TCN STRUCTURE CONCEPT JAN 2021:
            As most all NNs to date have been 2D networks, TCN will start out as a 2D network, for
            simplicity of construction, debugging, and monitoring.

            A 4-layer structure labeled IT, V4, V2, V1 will mimic the visual cortex
            The Fan out ratio will be 5:1 between each level.
            If IT = 5 entities, then V4 = 25, V2 = 125, V1 = 625.
            The basic unit in each layers is a SixPack L1 - L6, all the same size.
            L1 = 9; L2-6 = 18: Total neurons in a SixPack = 99.

            Each neuron has a unique ID (its index in the dynamic array) which can be used to locate the neuron
            for handling connection generation and signal passing.

            The  neurons are created in a vector dynamically on the heap. Multiple dynamic
            arrays simulate a C-type array of a classes. C++ classes include extra default pointers to constructors,
            copy constructors, and destructors so were eschewed in favor of structs.

            The connection process then creates all of the connections
            that represent the TCN interconnections.

            The TCN provides methods for a requestor to get the TCN reference
            indices for passing signals between neurons. These will be the index numbers in the relevant arrays.

            The TCN also provides the master clock for the network so every object can
            find out the current time.

            When processing has completed, the TCN master clock is advanced to
            the next useful clock time, when something can happen. As connections, neurons, and signals are processed,
            they each update the next clock time with their oldest clock tick that requires action.

            The TCN is contained in a flat list. The ID of each neuron is the same
            as it's index in the list.

            Logically the TCN is an equi-spaced lattice of x * y planes,
            stacked z deep. The only process that cares about this is the TCN
            initialize that builds connection objects that mimic a 3D stack of
            neurons and computes the temporal distances.

            Once the temporal connections have been assigned, there is no further
            need to recognize the spatial characteristics. Only the relative temporal separations matter.

            Example: a tcn[10*20*12] will have twelve layers, each of which is
                a 10x20 = two hundred sized layer. The last node in layer one will
                be the neuron with id 199 (200-1) as C++ has zero indexing.
    */

    /*      Sizing estimates:
            Empty Sizings:
            Neurons:        12 bytes per neuron with empty queues
            Connections:    20 bytes per connection
            Signals:        8  bytes per signal
                                                 10X     20X     30X     40X      50X
            1 million empty neurons requires     12 MB   24 MB    36 MB   48 MB    50 MB
            10 million connections requires     200 MB  400 MB   600 MB  800 MB  1000 MB
            10 million signals requires          80 MB  160 MB   240 MB  320 MB   400 MB
            Neuron queues
                10 million connections requires  40 MB   80 MB   120 MB  160 MB   200 MB
                10 million signals requires      40 MB

            Thus 1 million neurons with 10X connections and signals
                requires                        372 MB  744 MB  1.16 GB 1.48 GB  1.86 GB

            Therefore 1 GB can house ~2 million neurons with 10x connections + signals
    */

    // make sure we start processing with the oldest clocks
    static  int32_t oldestClock {INT32_MIN};
    class aTCN
    {
    public:
        LVIT *LVIT_net;
        LV4 *LV4_net;
        LV2 *LV2_net;
        LV1 *LV1_net;
        // this static is used to hold the next neuron process clock value
        aTCN();    // explicit default constructor
        aTCN(int); // IT, V4, V2, V1, sixpack in TCNConstants
        void buildVNet(int);

        void buildConnectionNetwork(LVIT *, LV4 *, LV2 *, LV1 *);
        void buildForwardVerticalBranched(LVIT *, LV4 *, LV2 *, LV1 *, float, float);
        void buildBackwardVerticalBranched(LVIT *, LV4 *, LV2 *, LV1 *, float, float);
        void buildForwardVerticalInterconnect(LVIT *, LV4 *, LV2 *, LV1 *, float, float);
        void buildBackwardVerticalInterconnect(LVIT *, LV4 *, LV2 *, LV1 *, float, float);
        void buildHorizontalInterconnect(LVIT *, LV4 *, LV2 *, LV1 *, float, float);
        void procees(Neurons, Connections, SignalRingBuffer, int);

        ~aTCN();

        // don't need the getters and setters as members are public
        //        void set_x (int x);
        //        void set_y (int y);
        //        void set_x (int z);
        //        int get_x ();
        //        int get_y ();
        //        int get_z ();

        void process(Neurons, Connections, SignalRingBuffer, int);

        // Build all the connections in the network
        // This will call the vertical horiztonal, forward, and backward connection builders
        // TODO build these algorithms
        //        void buildConnectionNetwork(int x, int y, int z);
        //
        //        // TODO - forward connections up the stack - Purkinje
        //        void buildForwardVerticalBranched(int x, int y, int z, density, inhibitRatio);
        //
        //        // TODO - backward connections down the stack - Purkinje
        //        void buildBackwardVerticalBranched(int x, int y, int z), density inhibitRatio;
        //
        //        // TODO - forward inter-layer connections - Pyramidal
        //        void buildForwardVerticalInterconnect(int x, int y, int z, density, inhibitRati);
        //
        //        // TODO - backward inter-layer connections - Pyramidal
        //        void buildBackwardVerticalInterconnect(int x, int y, int z);
        //
        //        // TODO - intra-layer distribution connectivity - intra-layer distribution
        //        void buildHorizontalINterconnect(int x, int y, int z);

    protected:
    private:
        int i, j, k; // work integers

    public:
        // all member values are public for speed

        int master_clock{0};   // starts at zero
        int l_f_c = INT_MAX;   // we track the next value to advance to for this tcn
        int n_l_f_c = INT_MAX; // and this is the next lower clock tick after l_f_c

    }; // end of class aTCN

} // end of tcn namespace wrapper
#endif // ATCN_H
