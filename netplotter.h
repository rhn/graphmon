#ifndef NETPLOTTER_H
#define NETPLOTTER_H

#include <QGraphicsWidget>

class NetPlotter : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit NetPlotter(QObject *parent = 0);

    void addTransmitPlot(const QColor &color);
    void addReceivePlot(const QColor &color);
    void addSamples(const QList<double> &txsamples, const QList<double> &rxsamples);
    void setUseAutoMax(bool value);
    void setMaxValue(double max);

    void setBackgroundColor(const QColor &color);
    QColor getBackgroundColors();
    void setPlotColors(const QList<QColor> &txcolors, const QList<QColor> &rxcolors);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void drawWidget(QPainter *p, uint w, uint height);
    void drawPlots(QPainter *p, int w, int h);

private:
    bool m_use_auto_max;
    double m_max_vertical;
    QList<QColor> m_tx_plot_colors;
    QList<QColor> m_rx_plot_colors;
    QList<QList<double> > m_tx_samples;
    QList<QList<double> > m_rx_samples;
    QColor m_background_color;
};

#endif // NETPLOTTER_H
