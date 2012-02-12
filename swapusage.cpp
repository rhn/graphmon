#include "swapusage.h"

SwapInfo::SwapInfo() : used(0), free(0),
    used_since_update(0), free_since_update(0)
{ }

void SwapInfo::reset()
{
    used_since_update = 0;
    free_since_update = 0;
}

SwapUsage::SwapUsage()
{
    m_signal_plotter = new SimplePlotter();
    this->m_signal_plotter->setBackgroundColor(Qt::darkGray);

    this->m_signal_plotter->setUseAutoMax(true);

    this->m_signal_plotter->addPlot(Qt::darkRed);
    this->m_signal_plotter->addPlot(Qt::black);

    this->m_signal_plotter->setPreferredSize(100, 100);
    this->m_signal_plotter->setMinimumSize(10, 10);


    QSizePolicy sizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->m_signal_plotter->setSizePolicy(sizePolicy);
    // TODO: memory leak
}

void SwapUsage::addValue(QString name, double value) {
    if (name == "used") {
        this->m_swap_info.used = value;
        this->m_swap_info.used_since_update++;
    } else if (name == "free") {
        this->m_swap_info.free = value;
        this->m_swap_info.free_since_update++;
    }

    if (this->m_swap_info.used_since_update && this->m_swap_info.free_since_update) {
        this->update();
    }
}

void SwapUsage::update()
{
    this->m_signal_plotter->addSample(QList<double>() << this->m_swap_info.used << this->m_swap_info.free);
    this->m_swap_info.reset();
}
