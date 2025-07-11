#ifndef LV4_H_INCLUDED
#define LV4_H_INCLUDED

#include "TCNConstants.h"
#include "SixPack.h"

using namespace std;
using namespace tcnconstants;

class LV4
{
public:
    static const int Vid{4}; // which vid layer
    SixPack * m_sptr;
    int m_tcnet;
    LV4(int tcnet)
    {
        m_tcnet = tcnet;
        m_sptr = new SixPack(Vid, arrayOfTCNs[tcnet][sp_size]);
    }
    ~LV4 ()
    {
        delete m_sptr;
    }
};

#endif // V4_H_INCLUDED
