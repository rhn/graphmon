#ifndef SIMPLEPLOTTER_H
#define SIMPLEPLOTTER_H

#include <QObject>
#include <QGraphicsWidget>
#include <QColor>
#include <QList>
#include <QPainter>
#include <QFont>

class SimplePlotter : public QGraphicsWidget
{
    Q_OBJECT
public:
    SimplePlotter();
    ~SimplePlotter();

    void addPlot(const QColor &color);
    void addSample(const QList<double> &samples);
    void setUseAutoMax(bool value);
    void setBinary(bool value);
    void setVerticalRange(double min, double max);
    void setInvertedPlotsCount(uint count);

    void setBackgroundColor(const QColor &color);
    QColor getBackgroundColors();
    void setPlotColors(const QList<QColor> &colors);
    QColor getPlotColors();

    void setLabel(const QString &label);
    void setShowLabel(bool value);
    void setLabelSize(uint label_size);

    //virtual void setGeometry(const QRectF &geometry);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void drawPlots(QPainter *painter, int width, int height);
    void drawLabel(QPainter *painter, const QRect &area);

private:
    void updateLabel();

    QString m_label;
    bool m_show_label;
    bool m_use_auto_max;
    bool m_binary;
    double m_min_vertical;
    double m_max_vertical;

    QList<QList<double> > m_samples;
    QList<QColor> m_plot_colors;
    QColor m_background_color;
    QColor m_font_color;

    QFont m_font;
    uint m_label_size;

    uint m_inverted_plots_count;

    static const QStringList suffixes;
};

#endif // SIMPLEPLOTTER_H
