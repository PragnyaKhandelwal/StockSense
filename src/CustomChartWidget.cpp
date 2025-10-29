#include "CustomChartWidget.h"
#include <cmath>
#include <algorithm>

CustomChartWidget::CustomChartWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(350);
    m_symbol = "RELIANCE";
    generateRealisticData();
    
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &CustomChartWidget::updateChart);
    m_animationTimer->start(5000);
}

void CustomChartWidget::setTimeframe(const QString &timeframe)
{
    m_timeframe = timeframe;
    generateRealisticData();
    update();
}

void CustomChartWidget::updateWithLiveData(const QString &symbol, const QJsonObject &data)
{
    if (symbol != m_symbol) return;
    
    double price = data["price"].toDouble();
    if (price > 0) {
        m_dataPoints.append(price);
        if (m_dataPoints.size() > 50) {
            m_dataPoints.removeFirst();
        }
        updateMinMax();
        update();
        qDebug() << "📈 Chart updated with live price ₹" << price << "for" << symbol;
    }
}

void CustomChartWidget::setSymbol(const QString &symbol)
{
    if (m_symbol != symbol) {
        m_symbol = symbol;
        generateRealisticData();
        update();
    }
}

void CustomChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Better margins to prevent cramping
    QRect chartRect = rect().adjusted(70, 50, -50, -70);
    
    // Background
    painter.fillRect(rect(), QColor("#ffffff"));
    painter.setPen(QPen(QColor("#e5e7eb"), 1));
    painter.drawRect(chartRect);
    
    // Cleaner grid lines
    painter.setPen(QPen(QColor("#f1f5f9"), 1));
    for (int i = 1; i < 6; ++i) {
        int y = chartRect.top() + (chartRect.height() * i / 6);
        painter.drawLine(chartRect.left(), y, chartRect.right(), y);
    }
    
    // Draw price line
    if (m_dataPoints.size() > 1) {
        painter.setPen(QPen(QColor("#3b82f6"), 2));
        for (int i = 0; i < m_dataPoints.size() - 1; ++i) {
            double x1 = chartRect.left() + (chartRect.width() * i / (m_dataPoints.size() - 1));
            double x2 = chartRect.left() + (chartRect.width() * (i + 1) / (m_dataPoints.size() - 1));
            double y1 = chartRect.bottom() - ((m_dataPoints[i] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
            double y2 = chartRect.bottom() - ((m_dataPoints[i + 1] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }
    
    // Fill area under curve
    QPolygonF polygon;
    for (int i = 0; i < m_dataPoints.size(); ++i) {
        double x = chartRect.left() + (chartRect.width() * i / (m_dataPoints.size() - 1));
        double y = chartRect.bottom() - ((m_dataPoints[i] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
        polygon << QPointF(x, y);
    }
    
    polygon << QPointF(chartRect.right(), chartRect.bottom());
    polygon << QPointF(chartRect.left(), chartRect.bottom());
    
    QLinearGradient gradient(0, chartRect.top(), 0, chartRect.bottom());
    gradient.setColorAt(0, QColor(59, 130, 246, 80));
    gradient.setColorAt(1, QColor(59, 130, 246, 15));
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(polygon);
    
    // Better Y-axis labels
    painter.setPen(QPen(QColor("#6b7280"), 1));
    painter.setFont(QFont("Arial", 9));
    for (int i = 0; i <= 5; ++i) {
        double price = m_minPrice + (m_maxPrice - m_minPrice) * i / 5;
        int y = chartRect.bottom() - (chartRect.height() * i / 5);
        painter.drawText(QRect(5, y - 12, 60, 24), Qt::AlignRight | Qt::AlignVCenter,
                        QString("₹%1").arg(price, 0, 'f', 1));
    }
    
    // Current price indicator
    if (!m_dataPoints.isEmpty()) {
        double currentPrice = m_dataPoints.last();
        double y = chartRect.bottom() - ((currentPrice - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
        painter.setPen(QPen(QColor("#10b981"), 2));
        painter.drawLine(chartRect.left(), y, chartRect.right(), y);
        
        // Price label
        QRect priceRect(chartRect.right() - 85, y - 15, 80, 30);
        painter.fillRect(priceRect, QColor("#10b981"));
        painter.setPen(QPen(QColor("#ffffff"), 1));
        painter.setFont(QFont("Arial", 9, QFont::Bold));
        painter.drawText(priceRect, Qt::AlignCenter, QString("₹%1").arg(currentPrice, 0, 'f', 2));
    }
    
    // IMPORTANT: Title with current symbol (DYNAMIC)
    painter.setPen(QPen(QColor("#1f2937"), 1));
    painter.setFont(QFont("Arial", 13, QFont::Bold));
    painter.drawText(QRect(70, 15, chartRect.width(), 30), Qt::AlignLeft | Qt::AlignVCenter,
                    QString("%1 Stock Chart (%2) - LIVE NSE").arg(m_symbol).arg(m_timeframe));
}

void CustomChartWidget::updateChart()
{
    if (!m_dataPoints.isEmpty()) {
        double lastPrice = m_dataPoints.last();
        double variation = (QRandomGenerator::global()->bounded(100) - 50) * 0.1;
        double newPrice = lastPrice + variation;
        
        newPrice = qMax(lastPrice * 0.999, qMin(lastPrice * 1.001, newPrice));
        
        m_dataPoints.append(newPrice);
        if (m_dataPoints.size() > 50) {
            m_dataPoints.removeFirst();
        }
        updateMinMax();
        update();
    }
}

void CustomChartWidget::generateRealisticData()
{
    m_dataPoints.clear();
    
    QMap<QString, double> realPrices = {
        {"RELIANCE", 1486.90}, {"TCS", 3084.90}, {"INFY", 1789.45},
        {"HDFCBANK", 1642.80}, {"ICICIBANK", 985.60}, {"BHARTIARTL", 892.75},
        {"ITC", 435.80}, {"WIPRO", 398.90}, {"LT", 2923.40}, {"MARUTI", 10156.75}
    };
    
    double basePrice = realPrices.value(m_symbol, 1486.90);
    int points = 30;
    
    if (m_timeframe == "1D") points = 24;
    else if (m_timeframe == "1W") points = 7;
    else if (m_timeframe == "1M") points = 30;
    else if (m_timeframe == "3M") points = 90;
    else if (m_timeframe == "1Y") points = 52;
    
    for (int i = 0; i < points; ++i) {
        double price = basePrice + (QRandomGenerator::global()->bounded(60) - 30) + std::sin(i * 0.2) * 15;
        m_dataPoints.append(price);
    }
    
    updateMinMax();
}

void CustomChartWidget::updateMinMax()
{
    if (m_dataPoints.isEmpty()) return;
    
    m_minPrice = *std::min_element(m_dataPoints.begin(), m_dataPoints.end());
    m_maxPrice = *std::max_element(m_dataPoints.begin(), m_dataPoints.end());
    
    double padding = (m_maxPrice - m_minPrice) * 0.1;
    m_minPrice -= padding;
    m_maxPrice += padding;
}
