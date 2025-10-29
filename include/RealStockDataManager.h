#ifndef REALSTOCKDATAMANAGER_H
#define REALSTOCKDATAMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include "MarketStatusChecker.h"

class RealStockDataManager : public QObject
{
    Q_OBJECT

public:
    explicit RealStockDataManager(QObject *parent = nullptr);
    void fetchStockData(const QString &symbol);
    void fetchAllStocks();
    bool isMarketOpen() const;
    QString getMarketStatus() const;

signals:
    void stockDataReceived(const QString &symbol, const QJsonObject &data);
    void apiError(const QString &error);
    void marketStatusChanged(bool isOpen, const QString &statusText);

private slots:
    void onMarketStatusChanged(bool isOpen, const QString &statusText);
    void handleReply();

private:
    void adjustUpdateInterval();
    void fetchFromYahoo(const QString &symbol, const QString &yahooSymbol);
    QJsonObject parseResponse(const QJsonObject &response, const QString &symbol);

    QNetworkAccessManager *m_networkManager;
    QTimer *m_updateTimer;
    MarketStatusChecker *m_marketChecker;
    QMap<QString, QString> m_yahooSymbols;
public:
    void fetchIndexData();
    void fetchNiftyData();
    void fetchSensexData();

signals:
    void indexDataReceived(const QString &index, const QJsonObject &data);

private:
    void fetchNiftyFallback();
    void fetchSensexFallback();
    QJsonObject parseIndexResponse(const QJsonObject &response, const QString &index);

};
#endif // REALSTOCKDATAMANAGER_H
