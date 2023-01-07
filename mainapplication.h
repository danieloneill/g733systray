#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QObject>
#include <QSvgRenderer>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>

#include "dbuswatcher.h"

class MainApplication : public QObject
{
    Q_OBJECT

    QMediaPlayer    m_player;
    QAudioOutput    m_audioOutput;

    DBUSWatcher     m_dbw;
    QSystemTrayIcon m_trayIcon;

    QTimer  m_chargingAnimationTimer;
    int     m_chargingAnimationFrameNumber;

    QImage  m_icon_offline;
    QImage  m_icon_online;
    QImage  m_icon_online_norgb;

    QList<QSvgRenderer*> m_icon_batteryLevels;
    QList<QSvgRenderer*> m_icon_batteryCharging;

    QList< QPair<int, double> > m_curve_discharging;
    QList< QPair<int, double> > m_curve_charging;

    bool m_connected;
    bool m_online;
    bool m_lighting;
    bool m_charging;
    int m_chargeLevel;
    int m_voltage;

    QString m_tooltip;
    QVariantMap m_state;

public:
    explicit MainApplication(QObject *parent = nullptr);

public slots:
    void playSoC();

private slots:
    void serviceActive();
    void serviceInactive();

    void updateTooltip();
    void updateIcon();

    void trayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void updateChargeState();
    void playClip(const QUrl &path);

signals:

};

#endif // MAINAPPLICATION_H
