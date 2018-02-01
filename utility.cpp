#include <QtGlobal>
#include "utility.h"

#define P 0xffffffffffffffc5ull
#define G 5

/*class Utility {
public:
    static qint64 mul_mod_p(quint64 a, quint64 b);
    static quint64 power_mod_p(quint64 m, quint64 n);
    static quint64 powmodp(quint64 k, quint64 l);
    static quint64 getExchangeKey(quint64 a);
};*/

qint64 Utility::mul_mod_p(quint64 a, quint64 b)
{
    quint64 m = 0;
    while(b) {
        if(b&1) {
            quint64 t = P-a;
            if ( m >= t) {
                m -= t;
            } else {
                m += a;
            }
        }
        if (a >= P - a) {
            a = a * 2 - P;
        } else {
            a = a * 2;
        }
        b>>=1;
    }
    return m;
}

quint64 Utility::power_mod_p(quint64 m, quint64 n){
    if (n == 1) {
        return m;
    }
    quint64 t = Utility::power_mod_p(m, n>>1);
    t = Utility::mul_mod_p(t, t);
    if (n % 2) {
        t = Utility::mul_mod_p(t, m);
    }
    return t;
}

quint64 Utility::powmodp(quint64 k, quint64 l) {
    if (k > P)
        k%=P;
    return Utility::power_mod_p(k, l);
}

quint64 Utility::getExchangeKey(quint64 a) {
    quint64 A = Utility::powmodp(G, a);
    return A;
}
