#pragma once

#include <QList>
#include <QPointF>
#include <QSharedPointer>
#include <QString>

struct Series {
    QVector<QList<QPointF>> data;
    QVector<QPointF> peaks;
    QVector<QString> names;
    double xMin = 0;
    double xMax = 1;
    double yMin = 0;
    double yMax = 0;
};

using SeriesPtr = QSharedPointer<Series>;
Q_DECLARE_METATYPE(SeriesPtr)
