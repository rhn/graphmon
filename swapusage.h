#ifndef SWAPUSAGE_H
#define SWAPUSAGE_H

#include "simpleplotter.h"
#include <KConfigGroup>

class SwapInfo
{
public:
    double used, free;
    int used_since_update, free_since_update;

    SwapInfo();
    void reset();
};

class SwapColors
{
public:
    SwapColors();
    SwapColors(const KConfigGroup &config);
    void save(KConfigGroup &config);

    QColor background;
    QColor used;
    QColor free;
};

class SwapUsage
{
public:
    SwapUsage();
    void addValue(QString &name, double value);
    void setColors(const SwapColors &colors);

private:
    void update();
    SwapInfo m_swap_info;

public:
    SimplePlotter *m_signal_plotter;
};

#endif // SWAPUSAGE_H
