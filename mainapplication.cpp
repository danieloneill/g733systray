#include "mainapplication.h"
#include "dbusconnector.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QObject>
#include <QPainter>

MainApplication::MainApplication(QObject *parent)
    : QObject{parent}
{
    m_icon_offline = QImage(":/images/icon-headset-glow-offline.png");
    m_icon_online = QImage(":/images/icon-headset-glow-rgb.png");
    m_icon_online_norgb = QImage(":/images/icon-headset-glow-norgb.png");

    m_connected = false;
    m_online = false;
    m_lighting = false;
    m_charging = false;
    m_chargeLevel = 0;
    m_voltage = 0;

#if QT_VERSION >= 0x060400
    m_player.setAudioOutput(&m_audioOutput);
#endif

    QList<QString> levels;
    levels << "000" << "010" << "020" << "030" << "040" << "050" << "060" << "070" << "080" << "090" << "100";
    for( QString lev : levels )
    {
        QSvgRenderer *img = new QSvgRenderer(this);
        img->load(QString(":/images/battery-%1.svg").arg(lev));
        m_icon_batteryLevels << img;
    }

    QList<QString> frames;
    frames << "020" << "040" << "060" << "080" << "100";
    for( QString lev : frames )
    {
        QSvgRenderer *img = new QSvgRenderer(this);
        img->load(QString(":/images/battery-%1-charging.svg").arg(lev));
        m_icon_batteryCharging << img;
    }

    m_chargingAnimationFrameNumber = 0;
    m_chargingAnimationTimer.setInterval(1000);
    connect( &m_chargingAnimationTimer, &QTimer::timeout, this, &MainApplication::updateIcon );

    QMenu *menu = new QMenu(tr("G733"));

    QAction *action_lighting = new QAction(tr("&Lighting"));
    action_lighting->setCheckable(true);
    action_lighting->setChecked(m_lighting);
    connect( action_lighting, &QAction::triggered, this, [this](bool checked) {
        m_lighting = checked;
        DBUSConnector *c = m_dbw.connector();
        if( c && c->isValid() )
            c->setLighting(m_lighting);

        updateTooltip();
        updateIcon();
    } );

    QAction *action_exit = new QAction(tr("E&xit"));
    connect( action_exit, &QAction::triggered, qApp, &QApplication::quit );

    menu->addAction(action_lighting);
    menu->addSeparator();
    menu->addAction( action_exit );

    m_trayIcon.setContextMenu(menu);

    updateTooltip();
    updateIcon();

    connect( &m_trayIcon, &QSystemTrayIcon::activated, this, &MainApplication::trayActivated );

    connect( &m_dbw, &DBUSWatcher::serviceActive, this, &MainApplication::serviceActive );
    connect( &m_dbw, &DBUSWatcher::serviceInactive, this, &MainApplication::serviceInactive );

    DBUSConnector *c = m_dbw.connector();
    if( c && c->isValid() )
        serviceActive();
}

void MainApplication::playClip(const QUrl &path)
{
#if QT_VERSION >= 0x060400
    m_player.setSource(path);
#else
    m_player.setMedia(path);
#endif
    m_player.play();
}

void MainApplication::playSoC()
{
    if( !m_connected || !m_online )
        playClip(QUrl("qrc:/audio/device not found.aac"));
    else if( m_charging )
        playClip(QUrl("qrc:/audio/charging.aac"));
    else if( m_chargeLevel > 97 )
        playClip(QUrl("qrc:/audio/battery fully charged.aac"));
    else if( m_chargeLevel > 93 )
        playClip(QUrl("qrc:/audio/battery 95%.aac"));
    else if( m_chargeLevel > 87 )
        playClip(QUrl("qrc:/audio/battery 90%.aac"));
    else if( m_chargeLevel > 83 )
        playClip(QUrl("qrc:/audio/battery 85%.aac"));
    else if( m_chargeLevel > 77 )
        playClip(QUrl("qrc:/audio/battery 80%.aac"));
    else if( m_chargeLevel > 73 )
        playClip(QUrl("qrc:/audio/battery 75%.aac"));
    else if( m_chargeLevel > 67 )
        playClip(QUrl("qrc:/audio/battery 70%.aac"));
    else if( m_chargeLevel > 63 )
        playClip(QUrl("qrc:/audio/battery 65%.aac"));
    else if( m_chargeLevel > 57 )
        playClip(QUrl("qrc:/audio/battery 60%.aac"));
    else if( m_chargeLevel > 53 )
        playClip(QUrl("qrc:/audio/battery 55%.aac"));
    else if( m_chargeLevel > 47 )
        playClip(QUrl("qrc:/audio/battery 50%.aac"));
    else if( m_chargeLevel > 43 )
        playClip(QUrl("qrc:/audio/battery 45%.aac"));
    else if( m_chargeLevel > 37 )
        playClip(QUrl("qrc:/audio/battery 40%.aac"));
    else if( m_chargeLevel > 33 )
        playClip(QUrl("qrc:/audio/battery 35%.aac"));
    else if( m_chargeLevel > 27 )
        playClip(QUrl("qrc:/audio/battery 30%.aac"));
    else if( m_chargeLevel > 23 )
        playClip(QUrl("qrc:/audio/battery 25%.aac"));
    else if( m_chargeLevel > 17 )
        playClip(QUrl("qrc:/audio/battery 20%.aac"));
    else if( m_chargeLevel > 13 )
        playClip(QUrl("qrc:/audio/battery 15%.aac"));
    else if( m_chargeLevel > 7 )
        playClip(QUrl("qrc:/audio/battery 10%.aac"));
    else if( m_chargeLevel > 3 )
        playClip(QUrl("qrc:/audio/battery 5%.aac"));
    else
        playClip(QUrl("qrc:/audio/battery level critical.aac"));
}

