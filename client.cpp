#include <QtWidgets>
#include <QtNetwork>

#include "client.h"
#include "bonjourservicebrowser.h"
#include "bonjourserviceresolver.h"
#include "utility.h"


Client::Client(QWidget *parent)
    : QDialog(parent), bonjourResolver(0)
{
    BonjourServiceBrowser *bonjourBrowser = new BonjourServiceBrowser(this);
    
    statusLabel = new QLabel(tr(""));
    statusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabels(QStringList() << tr("Peers found"));
    myPeersList = new QList<BonjourRecord>();
    peerInfoList = new QList<PeerInfo>();
    connect(bonjourBrowser, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
            this, SLOT(connectToPeer(const QList<BonjourRecord> &)));

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    tcpSocket = new QTcpSocket(this);

    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readExchangeKey()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(treeWidget, 0, 0, 2, 2);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Bonjour Client"));
    treeWidget->setFocus();
    bonjourBrowser->browseForServiceType(QLatin1String("_bananapeer._tcp"));
}

void Client::sendExchangeKeyToPeer(BonjourRecord record) {
    //do for all peers found
    blockSize = 0;
    tcpSocket->abort();
    if (!bonjourResolver) {
        bonjourResolver = new BonjourServiceResolver(this);
        connect(bonjourResolver, SIGNAL(bonjourRecordResolved(const QHostInfo &, int)),
                this, SLOT(connectToServer(const QHostInfo &, int)));
    }
    bonjourResolver->resolveBonjourRecord(record);
}

void Client::connectToServer(const QHostInfo &hostInfo, int port)
{
    const QList<QHostAddress> &addresses = hostInfo.addresses();
    if (!addresses.isEmpty()) {
        QHostAddress address = addresses.first();
        PeerInfo *peerInfo = new PeerInfo(address, port);
        if (!peerInfoList->contains(*peerInfo)) {
            tcpSocket->connectToHost(address, port);
            //compute exchange key
            int a = getRandom();
            quint64 A = Utility::getExchangeKey(a);
            QString text = QString::number(A);
            qDebug() << "Key to exchange: " << text;
            statusLabel->setText(text);

            peerInfo->key = a;
            peerInfoList->append(*peerInfo);
        }
    }
}

void Client::readExchangeKey() {
    qDebug() << "Received new connection from peer";

    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_0);
    quint64 B;
    in >> B;
    qDebug() << "Received key: " << B;

    QList<PeerInfo>::iterator it = peerInfoList->begin();
    for (; it != peerInfoList->end(); ++it) {
        PeerInfo& pi = *it;
        if ((pi.address == tcpSocket->peerAddress()) && (pi.port == tcpSocket->peerPort())) {
            statusLabel->setText(tr(""));
            quint64 a = pi.key;
            pi.secretKey = Utility::powmodp(B,a);
            qDebug() << "Secret key = " << pi.secretKey;

            break;
        }
    }

    connect(tcpSocket, SIGNAL(disconnected()),
            tcpSocket, SLOT(deleteLater()));
    tcpSocket->disconnectFromHost();
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Bonjour Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Bonjour Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the bonjour server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Bonjour Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

}

void Client::connectToPeer(const QList<BonjourRecord> &list)
{
    //do something
    treeWidget->clear();
    myPeersList->clear();
    peerInfoList->clear();
    foreach (BonjourRecord record, list) {
        qDebug() << "Found peer: " << record.serviceName;
        myPeersList->append(record);
        QVariant variant;
        variant.setValue(record);
        QTreeWidgetItem *processItem = new QTreeWidgetItem(treeWidget,
                                                           QStringList() << record.serviceName);
        processItem->setData(0, Qt::UserRole, variant);
    }
    //once a peer has been detected, connect to it as done in the requestNewFortune method
    QList<BonjourRecord>::iterator it = myPeersList->begin();
    for (; it != myPeersList->end(); ++it) {
        BonjourRecord& br = *it;
        sendExchangeKeyToPeer(br);
        myPeersList->erase(it);
    }
}

int Client::getRandom()
{
    srand(time(NULL));
    int num = rand() % 90 + 10; //from 10 to 100
    return num;
}

void Client::performDiffieHellman()
{
    int a = getRandom();
    quint64 A = Utility::getExchangeKey(a);
    qDebug() << "Key to exchange: " << A;
}
