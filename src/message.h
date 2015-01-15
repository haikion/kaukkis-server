#ifndef MESSAGE_H
#define MESSAGE_H
#include <QtNetwork/QTcpSocket>
#include <QByteArray>

using namespace std;

class Message
{
public:
    enum class Action: qint8
    {
        KEY_PRESS,
        KEY_RELEASE
    };

    Message(Action action, Qt::Key key);
    Message(QByteArray serializedMessage);
    QByteArray serialize() const;
    void deSerialize(QByteArray serializedMessage);

    Action action() const;
    Qt::Key key() const;

private:
    QPair<qint8, unsigned> values_;
};

#endif // MESSAGE_H
