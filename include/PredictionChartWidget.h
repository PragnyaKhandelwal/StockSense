#ifndef PREDICTIONCHARTWIDGET_H
#define PREDICTIONCHARTWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QVector>
#include <QString>
#include <QJsonObject>
#include <QTimer>
#include <QHash>
#include <QQueue>
#include <QStack>
#include <queue>
#include <cmath>

class PredictionChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PredictionChartWidget(QWidget *parent = nullptr);
    
    void updateWithLiveData(const QString &symbol, const QVector<double> &prices);
    void setCurrentStock(const QString &symbol);
    QString getCurrentStock() const { return m_currentSymbol; }

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private slots:
    void updatePredictions();

private:
    void requestHistoricalData(const QString &symbol);
    QVector<double> parseYahooData(const QJsonObject &response);
    void analyzeWithAllDSA();
    
    void cacheStock(const QString &symbol, const QVector<double> &prices);
    QVector<double> getCachedStock(const QString &symbol);
    void updateCache(const QString &symbol, double price);
    
    void calculateSlidingWindowIndicators();
    void detectTrendWithStack();
    void findTopPerformers();
    void generateLinearForecast();
    
    double calculateVolatility(const QVector<double> &prices, int period = 10);
    void updateChartBounds();
    void drawDSALegend(QPainter &painter, int startY);
    
    struct StockPerformance {
        QString symbol;
        double price = 0.0;
        double change = 0.0;
        double changePercent = 0.0;
        double volatility = 0.0;
        
        bool operator<(const StockPerformance &other) const {
            return changePercent < other.changePercent;
        }
    };
    
    struct RegressionResult {
        double slope = 0.0;
        double intercept = 0.0;
        double rSquared = 0.0;
    };
    
    QVector<StockPerformance> getTopGainers(int count = 3);
    QVector<StockPerformance> getTopLosers(int count = 3);
    QVector<StockPerformance> getMostVolatile(int count = 3);
    QVector<double> calculatePriceChanges(const QVector<double> &prices);
    QString analyzeTrend(const QVector<double> &prices);
    QVector<double> findSupportResistance(const QVector<double> &prices);
    void generateTradingSignals();
    void drawEnhancedLegend(QPainter &painter, int startY, int startX);
    double calculateSMA(const QVector<double> &prices, int window = 20);
    double calculateEMA(const QVector<double> &prices, int window = 12);
    double calculateRSI(const QVector<double> &prices, int window = 14);
    void calculateBollingerBands(const QVector<double> &prices, int window = 20);
    RegressionResult performLinearRegression(const QVector<double> &prices);
    QVector<double> generateForecast(const RegressionResult &model, int days = 7);
    void generateAdvancedPredictions();
    void tryAlternativeDataSource(const QString &symbol);
    void fetchCurrentPriceOnly(const QString &symbol);
    void generateHistoricalFromCurrentPrice(const QString &symbol, double currentPrice);
    void showNoDataError(const QString &symbol);
    
    QVector<double> m_historicalData;
    QVector<double> m_predictedData;
    QVector<double> m_confidenceIntervals;
    QVector<int> m_tradingSignals;
    QString m_currentSymbol;
    
    double m_minPrice, m_maxPrice;
    QTimer *m_updateTimer;
    
    QHash<QString, QVector<double>> m_stockCache;
    QVector<StockPerformance> m_stockPerformances;
    QVector<double> m_supportLevels, m_resistanceLevels;
    
    double m_smaValue, m_emaValue, m_rsiValue;
    double m_bollingerUpper, m_bollingerLower;
    QString m_trendDirection;
    double m_forecastAccuracy;
    
    int m_cacheHits, m_cacheMisses;
};

#endif
