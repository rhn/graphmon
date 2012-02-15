#ifndef SIMPLEPLOTTER_H
#define SIMPLEPLOTTER_H

#include <QObject>
#include <QGraphicsWidget>
#include <QColor>
#include <QList>
#include <QPainter>

class SimplePlotter : public QGraphicsWidget
{
    Q_OBJECT
public:
    SimplePlotter();
    ~SimplePlotter();

    void addPlot(const QColor &color);
    void addSample(const QList<double> &samples);
    void setUseAutoMax(bool value);
    void setVerticalRange(double min, double max);

    void setBackgroundColor(const QColor &color);
    QColor getBackgroundColors();
    void setPlotColors(const QList<QColor> &colors);
    QColor getPlotColors();

    //virtual void setGeometry(const QRectF &geometry);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void drawWidget(QPainter *p, uint w, uint height);
    void drawPlots(QPainter *p, int w, int h);

private:
    bool m_use_auto_max;
    double m_min_vertical;
    double m_max_vertical;
    QList<QColor> m_plot_colors;
    QList<QList<double> > m_samples;
    QColor m_background_color;
};

#endif // SIMPLEPLOTTER_H
