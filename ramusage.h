#ifndef RAMUSAGE_H
#define RAMUSAGE_H

#include "simpleplotter.h"

class RAMInfo
{
public:
    double application, cached, buf, free;
    int application_since_update, cached_since_update, buf_since_update, free_since_update;

    RAMInfo();
    void reset();
};

class RAMUsage
{
public:
    RAMUsage();
    void addValue(QString name, double value);

private:
    void update();
    RAMInfo m_ram_info;

public:
    SimplePlotter *m_signal_plotter;
};

#endif // RAMUSAGE_H
