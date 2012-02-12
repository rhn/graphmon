#include "ramusage.h"

RAMInfo::RAMInfo() : application(0), cached(0), buf(0), free(0),
    application_since_update(0), cached_since_update(0), buf_since_update(0), free_since_update(0)
{ }

void RAMInfo::reset()
{
    application_since_update = 0;
    cached_since_update = 0;
    buf_since_update = 0;
    free_since_update = 0;
}

RAMUsage::RAMUsage()
{
    m_signal_plotter = new SimplePlotter();
    this->m_signal_plotter->setBackgroundColor(Qt::darkGray);
    this->m_signal_plotter->setUseAutoMax(true);

    this->m_signal_plotter->addPlot(Qt::darkGreen);
    this->m_signal_plotter->addPlot(Qt::red);
    this->m_signal_plotter->addPlot(Qt::darkYellow);
    this->m_signal_plotter->addPlot(Qt::black);


    this->m_signal_plotter->setPreferredSize(100, 100);
    this->m_signal_plotter->setMinimumSize(10, 10);


    QSizePolicy sizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->m_signal_plotter->setSizePolicy(sizePolicy);
    // TODO: memory leak
}

void RAMUsage::addValue(QString name, double value) {
    if (name == "application") {
        this->m_ram_info.application = value;
        this->m_ram_info.application_since_update++;
    } else if (name == "cached") {
        this->m_ram_info.cached = value;
        this->m_ram_info.cached_since_update++;
    } else if (name == "buf") {
        this->m_ram_info.buf = value;
        this->m_ram_info.buf_since_update++;
    } else if (name == "free") {
        this->m_ram_info.free = value;
        this->m_ram_info.free_since_update++;
    }

    if (this->m_ram_info.application_since_update && this->m_ram_info.cached_since_update && this->m_ram_info.buf_since_update && this->m_ram_info.free_since_update) {
        this->update();
    }
}

void RAMUsage::update()
{
    this->m_signal_plotter->addSample(QList<double>() << this->m_ram_info.application << this->m_ram_info.buf << this->m_ram_info.cached << this->m_ram_info.free);
    this->m_ram_info.reset();
}
