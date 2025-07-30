#include "aTCN.h"
#include "Neurons.h"
#include "Connections.h"
#include "SignalRingBuffer.h"
#include "Tcnconstants.h"
#include "LVIT.h"
#include "LV4.h"
#include "LV2.h"
#include "LV1.h"

//#include "aTCNManager.h"

using namespace tcn;
using namespace neurons;
using namespace conns;
using namespace srb;
using namespace tcnconstants;

aTCN::aTCN(int tcnet=0)
{

    // ctor
    // allocate all of the required neurons, connections, and signals
    // to support the tcn structure.

    Neurons neurons = Neurons(TCNConstants::arrayOfTCNs[tcnet][tcn_neurons]);
    Connections connections =  Connections(TCNConstants::arrayOfTCNs[tcnet][tcn_connections]);
    SignalRingBuffer srb = SignalRingBuffer(TCNConstants::arrayOfTCNs[tcnet][tcn_signals]);


    // build the initial neuron temporal connection network
    buildConnectionNetwork(LVIT_net, LV4_net, LV2_net, LV1_net);
    process(neurons, connections, srb, tcnet);
}

 
 aTCN::~aTCN()
 {
     // this destructor will clean up when all is done
 }
 void aTCN::buildVNet (int tcnet=0)
 {
     //build static V1,2,4,IT and sixpack structure
     LVIT_net = new LVIT(TCNConstants::arrayOfTCNs[tcnet][tcn_IT]);
     LV1_net  = new LV1(TCNConstants::arrayOfTCNs[tcnet][tcn_V1]);
     LV2_net  = new LV2(TCNConstants::arrayOfTCNs[tcnet][tcn_V2]);
     LV4_net  = new LV4(TCNConstants::arrayOfTCNs[tcnet][tcn_V4]);
 }
void aTCN::process(Neurons n, Connections c, SignalRingBuffer s, int tcnet=0)
// find all the neurons that need to be processed in this network
{
    // step through all of the neurons looking for aggregations that cascade
    ;
//    for (int i=0; i < TCNConstants::arrayOfTCNs[tcnet][tcn_neurons]; i++)
//    {
//
//        if (tcn[i][j][k].l_f_c <= aTCNManager.master_clock)
//            tcn[i][j][k].integrateSignals(tcn_clock);
//        if (tcn[i][j][k].l_f_c < l_f_c)
//            l_f_c = tcn[i][j][k].l_f_c;     // capture the lowest neuron l_f_c
//    }
// x, y are the planes of neurons; z is the stack of planes
// Access the 3 layers using normal [][][] which is resolved via the pointers
}

void aTCN::buildConnectionNetwork( LVIT* vit, LV4* v4, LV2* v2, LV1* v1)
{
    buildForwardVerticalBranched( vit, v4, v2, v1, VIT_density, VIT_inhibition_ratio );
    buildBackwardVerticalBranched(  vit, v4, v2, v1, V4_density, V4_inhibition_ratio );
    buildForwardVerticalInterconnect( vit, v4, v2, v1, V2_density, V2_inhibition_ratio );
    buildBackwardVerticalInterconnect( vit, v4,  v2, v1, V1_density, V1_inhibition_ratio );
    buildHorizontalInterconnect(vit, v4, v2, v1, L6_density, L6_inhibition_ratio );
    // once connections are all built can tear-down static v-nets
    delete [] LVIT_net;
    delete [] LV1_net;
    delete [] LV2_net;
    delete [] LV4_net;
}
// build all the connections in the network
// This will call the vertcial, horizontal, forward, and backward connection builders

// TODO - forwards connections up the stack - Purkinje
void aTCN::buildForwardVerticalBranched(LVIT* vit, LV4* v4, LV2* v2, LV1* v1, float den, float inhibit)
{
    ;       // for now
}

//// TODO - backwards connections down the stack - Purkinje
void aTCN::buildBackwardVerticalBranched(LVIT* vit, LV4* v4, LV2* v2, LV1* v1, float den, float inhibit)
{
    ;       // for now
}
//
//// TODO - forward inter-layer connections - Pyramidal
void aTCN::buildForwardVerticalInterconnect(LVIT* vit, LV4*  v4, LV2* v2, LV1* v1, float den, float inhibit)
{
    ;       // for now
}
//
//// TODO - backware inter-layer connections - Pyramidal
void aTCN::buildBackwardVerticalInterconnect(LVIT* vit, LV4* v4, LV2* v2, LV1* v1, float den, float inhibit)
{
    ;       // for now
}
//
//// TODO - intra-layer distribution connectivity - intra-layer distribution
void aTCN::buildHorizontalInterconnect(LVIT* vit, LV4* v4, LV2* v2,LV1* v1, float den, float inhibit)
{
    ;       // for now
}
