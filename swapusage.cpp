#include "swapusage.h"

SwapInfo::SwapInfo() : used(0), free(0),
    used_since_update(0), free_since_update(0)
{ }

void SwapInfo::reset()
{
    used_since_update = 0;
    free_since_update = 0;
}

SwapColors::SwapColors()
{
}

SwapColors::SwapColors(const KConfigGroup &config)
{
    background = QColor(config.readEntry("swap_bg_color", QColor(Qt::darkBlue).name()));
    used = QColor(config.readEntry("swap_used_color", QColor(Qt::red).name()));
    free = QColor(config.readEntry("swap_free_color", QColor(Qt::black).name()));
}

void SwapColors::save(KConfigGroup &config)
{
    config.writeEntry("swap_bg_color", background.name());
    config.writeEntry("swap_used_color", used.name());
    config.writeEntry("swap_free_color", free.name());
}

SwapUsage::SwapUsage()
{
    m_signal_plotter = new SimplePlotter();
    this->m_signal_plotter->setBackgroundColor(Qt::darkBlue);

    this->m_signal_plotter->setUseAutoMax(true);

    this->m_signal_plotter->addPlot(Qt::red);
    this->m_signal_plotter->addPlot(Qt::black);

    this->m_signal_plotter->setPreferredSize(100, 100);
    this->m_signal_plotter->setMinimumSize(10, 10);


    QSizePolicy sizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->m_signal_plotter->setSizePolicy(sizePolicy);
    // TODO: memory leak?
}

void SwapUsage::addValue(QString &name, double value) {
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

void SwapUsage::setColors(const SwapColors &colors)
{
    this->m_signal_plotter->setBackgroundColor(colors.background);
    this->m_signal_plotter->setPlotColors(QList<QColor>() << colors.used << colors.free);
}