void MainApplication::serviceActive()
{
    qDebug() << "MainApplication making connections...";
    DBUSConnector *c = m_dbw.connector();
    connect( c, &DBUSConnector::voltageChanged, this, [this](int v){
        m_voltage = v;
    } );
    connect( c, &DBUSConnector::socChanged, this, [this](int v){
        m_chargeLevel = v;
        updateTooltip();
        updateIcon();
    } );
    connect( c, &DBUSConnector::chargingChanged, this, [this](bool ch){
        m_charging = ch;
        updateTooltip();
        updateIcon();
    });
    connect( c, &DBUSConnector::onlineChanged, this, [this, c](bool ol){
        m_online = ol;
        m_charging = c->getCharging();
        m_voltage = c->getVoltage();
        m_chargeLevel = c->getSoC();

        updateTooltip();
        updateIcon();
    });
    connect( c, &DBUSConnector::lightingChanged, this, [this](bool onoff) {
        m_lighting = onoff;

        updateTooltip();
        updateIcon();
    });
    connect( c, &DBUSConnector::buttonPressed, this, [this](int idx, bool pressed){
        if( 1 == idx && pressed )
            playSoC();
    });

    m_connected = true;
    m_online = c->getOnline();
    m_charging = c->getCharging();
    m_voltage = c->getVoltage();
    m_chargeLevel = c->getSoC();

    // Restore what we expect lighting to be:
    c->setLighting(m_lighting);

    updateTooltip();
    updateIcon();
}

void MainApplication::serviceInactive()
{
    m_connected = false;
    m_online = false;
    m_charging = false;
    m_voltage = 0;
    m_chargeLevel = 0;
    updateTooltip();
    updateIcon();
}

void MainApplication::updateTooltip()
{
    QString tt;
    if( !m_connected )
        tt = tr("Not connected to G733Daemon");
    else if( !m_online )
        tt = tr("Headset is off or out of range");
    else if( m_charging )
        tt = tr("Headset is charging and lighting is %1").arg( m_lighting ? tr("on") : tr("off"));
    else
        tt = tr("Headset is at %1% and lighting is %2").arg(m_chargeLevel).arg( m_lighting ? tr("on") : tr("off"));

    //if( tt != m_tooltip )
    m_trayIcon.setToolTip(tt);

    m_tooltip = tt;
}

void MainApplication::updateIcon()
{
    QVariantMap state;
    state["connected"] = m_connected;
    state["online"] = m_online;
    state["lighting"] = m_lighting;
    state["level"] = m_chargeLevel;
    state["charging"] = m_charging;

    if( state == m_state && !m_charging )
        return;

    m_state = state;

    QImage *source = nullptr;
    if( !m_online )
        source = &m_icon_offline;
    else if( m_lighting )
        source = &m_icon_online;
    else
        source = &m_icon_online_norgb;

    QImage wc = QImage( source->constBits(), source->width(), source->height(), source->format() );
    QFont smallFont;
    smallFont.setFamily("sans-serif");
    smallFont.setWeight(QFont::Bold);
    smallFont.setPixelSize(7);

    QString label = QString("%1%").arg(m_chargeLevel);
    if( !m_connected )
        label = tr("--");
    else if( !m_online )
        label = tr("off");
    else if( m_charging )
        label = tr("chg");

    QPainter p;
    p.begin(&wc);

    if( m_charging )
    {
        QRectF pos;
        pos.setLeft(0);
        pos.setTop(0);
        pos.setWidth(11);
        pos.setHeight(11);
        m_icon_batteryCharging[ m_chargingAnimationFrameNumber ]->render(&p, pos);
        m_chargingAnimationFrameNumber++;
        if( m_chargingAnimationFrameNumber >= m_icon_batteryCharging.length() )
            m_chargingAnimationFrameNumber = 0;
        m_chargingAnimationTimer.start();
    }
    else
    {
        m_chargingAnimationTimer.stop();

        QRectF pos;
        pos.setLeft(0);
        pos.setTop(0);
        pos.setWidth(11);
        pos.setHeight(11);

        for( int x=0; x <= 10; x++ )
        {
            if( x*10+5 > m_chargeLevel )
            {
                m_icon_batteryLevels[x]->render(&p, pos);
                break;
            }
        }

        // black shadow:
        p.setPen(Qt::black);
        p.setFont(smallFont);
        p.drawText(-1, 5, wc.width(), wc.height()-5, Qt::AlignHCenter | Qt::AlignVCenter, label);

        // charge perc:
        p.setPen(Qt::white);
        p.setFont(smallFont);
        p.drawText(0, 6, wc.width(), wc.height()-6, Qt::AlignHCenter | Qt::AlignVCenter, label);
    }

    p.end();

    QIcon ico = QIcon(QPixmap::fromImage(wc));
    if( ico.isNull() )
        return;

    m_trayIcon.setIcon(ico);
    m_trayIcon.setVisible(true);
}

void MainApplication::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason)
    //qDebug() << "Activated: " << reason;
}
