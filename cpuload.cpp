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


CPULoad::CPULoad(void)
{
    m_signal_plotter = new Plasma::SignalPlotter();
    this->m_signal_plotter->setSvgBackground(QString());
    this->m_signal_plotter->setBackgroundColor(Qt::black);
    this->m_signal_plotter->setThinFrame(false);
    this->m_signal_plotter->setShowHorizontalLines(false);
    this->m_signal_plotter->setShowLabels(false);
    this->m_signal_plotter->setShowTopBar(false);
    this->m_signal_plotter->setUseAutoRange(false);
    this->m_signal_plotter->setVerticalRange(0, 100);
    this->m_signal_plotter->addPlot(Qt::white);
    this->m_signal_plotter->addPlot(Qt::yellow);
    this->m_signal_plotter->addPlot(Qt::green);
    this->m_signal_plotter->addPlot(Qt::red);

    this->m_signal_plotter->setPreferredSize(100, 100);
    this->m_signal_plotter->setMinimumSize(10, 10);


    QSizePolicy sizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->m_signal_plotter->setSizePolicy(sizePolicy);
    // TODO: memory leak
}

void CPULoad::addValue(QString name, double value) {
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
    this->m_signal_plotter->addSample(QList<double>() << this->m_cpu_info.wait << this->m_cpu_info.nice << this->m_cpu_info.user << this->m_cpu_info.sys);
    this->m_cpu_info.reset();
}
