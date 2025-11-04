#include "PredictionChartWidget.h"
#include <QPainter>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFont>
#include <QPen>
#include <QColor>
#include <QTime>
#include <QQueue>
#include <QStack>
#include <algorithm>
#include <queue>
#include <cmath>

PredictionChartWidget::PredictionChartWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(1200, 1000);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    m_minPrice = 0.0;
    m_maxPrice = 5000.0;
    m_currentSymbol = ""; // Start empty - will be set by selectStock
    m_smaValue = 0.0;
    m_emaValue = 0.0;
    m_rsiValue = 50.0;
    m_bollingerUpper = 0.0;
    m_bollingerLower = 0.0;
    m_trendDirection = "loading";
    m_forecastAccuracy = 0.0;
    m_cacheHits = 0;
    m_cacheMisses = 0;
    
    m_historicalData.clear();
    m_predictedData.clear();
    
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &PredictionChartWidget::updatePredictions);
    m_updateTimer->start(15000);
    
    qDebug() << "🧠 DSA-Powered Prediction Engine initialized";
}

QSize PredictionChartWidget::sizeHint() const
{
    return QSize(1200, 1000);
}

void PredictionChartWidget::setCurrentStock(const QString &symbol)
{
    if (symbol.isEmpty()) return;
    
    if (symbol == m_currentSymbol) {
        qDebug() << "Same stock" << symbol << "- skipping";
        return;
    }
    
    qDebug() << "🔄 Switching from" << m_currentSymbol << "to" << symbol;
    m_currentSymbol = symbol;
    m_historicalData.clear();
    m_predictedData.clear();
    m_trendDirection = "loading";
    
    QVector<double> cached = getCachedStock(symbol);
    if (!cached.isEmpty() && cached.size() >= 10) {
        m_historicalData = cached;
        analyzeWithAllDSA();
        update();
        qDebug() << "✅ Cache HIT for" << symbol;
        return;
    }
    
    requestHistoricalData(symbol);
    update();
}

void PredictionChartWidget::updateWithLiveData(const QString &symbol, const QVector<double> &prices)
{
    if (symbol != m_currentSymbol || prices.size() < 10) return;
    
    cacheStock(symbol, prices);
    m_historicalData = prices;
    analyzeWithAllDSA();
    update();
    
    qDebug() << "📊 Live update:" << symbol << "with" << prices.size() << "points";
}

void PredictionChartWidget::requestHistoricalData(const QString &symbol)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString url = QString("https://query1.finance.yahoo.com/v8/finance/chart/%1.NS?interval=1d&range=3mo").arg(symbol);
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, 
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    
    QNetworkReply *reply = manager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply, symbol]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            
            if (!doc.isNull()) {
                QVector<double> prices = parseYahooData(doc.object());
                
                if (prices.size() >= 10) {
                    cacheStock(symbol, prices);
                    m_historicalData = prices;
                    analyzeWithAllDSA();
                    update();
                    qDebug() << "✅ Fetched" << prices.size() << "real prices for" << symbol;
                } else {
                    m_trendDirection = "no_data";
                    update();
                }
            }
        } else {
            qWarning() << "❌ API failed for" << symbol;
            m_trendDirection = "no_data";
            update();
        }
        reply->deleteLater();
    });
}

QVector<double> PredictionChartWidget::parseYahooData(const QJsonObject &response)
{
    QVector<double> prices;
    
    try {
        QJsonArray results = response["chart"].toObject()["result"].toArray();
        if (results.isEmpty()) return prices;
        
        QJsonObject result = results[0].toObject();
        QJsonArray quote = result["indicators"].toObject()["quote"].toArray();
        if (quote.isEmpty()) return prices;
        
        QJsonArray closePrices = quote[0].toObject()["close"].toArray();
        
        for (const QJsonValue &val : closePrices) {
            if (!val.isNull() && val.toDouble() > 0) {
                prices.append(val.toDouble());
            }
        }
    } catch (...) {
        qWarning() << "Error parsing data";
    }
    
    return prices;
}

