#ifndef MULTILOAD_H
#define MULTILOAD_H

#include <Plasma/Applet>
#include <Plasma/DataEngine>

#include <QGraphicsGridLayout>
#include "icongridlayout.h"
#include "cpuload.h"
#include "ramusage.h"

class Multiload : public Plasma::Applet
{
    Q_OBJECT
public:
    Multiload(QObject *parent, const QVariantList &args);
    ~Multiload();
    void init();

protected slots:
    void sourceAdded(const QString& source);
    void dataUpdated(const QString& source, const Plasma::DataEngine::Data &data);

private:
    void setCoreCount(const uint core_count);
    void insertCPU(void);
    void removeCPU(void);

    QVector<CPULoad*> m_cpu_load;
    RAMUsage *m_ram_usage;
    uint m_update_interval;

    IconGridLayout *m_layout;
};

K_EXPORT_PLASMA_APPLET(multiload, Multiload)

#endif // MULTILOAD_H
