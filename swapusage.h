#ifndef SWAPUSAGE_H
#define SWAPUSAGE_H

#include "simpleplotter.h"

class SwapInfo
{
public:
    double used, free;
    int used_since_update, free_since_update;

    SwapInfo();
    void reset();
};

class SwapUsage
{
public:
    SwapUsage();
    void addValue(QString name, double value);

private:
    void update();
    SwapInfo m_swap_info;

public:
    SimplePlotter *m_signal_plotter;
};

#endif // SWAPUSAGE_H
