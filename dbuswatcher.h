#ifndef DBUSWATCHER_H
#define DBUSWATCHER_H

#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QObject>

#include "dbusconnector.h"

class DBUSWatcher : public QObject
{
    Q_OBJECT

    DBUSConnector       *m_c;
    QDBusServiceWatcher *m_sw;
    QDBusConnection     m_conn;

public:
    explicit DBUSWatcher(QObject *parent = nullptr);
    ~DBUSWatcher();

    DBUSConnector *connector() { return m_c; }

private slots:
    void serviceRegistered(const QString &serviceName);
    void serviceUnregistered(const QString &serviceName);

signals:
    void serviceActive();
    void serviceInactive();
};

#endif // DBUSWATCHER_H
