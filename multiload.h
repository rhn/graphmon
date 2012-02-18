#ifndef MULTILOAD_H
#define MULTILOAD_H

#include <Plasma/Applet>
#include <Plasma/DataEngine>

#include <QList>
#include <QVector>
#include <QHash>

#include "rigidgridlayout.h"
#include "cpuload.h"
#include "ramusage.h"
#include "swapusage.h"
#include "netthroughput.h"

#include "ui_color-config.h"
#include "ui_general-config.h"

namespace Ui {
    class ColorConfig;
    class GeneralConfig;
}

class Multiload : public Plasma::Applet
{
    Q_OBJECT
public:
    Multiload(QObject *parent, const QVariantList &args);
    ~Multiload();
    void init();
    void createConfigurationInterface(KConfigDialog *parent);

protected slots:
    void configUpdated();
    void sourceAdded(const QString& source);
    void sourceRemoved(const QString& source);
    void dataUpdated(const QString& source, const Plasma::DataEngine::Data &data);

private:
    void setCoreCount(const uint core_count);
    void insertNetSource(const QString& source);
    void removeNetSource(const QString& source);

    void insertCPU(void);
    void removeCPU(void);

    void setInterval(uint milliseconds);
    void setSectionSize(uint size);

    QVector<CPULoad*> m_cpu_load;
    RAMUsage *m_ram_usage;
    SwapUsage *m_swap_usage;
    QHash<QString, NetThroughput*> m_net_throughputs;

    CPUColors m_cpu_colors;
    RAMColors m_ram_colors;
    SwapColors m_swap_colors;
    NetColors m_net_colors;

    uint m_label_size;

    uint m_update_interval;

    RigidGridLayout *m_layout;

    Ui::ColorConfig m_color_config_ui;
    Ui::GeneralConfig m_general_config_ui;
};

K_EXPORT_PLASMA_APPLET(multiload, Multiload)

#endif // MULTILOAD_H
