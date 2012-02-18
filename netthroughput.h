#ifndef NETTHROUGHPUT_H
#define NETTHROUGHPUT_H

#include "simpleplotter.h"
#include <KConfigGroup>
#include <QColor>

class NetInfo
{
public:
    double transmit, receive;
    int transmit_since_update, receive_since_update;

    NetInfo();
    void reset();
};

class NetColors
{
public:
    NetColors();
    NetColors(const KConfigGroup &config);
    void save(KConfigGroup &config);

    QColor background;
    QColor transmit;
    QColor receive;
};

class NetThroughput
{
public:
    enum Direction {
        Transmit,
        Receive
    };

    NetThroughput(const QString &if_name);
    void addValue(Direction dir, double value);
    void setColors(const NetColors &colors);
    QString m_if_name;

private:
    void update();
    NetInfo m_net_info;

public:
    SimplePlotter *m_signal_plotter;
};

#endif // NETTHROUGHPUT_H
