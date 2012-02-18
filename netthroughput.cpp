#include "netthroughput.h"

NetInfo::NetInfo() :
    transmit(0), receive(0),
    transmit_since_update(0), receive_since_update(0)
{ }

void NetInfo::reset() {
    transmit_since_update = 0;
    receive_since_update = 0;
}

NetColors::NetColors() {}

NetColors::NetColors(const KConfigGroup &config) {
    background = QColor(config.readEntry("net_bg_color", QColor(Qt::black).name()));
    transmit = QColor(config.readEntry("net_tx_color", QColor(Qt::red).name()));
    receive = QColor(config.readEntry("net_rx_color", QColor(Qt::darkGreen).name()));
}

void NetColors::save(KConfigGroup &config) {
    config.writeEntry("net_bg_color", background.name());
    config.writeEntry("net_tx_color", transmit.name());
    config.writeEntry("net_rx_color", receive.name());
}

NetThroughput::NetThroughput(const QString &if_name)
{
    this->m_if_name = if_name;
    m_signal_plotter = new SimplePlotter();
    this->m_signal_plotter->setBackgroundColor(Qt::black);

    this->m_signal_plotter->setUseAutoMax(true);
    this->m_signal_plotter->setBinary(true);

    this->m_signal_plotter->addPlot(Qt::red);
    this->m_signal_plotter->addPlot(Qt::green);
    this->m_signal_plotter->setInvertedPlotsCount(1);

    this->m_signal_plotter->setPreferredSize(100, 100);
    this->m_signal_plotter->setMinimumSize(10, 10);


    QSizePolicy sizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->m_signal_plotter->setSizePolicy(sizePolicy);
    // TODO: memory leak?
}

void NetThroughput::addValue(Direction dir, double value) {
    if (dir == Transmit) {
        this->m_net_info.transmit = value;
        this->m_net_info.transmit_since_update++;
    } else if (dir == Receive) {
        this->m_net_info.receive = value;
        this->m_net_info.receive_since_update++;
    }

    if (this->m_net_info.receive_since_update && this->m_net_info.transmit_since_update) {
        this->update();
    }
}

void NetThroughput::update() {
    this->m_signal_plotter->addSample(QList<double>() << this->m_net_info.transmit << this->m_net_info.receive);
    this->m_net_info.reset();
}

void NetThroughput::setColors(const NetColors &colors) {
    this->m_signal_plotter->setBackgroundColor(colors.background);
    this->m_signal_plotter->setPlotColors(QList<QColor>() << colors.transmit << colors.receive);
}
