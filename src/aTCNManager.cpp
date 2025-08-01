#include <iostream>
#include <vector>
#include <climits>
#include <cassert>

#include "aTCNManager.h"
#include "aTCN.h"

using namespace tcnconstants;
using namespace std;
using namespace tcn;

int aTCNManager::master_clock{0};
int aTCNManager::l_f_c{INT_MAX};   // any plausible clock value will be less than this initial value
int aTCNManager::n_l_f_c{INT_MAX}; // any plausible clock value will be less that this initial value
aTCN aTCNManager::current_tcn;     // tcn being processed

extern aTCN *firstTCN;

aTCNManager::aTCNManager(int tcnet = 0)
{
    // ctor
    //  for now, allocate and enqueue a single tcn

    firstTCN = new aTCN(tcnet); // first tcn in the list of tcns.
    tcn_list.push_back(firstTCN);
}

void aTCNManager::select()
// Jan 2019, single TCN, single thread
{
    l_f_c = INT_MAX; // force smaller tcn lfc to be selected.
    current_tcn = *tcn_list[0];
}
