#ifndef DBUSCONNECTOR_H
#define DBUSCONNECTOR_H

#include <QObject>
#include <QDBusInterface>

#define SERVICE_NAME "org.logitech.Headset.Power"

class DBUSConnector : public QObject
{
    Q_OBJECT

public:
    explicit DBUSConnector(QObject *parent = nullptr);

    QDBusInterface *iface;

    void start(const QString &name);
    bool isValid();

public slots:
    void setLighting(bool onoff);

    int getVoltage();
    int getSoC();
    bool getCharging();
    bool getOnline();
    bool getLighting();

signals:
    void voltageChanged(int v);
    void socChanged(int soc);
    void chargingChanged(bool ischarging);
    void onlineChanged(bool isonline);
    void lightingChanged(bool onoff);
    void buttonPressed(int idx, bool onoff);
};

#endif // DBUSCONNECTOR_H
