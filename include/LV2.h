#ifndef LV2_H_INCLUDED
#define LV2_H_INCLUDED

#include "TCNConstants.h"
#include "SixPack.h"
class LV2
{
public:
    static const int Vid{2}; //Which vid layer
    SixPack * m_sptr;
    int m_tcnet;
    LV2(int tcnet)
    {
        m_tcnet = tcnet;
        m_sptr = new SixPack(Vid, tcnconstants::arrayOfTCNs[tcnet][sp_size]);
    }
    ~LV2 ()
    {
        delete m_sptr;
    }
};

#endif // V2_H_INCLUDED
