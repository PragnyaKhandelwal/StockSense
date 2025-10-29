#ifndef CUSTOMCHARTWIDGET_H
#define CUSTOMCHARTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QVector>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QMap>
#include <QPaintEvent>
#include <QLinearGradient>
#include <QPolygonF>
#include <QRect>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QColor>
#include <QDebug>
#include <algorithm>

class CustomChartWidget : public QWidget
{
    Q_OBJECT

public:
    CustomChartWidget(QWidget *parent = nullptr);
    void setTimeframe(const QString &timeframe);
    void updateWithLiveData(const QString &symbol, const QJsonObject &data);
    void setSymbol(const QString &symbol);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateChart();

private:
    void generateRealisticData();
    void updateMinMax();

    QVector<double> m_dataPoints;
    double m_minPrice = 1400;
    double m_maxPrice = 1600;
    QString m_timeframe = "1M";
    QString m_symbol;
    QTimer *m_animationTimer;
};

#endif // CUSTOMCHARTWIDGET_H
