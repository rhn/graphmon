/***************************************************************************
 *   Copyright (C) 2010 - 2011 by Ingomar Wesp <ingomar@wesp.name>         *
 *   Copyright (C) 2012 by rhn <kdde.rhn@porcupinefactory.org>             *
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
#ifndef ICONGRIDLAYOUT_H
#define ICONGRIDLAYOUT_H

// Qt
#include <Qt>
#include <QtGlobal>
#include <QtCore/QList>
#include <QtCore/QSizeF>
#include <QtGui/QGraphicsLayout>

class QEvent;
class QRectF;
class QGraphicsLayoutItem;

class IconGridLayout : public QGraphicsLayout {

public:

    enum Mode {
        ForceHeight,
        ForceWidth
    };

    static Mode modeFromString(const QString s);
    static QString modeToString(Mode);

    IconGridLayout(QGraphicsLayoutItem *parent = 0);
    ~IconGridLayout();

    Mode mode() const;
    void setMode(Mode mode);

    int maxSectionCount() const;
    uint maxSectionSize() const;

    /**
     * Depending on the mode, @c setMaxSectionCount limits either the
     * number of rows or the number of columns that are displayed. In
     * @c ForceHeight mode, @a maxSectionCount limits the maximum
     * number of columns while in @c ForceWidth mode, it applies to
     * the maximum number of rows.
     *
     * Setting @a maxSectionCount to @c 0 disables the limitation.
     *
     * @param maxSectionCount the maximum number of rows or columns
     *    (depending on the mode) that should be displayed.
     */
    void setMaxSectionCount(int maxSectionCount);
    void setMaxSectionSize(uint maxSectionSize);

    void addItem(QGraphicsLayoutItem *item);
    void removeItem(QGraphicsLayoutItem *item);
    void insertItem(int index, QGraphicsLayoutItem *item);

    int count() const;
    int columnCount() const;
    int rowCount() const;
    QGraphicsLayoutItem *itemAt(int index) const;
    QGraphicsLayoutItem *itemAt(int row, int column) const;

    void removeAt(int index);

    void setGeometry(const QRectF &rect);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

    static const int CELL_SPACING;

private:
    void computeGridParameters(QSizeF &preferredSize);

    void updateGridParameters();

    QList<QGraphicsLayoutItem*> m_items;
    Mode m_mode;
    int m_maxSectionSize;
    int m_maxSectionCount;

    int m_rowCount;
    int m_columnCount;
    int m_cellHeight;
    int m_cellWidth;
    QSizeF m_preferredSizeHint;
};

#endif /* ICONGRIDLAYOUT_H */
