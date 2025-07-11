#ifndef LV1_H_INCLUDED
#define LV1_H_INCLUDED

#include "TCNConstants.h"
#include "SixPack.h"

// remove using namespace per recommendations.
class LV1
{
public:
    static const int Vid{1};    // which vid layer
    SixPack * m_sptr;
    int m_tcnet;
    LV1(int tcnet)
    {
        m_tcnet = tcnet;
        m_sptr = new SixPack(Vid, arrayOfTCNs[tcnet][sp_size]);
    }
    ~LV1 ()
    {
        delete m_sptr;
    }
};

#endif // V1_H_INCLUDED
