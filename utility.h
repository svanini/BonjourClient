#ifndef UTILITY_H
#define UTILITY_H

#include <QtGlobal>

class Utility {
public:
    static qint64 mul_mod_p(quint64 a, quint64 b);
    static quint64 power_mod_p(quint64 m, quint64 n);
    static quint64 powmodp(quint64 k, quint64 l);
    static quint64 getExchangeKey(quint64 a);
};

#endif // UTILITY_H
