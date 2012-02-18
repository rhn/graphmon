#include "multiload.h"

#include <QtGui/QPainter>
#include <KConfigDialog>

Multiload::Multiload(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
    , m_update_interval(500)
    , m_layout(new IconGridLayout())
{
    setHasConfigurationInterface(true);

    setBackgroundHints(DefaultBackground);
    setMinimumSize(20, 20);
    setLayout(this->m_layout);
    m_ram_usage = new RAMUsage();
    this->m_layout->addItem(this->m_ram_usage->m_signal_plotter);
    m_swap_usage = new SwapUsage();
    this->m_layout->addItem(this->m_swap_usage->m_signal_plotter);
    kDebug() << "created";
}

Multiload::~Multiload() {
    for(int i = 0; i < this->m_layout->count(); i++) {
        this->m_layout->removeAt(0);
    }
    // layout already taken care of
    Q_FOREACH(CPULoad *cpuload, this->m_cpu_load) {
        delete cpuload;
    }
    this->m_cpu_load.clear();

    Q_FOREACH(NetThroughput *netthroughput, this->m_net_throughputs.values()) {
        delete netthroughput;
    }
    this->m_net_throughputs.clear();

    delete this->m_ram_usage;
    delete this->m_swap_usage;
}

void Multiload::init()
{
    KConfigGroup cfg = config();

    CPUColors new_cpu_colors(cfg);
    m_cpu_colors = new_cpu_colors;

    RAMColors new_ram_colors(cfg);
    m_ram_colors = new_ram_colors;

    SwapColors new_swap_colors(cfg);
    m_swap_colors = new_swap_colors;

    NetColors new_net_colors(cfg);
    m_net_colors = new_net_colors;

    this->m_ram_usage->setColors(m_ram_colors);
    this->m_swap_usage->setColors(m_swap_colors);

    m_update_interval = cfg.readEntry("update_interval", 500);
    this->m_layout->setMaxSectionSize(cfg.readEntry("layout_section_size", 30));
    this->m_layout->setMaxSectionCount(cfg.readEntry("layout_section_count", 0));

    IconGridLayout::Mode mode = IconGridLayout::modeFromString(cfg.readEntry("layout_mode", "ForceHeight"));
    this->m_layout->setMode(mode);

    connect(dataEngine("systemmonitor"), SIGNAL(sourceAdded(QString)), this, SLOT(sourceAdded(QString)));
    connect(dataEngine("systemmonitor"), SIGNAL(sourceRemoved(QString)), this, SLOT(sourceRemoved(QString)));
    kDebug() << "inited";

    // XXX: race condition hell: if any sources change now, the changes are lost
    Q_FOREACH(QString source, dataEngine("systemmonitor")->sources()) {
        sourceAdded(source);
    }
}

void Multiload::sourceAdded(const QString& source)
{
    // TODO: more sensible sources
    if (source == "cpu/cores") {
        dataEngine("systemmonitor")->connectSource(source, this, this->m_update_interval);
    } else if (source.startsWith("cpu/cpu")) {
        dataEngine("systemmonitor")->connectSource(source, this, this->m_update_interval);
    } else if (source.startsWith("mem/physical")) {
        dataEngine("systemmonitor")->connectSource(source, this, this->m_update_interval);
    } else if (source.startsWith("mem/swap")) {
        dataEngine("systemmonitor")->connectSource(source, this, this->m_update_interval);
    } else if (source.startsWith("network/interfaces/") && source.endsWith("/data")) {
        this->insertNetSource(source);
    }
}

void Multiload::sourceRemoved(const QString &source)
{
    if (source == "cpu/cores") {
        dataEngine("systemmonitor")->disconnectSource(source, this);
    } else if (source.startsWith("cpu/cpu")) {
        dataEngine("systemmonitor")->disconnectSource(source, this);
    } else if (source.startsWith("mem/physical")) {
        dataEngine("systemmonitor")->disconnectSource(source, this);
    } else if (source.startsWith("mem/swap")) {
        dataEngine("systemmonitor")->disconnectSource(source, this);
    } else if (source.startsWith("network/interfaces/") && source.endsWith("/data")) {
        this->removeNetSource(source);
    }
}

void Multiload::insertNetSource(const QString &source) {
    QStringList fragments = source.split('/');
    QString if_name = fragments[2];

    if (if_name == "lo") {
        return;
    }

    if (!this->m_net_throughputs.contains(if_name)) {
        NetThroughput *new_net = new NetThroughput(if_name);
        new_net->setColors(this->m_net_colors);

        this->m_net_throughputs.insert(if_name, new_net);

        int index = this->m_cpu_load.count() + 1 + 1 + this->m_net_throughputs.count();

        this->m_layout->insertItem(index, new_net->m_signal_plotter);
    } else {
        // I don't care. Transfer/receive should always come in pairs
    }

    dataEngine("systemmonitor")->connectSource(source, this, this->m_update_interval);
}

void Multiload::removeNetSource(const QString &source)
{
    QStringList fragments = source.split('/');
    QString if_name = fragments[2];
    kDebug() << "removed net" << source;
    if (this->m_net_throughputs.contains(if_name)) {
        NetThroughput *net = this->m_net_throughputs[if_name];
        this->m_net_throughputs.remove(if_name);

        this->m_layout->removeItem(net->m_signal_plotter);
        delete net;
    } else {
        kDebug() << "ignored";
        // I don't care. Transfer/receive should always come in pairs
    }

    dataEngine("systemmonitor")->disconnectSource(source, this);
}

void Multiload::setInterval(uint milliseconds) {
    this->m_update_interval = milliseconds;

    Q_FOREACH(QString source, dataEngine("systemmonitor")->sources()) {
        // disconnect
        if (source == "cpu/cores") {
            dataEngine("systemmonitor")->disconnectSource(source, this);
        } else if (source.startsWith("cpu/cpu")) {
            dataEngine("systemmonitor")->disconnectSource(source, this);
        } else if (source.startsWith("mem/physical")) {
            dataEngine("systemmonitor")->disconnectSource(source, this);
        } else if (source.startsWith("mem/swap")) {
            dataEngine("systemmonitor")->disconnectSource(source, this);
        }
        // reconnect
        this->sourceAdded(source);
    }

    Q_FOREACH(NetThroughput *throughput, this->m_net_throughputs.values()) {
        QString rxSourceName = QString("network/interfaces/") + throughput->m_if_name + QString("/receiver/data");
        QString txSourceName = QString("network/interfaces/") + throughput->m_if_name + QString("/transmitter/data");

        dataEngine("systemmonitor")->disconnectSource(rxSourceName, this);
        dataEngine("systemmonitor")->disconnectSource(txSourceName, this);

        this->sourceAdded(rxSourceName);
        this->sourceAdded(txSourceName);
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
    } else if (source.startsWith("mem/swap")) {
        QStringList fragments = source.split('/');
        QString value_name = fragments.back();
        m_swap_usage->addValue(value_name, data["value"].toDouble());
    } else if (source.startsWith("network/interfaces") && source.endsWith("/data")) {
        QStringList fragments = source.split('/');
        QString if_name = fragments[2];
        QString direction = fragments[3];

        NetThroughput::Direction dir;
        if (direction == "transmitter") {
            dir = NetThroughput::Transmit;
        } else {
            dir = NetThroughput::Receive;
        }
        m_net_throughputs[if_name]->addValue(dir, data["value"].toDouble());
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
    new_cpu->setColors(this->m_cpu_colors);
    this->m_cpu_load.append(new_cpu);
    this->m_layout->insertItem(this->m_cpu_load.count() - 1, new_cpu->m_signal_plotter);
}

void Multiload::removeCPU() {
    CPULoad *removed = this->m_cpu_load.back();
    this->m_cpu_load.pop_back();
    this->m_layout->removeItem(removed->m_signal_plotter);
    delete removed;
}

void Multiload::createConfigurationInterface(KConfigDialog *parent) {
    // COLORS

    QWidget *colorPage = new QWidget();
    this->m_color_config_ui.setupUi(colorPage);

    // CPU colors
    this->m_color_config_ui.cpu_bg_color->setColor(this->m_cpu_colors.background);
    this->m_color_config_ui.cpu_sys_color->setColor(this->m_cpu_colors.sys);
    this->m_color_config_ui.cpu_user_color->setColor(this->m_cpu_colors.user);
    this->m_color_config_ui.cpu_nice_color->setColor(this->m_cpu_colors.nice);
    this->m_color_config_ui.cpu_wait_color->setColor(this->m_cpu_colors.wait);

    connect(this->m_color_config_ui.cpu_bg_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.cpu_sys_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.cpu_user_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.cpu_nice_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.cpu_wait_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));

    // RAM colors
    this->m_color_config_ui.ram_bg_color->setColor(this->m_ram_colors.background);
    this->m_color_config_ui.ram_application_color->setColor(this->m_ram_colors.application);
    this->m_color_config_ui.ram_buffers_color->setColor(this->m_ram_colors.buf);
    this->m_color_config_ui.ram_cached_color->setColor(this->m_ram_colors.cached);
    this->m_color_config_ui.ram_free_color->setColor(this->m_ram_colors.free);

    connect(this->m_color_config_ui.ram_bg_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.ram_application_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.ram_buffers_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.ram_cached_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.ram_free_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));

    // swap colors
    this->m_color_config_ui.swap_bg_color->setColor(this->m_swap_colors.background);
    this->m_color_config_ui.swap_used_color->setColor(this->m_swap_colors.used);
    this->m_color_config_ui.swap_free_color->setColor(this->m_swap_colors.free);

    connect(this->m_color_config_ui.swap_bg_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.swap_used_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.swap_free_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));

    // net colors
    this->m_color_config_ui.net_bg_color->setColor(this->m_net_colors.background);
    this->m_color_config_ui.net_receive_color->setColor(this->m_net_colors.receive);
    this->m_color_config_ui.net_transmit_color->setColor(this->m_net_colors.transmit);

    connect(this->m_color_config_ui.net_bg_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.net_receive_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));
    connect(this->m_color_config_ui.net_transmit_color, SIGNAL(changed(QColor)), parent, SLOT(settingsModified()));

    // GENERAL

    QWidget *generalPage = new QWidget();
    this->m_general_config_ui.setupUi(generalPage);

    this->m_general_config_ui.update_interval->setValue(this->m_update_interval);
    this->m_general_config_ui.layout_section_size->setValue(this->m_layout->maxSectionSize());
    this->m_general_config_ui.layout_section_count->setValue(this->m_layout->maxSectionCount());

    if (this->m_layout->mode() == IconGridLayout::ForceHeight) {
        this->m_general_config_ui.layout_mode->setCurrentIndex(0);
    } else {
        this->m_general_config_ui.layout_mode->setCurrentIndex(1);
    }

    connect(this->m_general_config_ui.update_interval, SIGNAL(valueChanged(int)), parent, SLOT(settingsModified()));
    connect(this->m_general_config_ui.layout_section_size, SIGNAL(valueChanged(int)), parent, SLOT(settingsModified()));
    connect(this->m_general_config_ui.layout_section_count, SIGNAL(valueChanged(int)), parent, SLOT(settingsModified()));
    connect(this->m_general_config_ui.layout_mode, SIGNAL(currentIndexChanged(int)), parent, SLOT(settingsModified()));


    parent->addPage(generalPage, i18n("General"), icon());
    parent->addPage(colorPage, i18n("Colors"), icon());
    connect(parent, SIGNAL(accepted()), this, SLOT(configUpdated()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configUpdated()));
}

