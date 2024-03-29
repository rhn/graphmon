#include "simpleplotter.h"
#include <KDebug>
#include <math.h>
#include <Plasma/Theme>

double sum_sample(QList<double> sample) {
    double value = 0;
    Q_FOREACH(double subvalue, sample) {
        value += subvalue;
    }
    return value;
}

const QStringList SimplePlotter::suffixes(QList<QString>() << "" << "K" << "M" << "G" << "T");

SimplePlotter::SimplePlotter() :
    QGraphicsWidget(),
    m_use_auto_max(false),
    m_binary(false),
    m_min_vertical(0),
    m_max_vertical(100),
    m_background_color(Qt::black),
    m_inverted_plots_count(0),
    m_show_label(false),
    m_label(""),
    m_label_size(16)
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
    update();
}

void SimplePlotter::setPlotColors(const QList<QColor> &colors)
{
    this->m_plot_colors = colors;
    update();
}

void SimplePlotter::setUseAutoMax(bool value)
{
    this->m_use_auto_max = value;
    update();
}

void SimplePlotter::setBinary(bool value)
{
    this->m_binary = value;
    update();
}

void SimplePlotter::setLabel(const QString &label)
{
    this->m_label = label;
    this->updateLabel();
}

void SimplePlotter::setLabelSize(uint label_size)
{
    this->m_label_size = label_size;
    this->updateLabel();
}

void SimplePlotter::setShowLabel(bool value)
{
    this->m_show_label = value;
    this->updateLabel();
}

void SimplePlotter::updateLabel()
{
    if (this->m_show_label) {
        this->m_font_color = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
        this->m_font = Plasma::Theme::defaultTheme()->font(Plasma::Theme::DefaultFont);
        this->m_font.setPixelSize(m_label_size);
    }
    update();
}

void SimplePlotter::setVerticalRange(double min, double max)
{
    this->m_min_vertical = min;
    this->m_max_vertical = max;
    update();
}

void SimplePlotter::setInvertedPlotsCount(uint count) {
    this->m_inverted_plots_count = count;
    update();
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

    uint plot_height = height;

    if (this->m_show_label) {
        plot_height -= m_label_size;
        drawLabel(painter, QRect(0, plot_height, width, m_label_size));
    }

    drawPlots(painter, width, plot_height);
}

void SimplePlotter::drawLabel(QPainter *painter, const QRect &area) {
    painter->save();

    painter->setPen(this->m_font_color);
    painter->setFont(this->m_font);

    painter->drawText(area, Qt::AlignRight, this->m_label);

    if (this->m_samples.count() == 0) {
        return;
    }

    // Divide the number in order to find magnitude and new value.
    QString currentValueText;
    double currentValue = sum_sample(this->m_samples.back());

    if (this->m_binary) { // operate on binary numbers, divide by 1024
        int value = currentValue;
        uint magnitude = 0;
        while (value >= 1024) {
            value /= 1024;
            magnitude++;
            if (magnitude + 1 == suffixes.count()) {
                break;
            }
        }
        currentValueText = QString::number(value) + suffixes[magnitude];
    } else {
        double value = currentValue;
        uint magnitude = 0;
        while (value >= 1000) {
            value /= 1000;
            magnitude++;
            if (magnitude + 1 == suffixes.count()) {
                break;
            }
        }
        currentValueText = QString::number(value, 'g', 2) + suffixes[magnitude];
    }

    painter->drawText(area, Qt::AlignLeft, currentValueText);
    painter->restore();
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

        if (this->m_binary) {
            int vert_size = round(max_vertical - min_vertical);

            // make vert_size rounded to nearest bigger power of 2
            int bit_count = 0;
            while(vert_size) {
                bit_count++;
                vert_size >>= 1;
            }
            vert_size = (1 << bit_count);

            max_vertical = vert_size + min_vertical;
        }
    }

    if (max_vertical == 0) {
        max_vertical = 1;
    }

    int left = width - this->m_samples.count();

    Q_FOREACH(QList<double> sample, this->m_samples) {
        double bottom = height; // for normal graphs
        double top = 0; // for inverted graphs

        if (left >= 0) {
            int plot_index = 0;
            Q_FOREACH(double value, sample) {
                // FIXME: clip to pixels, not values
                double vheight = (value - min_vertical) / (max_vertical - min_vertical) * height;

                if (plot_index < this->m_inverted_plots_count) {
                    // draw inverted (stack from top)
                    vheight = qMin(vheight, height - top);
                    painter->fillRect(left, top, 1, ceil(vheight), this->m_plot_colors[plot_index]);

                    top += vheight;
                    Q_ASSERT(top < height + 1); // float error
                } else {
                    // draw normal (stack from bottom)
                    top = qMax(bottom - vheight, double(0));

                    painter->fillRect(left, top, 1, ceil(vheight), this->m_plot_colors[plot_index]);

                    bottom -= vheight;
                    Q_ASSERT(bottom >= -1); // float precision. should be 0
                }
                plot_index++;
            }
        }

        left++;
        Q_ASSERT(left <= width);
    }
}

#include "simpleplotter.moc"
