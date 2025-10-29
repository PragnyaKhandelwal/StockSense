#ifndef REALNEWSMANAGER_H
#define REALNEWSMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QStringList>
#include <QTime>
#include <QDateTime>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

class RealNewsManager : public QObject
{
    Q_OBJECT

public:
    explicit RealNewsManager(QObject *parent = nullptr);
    void fetchNewsForStock(const QString &symbol);

signals:
    void newsReceived(const QStringList &newsItems);

private slots:
    void fetchNewsForCurrentStock();

private:
    void fetchStockSpecificNews(const QString &symbol);
    void fetchGeneralMarketNews();
    void fetchAlternativeNews(const QString &query, const QString &type);
    QStringList generateIntelligentNews(const QString &query, const QString &type);
    
    // Yahoo Finance specific methods
    void fetchYahooStockNews(const QString &symbol);
    void fetchYahooGeneralNews();
    void fetchYahooRSSNews(const QString &symbol, const QString &type);
    QStringList parseYahooNewsResponse(const QByteArray &jsonData, const QString &symbol);
    QStringList parseYahooRSSFeed(const QByteArray &xmlData, const QString &type, const QString &symbol);
    void emitCombinedNews();

    QNetworkAccessManager *m_networkManager;
    QTimer *m_newsTimer;
    QString m_currentSymbol = "RELIANCE";
    QStringList m_stockNews;
    QStringList m_generalNews;
};

#endif // REALNEWSMANAGER_H
