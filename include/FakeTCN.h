#ifndef FAKETCN_H
#define FAKETCN_H

namespace tcn
{
    typedef int fcv;    // future clock value
    fcv l_fcv;          // lowest future clock value
    fcv nl_fcv;         // next lowest fcv
    int masterClock;    // for testing
    constexpr int neuronCount{1000000};
}

#endif // FAKETCN_H
