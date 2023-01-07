#include <QCoreApplication>
#include <QDebug>

#include "dbuswatcher.h"

DBUSWatcher::DBUSWatcher(QObject *parent)
    : QObject{parent},
      m_c{nullptr},
      m_sw{nullptr},
      m_conn{QDBusConnection::connectToBus(QDBusConnection::SessionBus, "G733TestWatcher")}
{
    if (!m_conn.isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        qApp->exit(-1);
        return;
    }

    m_sw = new QDBusServiceWatcher(this);
    m_sw->addWatchedService("org.logitech.Headset.Power");
    m_sw->setConnection(m_conn);
    //m_sw->setWatchMode(QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration);
    connect( m_sw, &QDBusServiceWatcher::serviceRegistered, this, &DBUSWatcher::serviceRegistered );
    connect( m_sw, &QDBusServiceWatcher::serviceUnregistered, this, &DBUSWatcher::serviceUnregistered );
    serviceRegistered("initial startup");
}

DBUSWatcher::~DBUSWatcher()
{
    if( m_sw )
        m_sw->deleteLater();

    if( m_c )
        m_c->deleteLater();
}

void DBUSWatcher::serviceRegistered(const QString &serviceName)
{
    qDebug() << "QDBUSWatcher::registered:" << serviceName;
    if( m_c )
        m_c->deleteLater();
    m_c = nullptr;

    m_c = new DBUSConnector(this);
    m_c->start(SERVICE_NAME);

    if( m_c->isValid() )
        emit serviceActive();
}

void DBUSWatcher::serviceUnregistered(const QString &serviceName)
{
    qDebug() << "QDBUSWatcher::unregistered:" << serviceName;
    if( m_c )
    {
        emit serviceInactive();
        m_c->deleteLater();
        m_c = nullptr;
    }
}
