#include "simpleplotter.h"
#include <KDebug>
#include <math.h>


double sum_sample(QList<double> sample) {
    double value = 0;
    Q_FOREACH(double subvalue, sample) {
        value += subvalue;
    }
    return value;
}

SimplePlotter::SimplePlotter() :
    m_use_auto_max(false),
    m_min_vertical(0),
    m_max_vertical(100),
    m_background_color(Qt::black)
{
}

SimplePlotter::~SimplePlotter()
{

}

/*void SimplePlotter::setGeometry(const QRectF &geometry)
{

}*/

void SimplePlotter::setBackgroundColor(const QColor &color)
{
    this->m_background_color = color;
}

void SimplePlotter::setPlotColors(const QList<QColor> &colors)
{
    this->m_plot_colors = colors;
}

void SimplePlotter::setUseAutoMax(bool value)
{
    this->m_use_auto_max = value;
}

void SimplePlotter::setVerticalRange(double min, double max)
{
    this->m_min_vertical = min;
    this->m_max_vertical = max;
}

void SimplePlotter::addPlot(const QColor &color) {
    this->m_plot_colors.append(color);
}

void SimplePlotter::addSample(const QList<double> &samples)
{
    this->m_samples.append(samples);
    if (this->m_samples.count() > size().width()) {
        this->m_samples.pop_front();
    }
    update();
}

void SimplePlotter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    uint width = (uint) size().width();
    uint height = (uint) size().height();
    drawPlots(painter, width, height);
}

void SimplePlotter::drawPlots(QPainter *painter, int width, int height) {
    painter->fillRect(0, 0, width, height, this->m_background_color);

    if (this->m_samples.count() == 0) {
        return;
    }

    double min_vertical = this->m_min_vertical;
    double max_vertical = this->m_max_vertical;
    if (this->m_use_auto_max) {
        QList<double> oldest_sample = this->m_samples[0];
        max_vertical = sum_sample(oldest_sample);

        Q_FOREACH(QList<double> sample, this->m_samples) {
            max_vertical = qMax(max_vertical, sum_sample(sample));
        }
    }

    int left = width - this->m_samples.count();

    Q_FOREACH(QList<double> sample, this->m_samples) {
        double bottom = height;

        int plot_index = 0;
        Q_FOREACH(double value, sample) {
            double vheight = (value - min_vertical) / (max_vertical - min_vertical) * height;
            painter->fillRect(left, bottom - vheight, 1, ceil(vheight), this->m_plot_colors[plot_index]);
            bottom -= vheight;
            Q_ASSERT(bottom >= -1); // float precision. should be 0
            plot_index++;
        }
        left++;
        Q_ASSERT(left <= width);
    }
}

#include "simpleplotter.moc"
