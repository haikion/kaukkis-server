#include <QCoreApplication>
#include <QCommandLineParser>
#include "server.h"

int main(int argc, char *argv[])
{
    const unsigned DEFAULT_PORT = 4385;

    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("kaukkis server");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("Server for the kaukkis network output plugin.");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption portOption({"p","port"}, "Sets port to listen. Default: "
                                  + QString::number(DEFAULT_PORT), "port");
    portOption.setDefaultValue(QString::number(DEFAULT_PORT));
    parser.addOption(portOption);
    parser.process(a);

    Server server;
    server.listen(parser.value("port").toUInt());

    return a.exec();
}