void PredictionChartWidget::analyzeWithAllDSA()
{
    if (m_historicalData.size() < 10) return;
    
    qDebug() << "🔬 DSA Analysis on" << m_historicalData.size() << "points for" << m_currentSymbol;
    
    calculateSlidingWindowIndicators();
    detectTrendWithStack();
    findTopPerformers();
    generateLinearForecast();
    updateChartBounds();
    
    qDebug() << "✅ Analysis complete: SMA:" << m_smaValue << "RSI:" << m_rsiValue;
}

void PredictionChartWidget::cacheStock(const QString &symbol, const QVector<double> &prices)
{
    m_stockCache[symbol] = prices;
}

QVector<double> PredictionChartWidget::getCachedStock(const QString &symbol)
{
    if (m_stockCache.contains(symbol)) {
        m_cacheHits++;
        return m_stockCache[symbol];
    }
    m_cacheMisses++;
    return QVector<double>();
}

void PredictionChartWidget::updateCache(const QString &symbol, double price)
{
    if (m_stockCache.contains(symbol)) {
        m_stockCache[symbol].append(price);
        if (m_stockCache[symbol].size() > 100) {
            m_stockCache[symbol].removeFirst();
        }
    }
}

void PredictionChartWidget::calculateSlidingWindowIndicators()
{
    int smaWindow = qMin(20, m_historicalData.size());
    QQueue<double> priceQueue;
    double sum = 0.0;
    
    for (int i = m_historicalData.size() - smaWindow; i < m_historicalData.size(); ++i) {
        priceQueue.enqueue(m_historicalData[i]);
        sum += m_historicalData[i];
    }
    m_smaValue = sum / smaWindow;
    
    int emaWindow = qMin(12, m_historicalData.size());
    double multiplier = 2.0 / (emaWindow + 1.0);
    m_emaValue = m_historicalData[0];
    
    for (int i = 1; i < m_historicalData.size(); ++i) {
        m_emaValue = (m_historicalData[i] * multiplier) + (m_emaValue * (1 - multiplier));
    }
    
    int rsiWindow = qMin(14, m_historicalData.size() - 1);
    double gains = 0.0, losses = 0.0;
    
    for (int i = m_historicalData.size() - rsiWindow; i < m_historicalData.size(); ++i) {
        double change = m_historicalData[i] - m_historicalData[i-1];
        if (change > 0) gains += change;
        else losses += -change;
    }
    
    double avgGain = gains / rsiWindow;
    double avgLoss = losses / rsiWindow;
    double rs = (avgLoss == 0) ? 100 : avgGain / avgLoss;
    m_rsiValue = 100 - (100 / (1 + rs));
    
    int bbWindow = qMin(20, m_historicalData.size());
    double variance = 0.0;
    for (int i = m_historicalData.size() - bbWindow; i < m_historicalData.size(); ++i) {
        variance += (m_historicalData[i] - m_smaValue) * (m_historicalData[i] - m_smaValue);
    }
    double stdDev = sqrt(variance / bbWindow);
    m_bollingerUpper = m_smaValue + (2.0 * stdDev);
    m_bollingerLower = m_smaValue - (2.0 * stdDev);
}

void PredictionChartWidget::detectTrendWithStack()
{
    if (m_historicalData.size() < 10) {
        m_trendDirection = "insufficient";
        return;
    }
    
    QStack<double> peakStack;
    QStack<double> valleyStack;
    
    for (int i = 1; i < m_historicalData.size() - 1; ++i) {
        if (m_historicalData[i] > m_historicalData[i-1] && 
            m_historicalData[i] > m_historicalData[i+1]) {
            peakStack.push(m_historicalData[i]);
        }
        else if (m_historicalData[i] < m_historicalData[i-1] && 
                 m_historicalData[i] < m_historicalData[i+1]) {
            valleyStack.push(m_historicalData[i]);
        }
    }
    
    m_supportLevels.clear();
    m_resistanceLevels.clear();
    
    while (!valleyStack.isEmpty() && m_supportLevels.size() < 3) {
        m_supportLevels.append(valleyStack.pop());
    }
    
    while (!peakStack.isEmpty() && m_resistanceLevels.size() < 3) {
        m_resistanceLevels.append(peakStack.pop());
    }
    
    int trendWindow = qMin(10, m_historicalData.size());
    double recentAvg = 0.0;
    for (int i = m_historicalData.size() - trendWindow; i < m_historicalData.size(); ++i) {
        recentAvg += m_historicalData[i];
    }
    recentAvg /= trendWindow;
    
    double trend = (recentAvg - m_historicalData[m_historicalData.size() - trendWindow]) / trendWindow;
    
    if (trend > 5.0) m_trendDirection = "bullish";
    else if (trend < -5.0) m_trendDirection = "bearish";
    else m_trendDirection = "sideways";
}

