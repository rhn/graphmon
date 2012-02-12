#ifndef CPULOAD_H
#define CPULOAD_H

#include "simpleplotter.h"

class CpuInfo
{
public:
    double user, sys, nice, wait;
    int user_since_update, sys_since_update, nice_since_update, wait_since_update;

    CpuInfo();
    void reset();
};


class CPULoad
{
public:
    CPULoad(void);
    void addValue(QString name, double value);

private:
    void update();
    CpuInfo m_cpu_info;

public:
    //Plasma::SignalPlotter *m_signal_plotter;
    SimplePlotter *m_signal_plotter;
};

#endif // CPULOAD_H
