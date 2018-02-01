#ifndef PEERINFO_H
#define PEERINFO_H

#include <QtCore/QMetaType>
#include <QHostAddress>

class PeerInfo
{
public:
    PeerInfo() {}
    PeerInfo(const QHostAddress &addr, const quint16 &portNumber)
        : address(addr), port(portNumber) {}
    QHostAddress address;
    quint16 port;
    quint64 key;
    quint64 secretKey;
    bool operator==(const PeerInfo &other) const {
        return address == other.address
                && port == other.port;
    }
};

Q_DECLARE_METATYPE(PeerInfo)

#endif // PEERINFO_H
