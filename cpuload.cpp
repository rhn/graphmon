#include "cpuload.h"
#include <KDebug>

CpuInfo::CpuInfo(): user(0), sys(0), nice(0), wait(0),
        user_since_update(0), sys_since_update(0), nice_since_update(0), wait_since_update(0)
{
}

void CpuInfo::reset() {
    user_since_update = 0;
    sys_since_update = 0;
    nice_since_update = 0;
    wait_since_update = 0;
}

CPUColors::CPUColors() {}

CPUColors::CPUColors(const KConfigGroup &config) {
    background = QColor(config.readEntry("cpu_bg_color", QColor(Qt::black).name()));
    sys = QColor(config.readEntry("cpu_sys_color", QColor(Qt::red).name()));
    user = QColor(config.readEntry("cpu_user_color", QColor(Qt::green).name()));
    nice = QColor(config.readEntry("cpu_nice_color", QColor(Qt::blue).name()));
    wait = QColor(config.readEntry("cpu_wait_color", QColor(Qt::white).name()));
}

void CPUColors::save(KConfigGroup &config) {
    config.writeEntry("cpu_bg_color", background.name());
    config.writeEntry("cpu_sys_color", sys.name());
    config.writeEntry("cpu_user_color", user.name());
    config.writeEntry("cpu_nice_color", nice.name());
    config.writeEntry("cpu_wait_color", wait.name());
}

CPULoad::CPULoad(void)
{
    m_signal_plotter = new SimplePlotter();
    this->m_signal_plotter->setBackgroundColor(Qt::black);
    this->m_signal_plotter->setUseAutoMax(false);
    this->m_signal_plotter->setVerticalRange(0, 100);

    this->m_signal_plotter->addPlot(Qt::red);
    this->m_signal_plotter->addPlot(Qt::green);
    this->m_signal_plotter->addPlot(Qt::yellow);
    this->m_signal_plotter->addPlot(Qt::white);

    this->m_signal_plotter->setPreferredSize(100, 100);
    this->m_signal_plotter->setMinimumSize(10, 10);


    QSizePolicy sizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->m_signal_plotter->setSizePolicy(sizePolicy);
    // TODO: memory leak
}

void CPULoad::addValue(QString &name, double value) {
    if (name == "sys") {
        this->m_cpu_info.sys = value;
        this->m_cpu_info.sys_since_update++;
    } else if (name == "user") {
        this->m_cpu_info.user = value;
        this->m_cpu_info.user_since_update++;
    } else if (name == "nice") {
        this->m_cpu_info.nice = value;
        this->m_cpu_info.nice_since_update++;
    } else if (name == "wait") {
        this->m_cpu_info.wait = value;
        this->m_cpu_info.wait_since_update++;
    }

    if (this->m_cpu_info.sys_since_update && this->m_cpu_info.user_since_update && this->m_cpu_info.nice_since_update && this->m_cpu_info.wait_since_update) {
        this->update();
    }
}

void CPULoad::update()
{
    this->m_signal_plotter->addSample(QList<double>() << this->m_cpu_info.sys << this->m_cpu_info.user << this->m_cpu_info.nice << this->m_cpu_info.wait);
    this->m_cpu_info.reset();
}

void CPULoad::setColors(const CPUColors &colors) {
    this->m_signal_plotter->setBackgroundColor(colors.background);
    this->m_signal_plotter->setPlotColors(QList<QColor>() << colors.sys << colors.user << colors.nice << colors.wait);
}