void Multiload::configUpdated() {
    KConfigGroup cfg = config();

    // COLORS
    // CPU
    m_cpu_colors.background = m_color_config_ui.cpu_bg_color->color();
    m_cpu_colors.sys = m_color_config_ui.cpu_sys_color->color();
    m_cpu_colors.user = m_color_config_ui.cpu_user_color->color();
    m_cpu_colors.nice = m_color_config_ui.cpu_nice_color->color();
    m_cpu_colors.wait = m_color_config_ui.cpu_wait_color->color();

    Q_FOREACH(CPULoad *cpu, m_cpu_load) {
        cpu->setColors(m_cpu_colors);
    }
    m_cpu_colors.save(cfg);

    // RAM
    m_ram_colors.background = m_color_config_ui.ram_bg_color->color();
    m_ram_colors.application = m_color_config_ui.ram_application_color->color();
    m_ram_colors.buf = m_color_config_ui.ram_buffers_color->color();
    m_ram_colors.cached = m_color_config_ui.ram_cached_color->color();
    m_ram_colors.free = m_color_config_ui.ram_free_color->color();

    m_ram_usage->setColors(m_ram_colors);

    m_ram_colors.save(cfg);

    // SWAP
    m_swap_colors.background = m_color_config_ui.swap_bg_color->color();
    m_swap_colors.used = m_color_config_ui.swap_used_color->color();
    m_swap_colors.free = m_color_config_ui.swap_free_color->color();

    m_swap_usage->setColors(m_swap_colors);

    m_swap_colors.save(cfg);

    // NET
    m_net_colors.background = m_color_config_ui.net_bg_color->color();
    m_net_colors.receive = m_color_config_ui.net_receive_color->color();
    m_net_colors.transmit = m_color_config_ui.net_transmit_color->color();

    Q_FOREACH(NetThroughput *net, m_net_throughputs.values()) {
        net->setColors(m_net_colors);
    }

    m_net_colors.save(cfg);

    // GENERAL

    if (this->m_update_interval != m_general_config_ui.update_interval->value()) {
        this->setInterval(m_general_config_ui.update_interval->value());
        cfg.writeEntry("update_interval", this->m_update_interval);
    }

    uint section_size = m_general_config_ui.layout_section_size->value();
    this->m_layout->setMaxSectionSize(section_size);
    cfg.writeEntry("layout_section_size", section_size);

    uint section_count = m_general_config_ui.layout_section_count->value();
    this->m_layout->setMaxSectionCount(section_count);
    cfg.writeEntry("layout_section_count", section_count);

    uint mode_index = m_general_config_ui.layout_mode->currentIndex();
    IconGridLayout::Mode mode;
    if (mode_index == 0) {
        mode = IconGridLayout::ForceHeight;
    } else {
        mode = IconGridLayout::ForceWidth;
    }
    this->m_layout->setMode(mode);
    cfg.writeEntry("layout_mode", IconGridLayout::modeToString(mode));

    emit configNeedsSaving();
}

#include "multiload.moc"
