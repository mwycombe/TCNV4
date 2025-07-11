// SixPack.cpp

#include <iostream>
#include <cassert>
#include "TCNConstants.h"
#include "SixPack.h"

using namespace std;
using namespace neurons;
using namespace tcnconstants;


SixPack::SixPack (int vid, int tcnet)
{
    m_vid = vid;    // which vid layer this six pack is in
    m_tcnet = tcnet;
    m_origin = Neurons::allocateNeurons(TCNConstants::arrayOfTCNs[sp_size]);
}
int SixPack::topIndex()
{
    return( m_origin + TCNConstants::arrayOfTCNs[m_tcnet][sp_size] );
}
