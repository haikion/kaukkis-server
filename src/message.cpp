#include "message.h"


Message::Message(Action action, Qt::Key key)
{
    values_.first = static_cast<qint8>(action);
    values_.second = static_cast<unsigned>(key);
}

Message::Message(QByteArray serializedMessage)
{
    deSerialize(serializedMessage);
}

QByteArray Message::serialize() const
{
   QByteArray data;
   QDataStream stream(&data, QIODevice::WriteOnly);

   stream << values_;
   return data;
}

void Message::deSerialize(QByteArray serializedMessage)
{
    QDataStream stream(&serializedMessage, QIODevice::ReadOnly);
    stream >> values_;
}

Message::Action Message::action() const
{
    return static_cast<Action>(values_.first);
}

Qt::Key Message::key() const
{
    return static_cast<Qt::Key>(values_.second);
}
