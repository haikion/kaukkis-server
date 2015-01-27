#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMutex>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <X11/extensions/XTest.h>

class Server : public QObject
{
   Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();

    bool listen(const unsigned port);
signals:

public slots:
    void newConnection();

private:
    QTcpServer server_;
    QTcpSocket* socket_;
    unsigned messageSize_;
    Display* display_; //unique_ptr doesn't work.
    QMutex newConnectionMutex_;

    void pressKey(const unsigned keyCode);
    void releaseKey(const unsigned keyCode);
    unsigned QtKeyToXKeySym(Qt::Key key);
};

#endif // SERVER_H
