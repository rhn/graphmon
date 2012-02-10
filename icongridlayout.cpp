/***************************************************************************
 *   Copyright (C) 2010 - 2011 by Ingomar Wesp <ingomar@wesp.name>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/
#include "icongridlayout.h"

// Qt
#include <Qt>
#include <QtGlobal>
#include <QtCore/QList>
#include <QtCore/QSizeF>
#include <QtCore/QRectF>
#include <QtGui/QGraphicsLayout>
#include <QtGui/QGraphicsLayoutItem>
#include <QtGui/QSizePolicy>

// KDE
#include <KDebug>
#include <KIconLoader>

// stdlib
#include <math.h>

const int IconGridLayout::CELL_SPACING = 2;

IconGridLayout::IconGridLayout(QGraphicsLayoutItem *parent)
    : QGraphicsLayout(parent),
      m_items(),
      m_mode(ForceHeight),
      m_maxSectionSize(100),
      m_maxSectionCount(0),
      m_maxSectionCountForced(false),
      m_rowCount(0),
      m_columnCount(0),
      m_cellHeight(0),
      m_cellWidth(0)
{
    setContentsMargins(0, 0, 0, 0);

    QSizePolicy sizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    //sizePolicy.setHeightForWidth(true);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(1);
    setSizePolicy(sizePolicy);
}

IconGridLayout::~IconGridLayout()
{
    Q_FOREACH(QGraphicsLayoutItem *item, m_items) {
        if (item->ownedByLayout()) {
            delete item;
        }
    }
    m_items.clear();
}

IconGridLayout::Mode IconGridLayout::mode() const
{
    return m_mode;
}

void IconGridLayout::setMode(Mode mode)
{
    if (mode == m_mode) {
        return;
    }

    m_mode = mode;
    updateGridParameters();
    invalidate();
}

int IconGridLayout::maxSectionCount() const
{
    return m_maxSectionCount;
}

void IconGridLayout::setMaxSectionCount(int maxSectionCount)
{
    if (m_maxSectionCount == maxSectionCount) {
        return;
    }

    m_maxSectionCount = maxSectionCount;
    updateGridParameters();
    invalidate();
}

bool IconGridLayout::maxSectionCountForced() const
{
    return m_maxSectionCountForced;
}

void IconGridLayout::setMaxSectionCountForced(bool enable)
{
    if (m_maxSectionCountForced == enable) {
        return;
    }

    m_maxSectionCountForced = enable;
    updateGridParameters();
    invalidate();
}

void IconGridLayout::addItem(QGraphicsLayoutItem *item)
{
    m_items.append(item);
    addChildLayoutItem(item);
    item->setParentLayoutItem(this);
    updateGridParameters();
    invalidate();
}

void IconGridLayout::insertItem(int index, QGraphicsLayoutItem *item)
{
    m_items.insert(index, item);
    addChildLayoutItem(item);
    item->setParentLayoutItem(this);
    updateGridParameters();
    invalidate();
}

int IconGridLayout::rowCount() const
{
    return m_rowCount;
}

int IconGridLayout::columnCount() const
{
    return m_columnCount;
}

int IconGridLayout::count() const
{
    return m_items.size();
}

QGraphicsLayoutItem *IconGridLayout::itemAt(int index) const
{
    return m_items[index];
}

QGraphicsLayoutItem *IconGridLayout::itemAt(int row, int column) const
{
    return m_items[row * m_columnCount + column];
}

void IconGridLayout::removeAt(int index)
{
    QGraphicsLayoutItem *item = m_items.takeAt(index);
    item->setParentLayoutItem(0);

    if (item->ownedByLayout()) {
        delete item;
    }

    updateGridParameters();
    invalidate();
}

void IconGridLayout::removeItem(QGraphicsLayoutItem *item) {
    this->removeAt(this->m_items.indexOf(item));
}

void IconGridLayout::setGeometry(const QRectF &rect)
{
    QGraphicsLayout::setGeometry(rect);
    kDebug() << "setgeom" << rect;
    updateGridParameters();

    qreal offsetLeft = rect.left();
    qreal offsetTop = rect.top();

    QPointF pos(offsetLeft, offsetTop);
    kDebug() << "posin" << m_columnCount << m_rowCount << m_cellWidth << m_cellHeight << pos;
    QSizeF size(m_cellWidth, m_cellHeight);

    int itemCount = m_items.size();
    for (int i = 0; i < itemCount; i++) {
        int row = i / m_columnCount;
        int column = i % m_columnCount;

        if (column == 0) {
            if (row > 0) {
                pos.rx() = offsetLeft;
                pos.ry() += m_cellHeight + CELL_SPACING;
            }
        } else {
            pos.rx() += m_cellWidth + CELL_SPACING;
        }
    kDebug() << "pos " << pos;
        m_items[i]->setGeometry(QRectF(pos, size));
    }
}

QSizeF IconGridLayout::sizeHint(
    Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(constraint);

    switch(which) {
        case Qt::PreferredSize: return m_preferredSizeHint;
        case Qt::MinimumSize:
            if (m_mode == ForceHeight) {
                return QSizeF(0, m_maxSectionSize);
            }
            else {
                return QSizeF(KIconLoader::SizeSmall, m_preferredSizeHint.height());
            }

        default:
            return QSizeF();
    }
}

void IconGridLayout::computeGridParameters(QSizeF &preferredSize)
{
    const int itemCount = m_items.size();
    if (itemCount == 0) {
        preferredSize.setWidth(.0);
        preferredSize.setHeight(.0);
        return;
    }

    int minCellHeight = 0;
    int minCellWidth = 0;
    int preferredCellHeight = 0;
    int preferredCellWidth = 0;
    int usableCellHeight = 0;
    int usableCellWidth = 0;

    Q_FOREACH(QGraphicsLayoutItem *item, m_items) {
        minCellHeight = qMax(minCellHeight, (int)item->minimumHeight());
        minCellWidth = qMax(minCellWidth, (int)item->minimumWidth());
        preferredCellHeight = qMax(preferredCellHeight, (int)item->preferredHeight());
        preferredCellWidth = qMax(preferredCellWidth, (int)item->preferredWidth());
    }

    int rowCount;
    int columnCount;

    const int height = int(contentsRect().height());
    const int width = int(contentsRect().width());

    kDebug() << "reconf" << width << height;

    kDebug() << "start_cell" << preferredCellWidth << preferredCellHeight;
    if (m_mode == ForceHeight) {
        preferredCellHeight = m_maxSectionSize;
        usableCellHeight = qMax(qMin(m_maxSectionSize,
                                     height),
                                minCellHeight);

        // TODO: usable cell height
        int maximumRowCount = floor(double(height + CELL_SPACING) / (usableCellHeight + CELL_SPACING));
        columnCount = ceil(double(itemCount) / maximumRowCount);
        rowCount = ceil(double(itemCount) / columnCount);

        int availableCellWidth = (width + CELL_SPACING) / columnCount - CELL_SPACING;
        usableCellWidth = qMin(availableCellWidth, width);
    } else {
    }

    kDebug() << "resultcell" << preferredCellWidth << preferredCellHeight;
    int maxCellHeight =
        qMax(minCellHeight,
             (height - (rowCount - 1) * CELL_SPACING) / rowCount);

    Q_ASSERT(rowCount > 0 && columnCount > 0);

    preferredSize.setWidth(columnCount * (preferredCellWidth + CELL_SPACING) - CELL_SPACING);
    preferredSize.setHeight(rowCount * (preferredCellHeight + CELL_SPACING) - CELL_SPACING);

    kDebug() << "preferred" << preferredSize;
    m_cellHeight = usableCellHeight;
    m_cellWidth = usableCellWidth;

    m_columnCount = columnCount;
    m_rowCount = rowCount;
}

void IconGridLayout::updateGridParameters()
{
    QSizeF newPreferredSize;

    computeGridParameters(newPreferredSize);

    if (newPreferredSize != m_preferredSizeHint) {
        m_preferredSizeHint = newPreferredSize;
        updateGeometry();
    }
}
