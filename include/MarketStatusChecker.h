#ifndef MARKETSTATUSCHECKER_H
#define MARKETSTATUSCHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>

class MarketStatusChecker : public QObject
{
    Q_OBJECT

public:
    explicit MarketStatusChecker(QObject *parent = nullptr);
    void fetchIndexData();
    
    // Methods that RealStockDataManager needs
    bool isMarketOpen() const { return m_marketOpen; }
    QString getMarketStatusText() const { return m_marketStatusText; }

signals:
    void niftyUpdated(const QString &price, const QString &change, const QString &color);
    void sensexUpdated(const QString &price, const QString &change, const QString &color);
    void marketStatusChanged(bool isOpen);  // Signal for when market opens/closes

private:
    void fetchIndexPrice(const QString &symbol, const QString &indexName);
    void parseIndexResponse(const QByteArray &jsonData, const QString &indexName, const QString &symbol);
    void updateMarketStatus();
    
    QNetworkAccessManager *m_networkManager;
    QTimer *m_updateTimer;
    bool m_marketOpen;
    QString m_marketStatusText;
};

#endif // MARKETSTATUSCHECKER_H