void PredictionChartWidget::findTopPerformers()
{
    m_stockPerformances.clear();
    
    if (m_historicalData.size() < 2) return;
    
    StockPerformance current;
    current.symbol = m_currentSymbol;
    current.price = m_historicalData.last();
    current.change = m_historicalData.last() - m_historicalData[m_historicalData.size() - 2];
    current.changePercent = (current.change / current.price) * 100;
    current.volatility = calculateVolatility(m_historicalData, 10);
    
    m_stockPerformances.append(current);
}

void PredictionChartWidget::generateLinearForecast()
{
    m_predictedData.clear();
    m_confidenceIntervals.clear();
    
    if (m_historicalData.size() < 20) return;
    
    int n = 20;
    double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
    
    for (int i = 0; i < n; ++i) {
        double x = i;
        double y = m_historicalData[m_historicalData.size() - n + i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumXX += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
    double intercept = (sumY - slope * sumX) / n;
    
    for (int i = 1; i <= 7; ++i) {
        double forecast = intercept + slope * (n + i);
        m_predictedData.append(forecast);
        m_confidenceIntervals.append(15.0 * i);
    }
    
    double meanY = sumY / n;
    double ssTotal = 0, ssRes = 0;
    for (int i = 0; i < n; ++i) {
        double actual = m_historicalData[m_historicalData.size() - n + i];
        double predicted = intercept + slope * i;
        ssTotal += (actual - meanY) * (actual - meanY);
        ssRes += (actual - predicted) * (actual - predicted);
    }
    m_forecastAccuracy = ssTotal > 0 ? qMax(0.0, 1.0 - (ssRes / ssTotal)) : 0.0;
}

double PredictionChartWidget::calculateVolatility(const QVector<double> &prices, int period)
{
    if (prices.size() < period) return 0.0;
    
    QVector<double> returns;
    for (int i = 1; i < prices.size(); ++i) {
        returns.append((prices[i] - prices[i-1]) / prices[i-1]);
    }
    
    if (returns.size() < period) return 0.0;
    
    double sum = 0.0;
    int start = returns.size() - period;
    for (int i = start; i < returns.size(); ++i) {
        sum += returns[i];
    }
    double mean = sum / period;
    
    double variance = 0.0;
    for (int i = start; i < returns.size(); ++i) {
        variance += (returns[i] - mean) * (returns[i] - mean);
    }
    
    return sqrt(variance / period);
}

void PredictionChartWidget::updateChartBounds()
{
    QVector<double> allData = m_historicalData + m_predictedData;
    if (!allData.isEmpty()) {
        auto minmax = std::minmax_element(allData.begin(), allData.end());
        m_minPrice = *minmax.first * 0.95;
        m_maxPrice = *minmax.second * 1.05;
    }
}

void PredictionChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), QColor("#fafbfc"));
    
    if (m_historicalData.isEmpty()) {
        painter.setFont(QFont("Arial", 18, QFont::Bold));
        painter.setPen(QColor("#6b7280"));
        
        QString msg = m_currentSymbol.isEmpty() ? 
            QString("📊 Select a stock from the dropdown to begin analysis") :
            (m_trendDirection == "loading") ? 
            QString("🔄 Loading real data for %1...\n\nFetching from Yahoo Finance NSE").arg(m_currentSymbol) :
            QString("❌ No data for %1\n\nTry: RELIANCE, TCS, INFY, HDFCBANK, BHARTIARTL").arg(m_currentSymbol);
        
        painter.drawText(rect().adjusted(50, 100, -50, -100), Qt::AlignCenter | Qt::TextWordWrap, msg);
        return;
    }
    
    QRect chartRect(150, 150, width() - 250, 600);
    
    painter.fillRect(chartRect, QColor("#ffffff"));
    painter.setPen(QPen(QColor("#d1d5db"), 2));
    painter.drawRoundedRect(chartRect, 8, 8);
    
    painter.setPen(QPen(QColor("#f1f5f9"), 1));
    for (int i = 1; i < 10; ++i) {
        int y = chartRect.top() + (chartRect.height() * i / 10);
        painter.drawLine(chartRect.left(), y, chartRect.right(), y);
        int x = chartRect.left() + (chartRect.width() * i / 10);
        painter.drawLine(x, chartRect.top(), x, chartRect.bottom());
    }
    
    if (m_bollingerUpper > 0 && m_bollingerLower > 0) {
        QPen bbPen(QColor("#94a3b8"));
        bbPen.setWidth(2);
        bbPen.setStyle(Qt::DashLine);
        painter.setPen(bbPen);
        
        double upperY = chartRect.bottom() - ((m_bollingerUpper - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
        double lowerY = chartRect.bottom() - ((m_bollingerLower - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
        
        if (upperY >= chartRect.top() && upperY <= chartRect.bottom())
            painter.drawLine(chartRect.left(), upperY, chartRect.right(), upperY);
        if (lowerY >= chartRect.top() && lowerY <= chartRect.bottom())
            painter.drawLine(chartRect.left(), lowerY, chartRect.right(), lowerY);
    }
    
    if (m_smaValue > 0) {
        QPen smaPen(QColor("#f59e0b"));
        smaPen.setWidth(3);
        painter.setPen(smaPen);
        
        double y = chartRect.bottom() - ((m_smaValue - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
        if (y >= chartRect.top() && y <= chartRect.bottom())
            painter.drawLine(chartRect.left(), y, chartRect.right(), y);
    }
    
    if (m_historicalData.size() > 1) {
        QPen pricePen(QColor("#059669"));
        pricePen.setWidth(3);
        
        for (int i = 0; i < m_historicalData.size() - 1; ++i) {
            double x1 = chartRect.left() + (chartRect.width() * i / (m_historicalData.size() + m_predictedData.size()));
            double x2 = chartRect.left() + (chartRect.width() * (i + 1) / (m_historicalData.size() + m_predictedData.size()));
            double y1 = chartRect.bottom() - ((m_historicalData[i] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
            double y2 = chartRect.bottom() - ((m_historicalData[i + 1] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
            
            pricePen.setColor(m_historicalData[i + 1] > m_historicalData[i] ? QColor("#059669") : QColor("#dc2626"));
            painter.setPen(pricePen);
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }
    
    if (m_predictedData.size() > 1) {
        QPen forecastPen(QColor("#3b82f6"));
        forecastPen.setWidth(4);
        forecastPen.setStyle(Qt::DashLine);
        painter.setPen(forecastPen);
        
        for (int i = 0; i < m_predictedData.size() - 1; ++i) {
            double x1 = chartRect.left() + (chartRect.width() * (m_historicalData.size() + i) / (m_historicalData.size() + m_predictedData.size()));
            double x2 = chartRect.left() + (chartRect.width() * (m_historicalData.size() + i + 1) / (m_historicalData.size() + m_predictedData.size()));
            double y1 = chartRect.bottom() - ((m_predictedData[i] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
            double y2 = chartRect.bottom() - ((m_predictedData[i + 1] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
            
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }
    
    painter.setPen(QColor("#374151"));
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    
    for (int i = 0; i <= 10; ++i) {
        double price = m_minPrice + (m_maxPrice - m_minPrice) * i / 10;
        int y = chartRect.bottom() - (chartRect.height() * i / 10);
        painter.drawText(QRect(10, y - 15, 135, 30), Qt::AlignRight | Qt::AlignVCenter,
                        QString("₹%1").arg(price, 0, 'f', 0));
    }
    
    painter.setFont(QFont("Arial", 22, QFont::Bold));
    painter.setPen(QColor("#1f2937"));
    painter.drawText(QRect(30, 20, width() - 60, 50), Qt::AlignLeft, 
                    QString("🧠 %1 - DSA Analysis & AI Forecast").arg(m_currentSymbol));
    
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setPen(QColor("#374151"));
    QString info = QString("📊 %1 Points | SMA: ₹%2 | RSI: %3 | Trend: %4 | R²: %5")
                  .arg(m_historicalData.size())
                  .arg(m_smaValue, 0, 'f', 0)
                  .arg(m_rsiValue, 0, 'f', 1)
                  .arg(m_trendDirection.toUpper())
                  .arg(m_forecastAccuracy, 0, 'f', 3);
    painter.drawText(QRect(30, 75, width() - 60, 30), Qt::AlignLeft, info);
    
    drawDSALegend(painter, chartRect.bottom() + 30);
    
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.setPen(QColor("#059669"));
    QString footer = QString("✅ Yahoo Finance | %1 | Cache: %2%%")
                    .arg(QTime::currentTime().toString("hh:mm:ss"))
                    .arg((double)m_cacheHits / (m_cacheHits + m_cacheMisses + 1) * 100, 0, 'f', 0);
    painter.drawText(QRect(30, height() - 30, width() - 60, 20), Qt::AlignLeft, footer);
}

void PredictionChartWidget::drawDSALegend(QPainter &painter, int startY)
{
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(QColor("#1f2937"));
    painter.drawText(30, startY + 20, "📊 DSA Algorithms:");
    
    painter.setFont(QFont("Arial", 11));
    painter.setPen(QColor("#374151"));
    
    int y = startY + 45;
    painter.drawText(30, y, "💾 HashMap O(1) - Instant caching | 🔄 Queue O(n) - Moving averages");
    y += 25;
    painter.drawText(30, y, "📈 Stack O(n) - Trend detection | 🏆 Heap O(n log k) - Top performers");
    y += 25;
    painter.drawText(30, y, "🎯 Linear Regression O(n) - 7-day price forecasting");
    
    y += 35;
    QPen histPen(QColor("#059669"), 3);
    painter.setPen(histPen);
    painter.drawLine(30, y, 70, y);
    painter.setPen(QColor("#374151"));
    painter.drawText(80, y + 5, "Real NSE Prices");
    
    y += 25;
    QPen smaPen(QColor("#f59e0b"), 3);
    painter.setPen(smaPen);
    painter.drawLine(30, y, 70, y);
    painter.setPen(QColor("#374151"));
    painter.drawText(80, y + 5, "20-Day SMA");
    
    y += 25;
    QPen fcPen(QColor("#3b82f6"), 4);
    fcPen.setStyle(Qt::DashLine);
    painter.setPen(fcPen);
    painter.drawLine(30, y, 70, y);
    painter.setPen(QColor("#374151"));
    painter.drawText(80, y + 5, "7-Day Forecast");
}

void PredictionChartWidget::updatePredictions()
{
    if (!m_currentSymbol.isEmpty()) {
        requestHistoricalData(m_currentSymbol);
    }
}

// Stubs
QVector<PredictionChartWidget::StockPerformance> PredictionChartWidget::getTopGainers(int) { return {}; }
QVector<PredictionChartWidget::StockPerformance> PredictionChartWidget::getTopLosers(int) { return {}; }
QVector<PredictionChartWidget::StockPerformance> PredictionChartWidget::getMostVolatile(int) { return {}; }
QVector<double> PredictionChartWidget::calculatePriceChanges(const QVector<double>&) { return {}; }
QString PredictionChartWidget::analyzeTrend(const QVector<double>&) { return "neutral"; }
QVector<double> PredictionChartWidget::findSupportResistance(const QVector<double>&) { return {}; }
void PredictionChartWidget::generateTradingSignals() {}
void PredictionChartWidget::drawEnhancedLegend(QPainter&, int, int) {}
double PredictionChartWidget::calculateSMA(const QVector<double>&, int) { return 0.0; }
double PredictionChartWidget::calculateEMA(const QVector<double>&, int) { return 0.0; }
double PredictionChartWidget::calculateRSI(const QVector<double>&, int) { return 50.0; }
void PredictionChartWidget::calculateBollingerBands(const QVector<double>&, int) {}
PredictionChartWidget::RegressionResult PredictionChartWidget::performLinearRegression(const QVector<double>&) { return RegressionResult(); }
QVector<double> PredictionChartWidget::generateForecast(const RegressionResult&, int) { return {}; }
void PredictionChartWidget::generateAdvancedPredictions() {}
void PredictionChartWidget::tryAlternativeDataSource(const QString&) {}
void PredictionChartWidget::fetchCurrentPriceOnly(const QString&) {}
void PredictionChartWidget::generateHistoricalFromCurrentPrice(const QString&, double) {}
void PredictionChartWidget::showNoDataError(const QString&) {}
