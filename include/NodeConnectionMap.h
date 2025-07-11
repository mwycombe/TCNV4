#ifndef NODECONNECTIONMAP_H_INCLUDED
#define NODECONNECTIONMAP_H_INCLUDED

#include <bitset>
#include <iostream>

#include "aTCN.h"
#include "TCNConstants.h"

using namespace std;
using namespace tcn;
using namespace tcnconstants;

const size_t neuronCount = static_cast<size_t> (TCNConstants::arrayOfTCNs[0][tcn_neurons]);

class NodeConnectionMap
{
private:
    bitset<neuronCount> nodesIn;
    bitset<neuronCount> nodesOut;

public:

NodeConnectionMap (int tcnet)
{

    size_t i;
    for (i=0; i< neuronCount; ++i)
    {
        nodesIn.reset(i);
        nodesOut.reset(i);
    }
}
    void setInNode(size_t v)
    {   nodesIn.set(v); }
    void setOutNode(size_t v)
    {   nodesOut.set(v);}
    int inCount()
    {   return nodesIn.count(); }
    int outCount()
    {   return nodesOut.count(); }
    bool testInNode(long long unsigned v)
    {   return nodesIn.test(v);}
    bool testOutNode(long long unsigned v)
    {   return nodesOut.test(v);}
};

#endif // NODECONNECTIONMAP_H_INCLUDED
