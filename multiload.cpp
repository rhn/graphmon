#include "multiload.h"
#include "cpuload.h"
#include "ramusage.h"

#include <QtGui/QPainter>
#include <Plasma/SignalPlotter>

Multiload::Multiload(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
    , m_update_interval(500)
    , m_layout(new IconGridLayout())
{
    setBackgroundHints(DefaultBackground);
    setMinimumSize(20, 20);
    setLayout(this->m_layout);
    m_ram_usage = new RAMUsage();
    this->m_layout->addItem(this->m_ram_usage->m_signal_plotter);
}

Multiload::~Multiload() {
    for(int i = 0; i < this->m_layout->count(); i++) {
        this->m_layout->removeAt(0);
    }

    Q_FOREACH(CPULoad *cpuload, this->m_cpu_load) {
        delete cpuload;
    }
    this->m_cpu_load.clear();

    delete this->m_ram_usage;
}

void Multiload::init()
{
    connect(dataEngine("systemmonitor"), SIGNAL(sourceAdded(QString)), this, SLOT(sourceAdded(QString)));
}

void Multiload::sourceAdded(const QString& source)
{
    // TODO: more sensible sources
    kDebug() << source;
    if (source == "cpu/cores") {
        dataEngine("systemmonitor")->connectSource(source, this, this->m_update_interval);
    } else if (source.startsWith("cpu/cpu")) {
        dataEngine("systemmonitor")->connectSource(source, this, this->m_update_interval);
    } else if (source.startsWith("mem/physical")) {
        dataEngine("systemmonitor")->connectSource(source, this, this->m_update_interval);
    }
}

void Multiload::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data) {
    if (source == "cpu/cores") {
        this->setCoreCount(data["value"].toInt());
    } else if (source.startsWith("cpu/cpu")) {
        QStringList fragments = source.split('/');
        int cpu_number = fragments[1].mid(3).toInt();
        QString value_name = fragments.back();

        if (cpu_number >= this->m_cpu_load.count()) {
            this->setCoreCount(cpu_number + 1);
        }

        m_cpu_load[cpu_number]->addValue(value_name, data["value"].toDouble());
    } else if (source.startsWith("mem/physical")) {
        QStringList fragments = source.split('/');
        QString value_name = fragments.back();
        m_ram_usage->addValue(value_name, data["value"].toDouble());
    }
}

void Multiload::setCoreCount(const uint core_count) {
    int current_entries = this->m_cpu_load.count();
    int difference = core_count - current_entries;
    if (difference != 0) {
        if (difference > 0) {
            for (int i = 0; i < difference; i++) {
                this->insertCPU();
            }
        } else {
            for (int i = 0; i < -difference; i++) {
                this->removeCPU();
            }
        }
    }
}

void Multiload::insertCPU() {
    CPULoad *new_cpu = new CPULoad();
    this->m_cpu_load.append(new_cpu);
    this->m_layout->insertItem(this->m_cpu_load.count() - 1, new_cpu->m_signal_plotter);
}

void Multiload::removeCPU() {
    CPULoad *removed = this->m_cpu_load.back();
    this->m_cpu_load.pop_back();
    this->m_layout->removeItem(removed->m_signal_plotter);
    delete removed;
}

#include "multiload.moc"
