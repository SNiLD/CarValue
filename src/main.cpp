#include "CarValueApplication.h"

#include <QDateTime>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QtGlobal>


void MessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QFile file("logs/log.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QString typeString;

    switch (type)
    {
        case QtDebugMsg:
            typeString = "Debug";
            break;
        case QtWarningMsg:
            typeString = "Warning";
            break;
        case QtCriticalMsg:
            typeString = "Critical";
            break;
        case QtFatalMsg:
            typeString = "Fatal";
            break;
    }

    QTextStream stream(&file);
    stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << " [" << typeString << "] " << message << endl;
}


int main(int argc, char** argv)
{
    qInstallMessageHandler(MessageHandler);
    CarValueApplication application(argc, argv);
    return application.exec();
}
