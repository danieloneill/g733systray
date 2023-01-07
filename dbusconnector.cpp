#include "dbusconnector.h"

#include <QCoreApplication>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QDebug>

DBUSConnector::DBUSConnector(QObject *parent)
    : QObject{parent}
{

}

void DBUSConnector::start(const QString &name)
{
    if (name != SERVICE_NAME)
        return;

    iface = new QDBusInterface(SERVICE_NAME, "/", "org.logitech.Headset.Power.Service",
                                   QDBusConnection::sessionBus(), this);
    if (!iface->isValid()) {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        return;
    }

    connect(iface, SIGNAL(aboutToQuit()), QCoreApplication::instance(), SLOT(quit()));
    connect(iface, SIGNAL(voltageChanged(int)), this, SIGNAL(voltageChanged(int)));
    connect(iface, SIGNAL(socChanged(int)), this, SIGNAL(socChanged(int)) );
    connect(iface, SIGNAL(buttonPressed(int,bool)), this, SIGNAL(buttonPressed(int,bool)) );
    connect(iface, SIGNAL(onlineChanged(bool)), this, SIGNAL(onlineChanged(bool)) );
    connect(iface, SIGNAL(lightingChanged(bool)), this, SIGNAL(lightingChanged(bool)) );
    connect(iface, SIGNAL(chargingChanged(bool)), this, SIGNAL(chargingChanged(bool)) );
    qDebug() << "Starting up...";
}

bool DBUSConnector::isValid()
{
    return iface->isValid();
}

void DBUSConnector::setLighting(bool onoff)
{
    iface->call("setLighting", onoff);
    if (iface->lastError().isValid())
        qDebug() << "Call failed: " << qPrintable(iface->lastError().message());
}

bool DBUSConnector::getLighting()
{
    QDBusReply<bool> r = iface->call("lighting");
    if( r.isValid() )
        return r.value();
    return false;
}

int DBUSConnector::getVoltage()
{
    QDBusReply<qint32> r = iface->call("voltage");
    if( r.isValid() )
        return r.value();
    return 0;
}

int DBUSConnector::getSoC()
{
    QDBusReply<qint32> r = iface->call("soc");
    if( r.isValid() )
        return r.value();
    return 0;
}

bool DBUSConnector::getCharging()
{
    QDBusReply<bool> r = iface->call("charging");
    if( r.isValid() )
        return r.value();
    return false;
}

bool DBUSConnector::getOnline()
{
    QDBusReply<bool> r = iface->call("online");
    if( r.isValid() )
        return r.value();
    return false;
}
