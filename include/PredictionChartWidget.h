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
#include <QScrollArea>
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
    QSize sizeHint() const override; // Add size hint for scrolling

private slots:
    void updatePredictions();

private:
    // Core data methods
    void generateAdvancedPredictions();
    void requestHistoricalData(const QString &symbol);
    QVector<double> parseHistoricalPrices(const QJsonObject &response);
    void generateTradingSignals();
    
    // Real data methods
    void tryAlternativeDataSource(const QString &symbol);
    void fetchCurrentPriceOnly(const QString &symbol);
    void generateHistoricalFromCurrentPrice(const QString &symbol, double currentPrice);
    void showNoDataError(const QString &symbol);
    
    // Enhanced legend method
    void drawEnhancedLegend(QPainter &painter, int startY, int startX);
    
    // DSA Algorithm Implementations
    double calculateSMA(const QVector<double> &prices, int window = 20);
    double calculateEMA(const QVector<double> &prices, int window = 12);
    double calculateRSI(const QVector<double> &prices, int window = 14);
    void calculateBollingerBands(const QVector<double> &prices, int window = 20);
    
    void cacheStock(const QString &symbol, const QVector<double> &prices);
    QVector<double> getCachedStock(const QString &symbol);
    void updateCache(const QString &symbol, double price);
    
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
    
    QVector<StockPerformance> getTopGainers(int count = 3);
    QVector<StockPerformance> getTopLosers(int count = 3);
    QVector<StockPerformance> getMostVolatile(int count = 3);
    
    struct RegressionResult {
        double slope = 0.0;
        double intercept = 0.0;
        double rSquared = 0.0;
    };
    
    RegressionResult performLinearRegression(const QVector<double> &prices);
    QVector<double> generateForecast(const RegressionResult &model, int days = 7);
    
    QString analyzeTrend(const QVector<double> &prices);
    QVector<double> findSupportResistance(const QVector<double> &prices);
    
    double calculateVolatility(const QVector<double> &prices, int period = 10);
    QVector<double> calculatePriceChanges(const QVector<double> &prices);
    
    // Core data
    QVector<double> m_historicalData;
    QVector<double> m_predictedData;
    QVector<double> m_confidenceIntervals;
    QVector<int> m_tradingSignals;
    QString m_currentSymbol;
    
    double m_minPrice, m_maxPrice;
    QTimer *m_updateTimer;
    
    QHash<QString, QVector<double>> m_stockCache;
    QHash<QString, double> m_priceChanges;
    QVector<StockPerformance> m_stockPerformances;
    
    double m_smaValue, m_emaValue, m_rsiValue;
    double m_bollingerUpper, m_bollingerLower;
    QString m_trendDirection;
    double m_forecastAccuracy;
    QVector<double> m_supportLevels, m_resistanceLevels;
    
    int m_cacheHits, m_cacheMisses;
    double m_avgCalculationTime;
};

#endif // PREDICTIONCHARTWIDGET_H
