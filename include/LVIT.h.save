#ifndef LVIT_H_INCLUDED
#define LVIT_H_INCLUDED

#include "TCNConstants.h"
#include "SixPack.h"

using namespace std;
using namespace tcnconstants;

class LVIT
{
public:
    static const int Vid{6};
    SixPack * m_sptr;
    int m_tcnet;
    LVIT(int tcnet)
    {
        m_tcnet = tcnet;
        m_sptr = new SixPack(Vid, TCNConstants::arrayOfTCNs[tcnet][sp_size]);
    }
    ~LVIT()
    {
        delete m_sptr;
    }
};

#endif // IT_H_INCLUDED
