#include "MarketStatusChecker.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDateTime>
#include <QTime>

MarketStatusChecker::MarketStatusChecker(QObject *parent) : QObject(parent),
    m_marketOpen(false), m_marketStatusText("Market Closed")
{
    m_networkManager = new QNetworkAccessManager(this);
    
    // Timer to update every 30 seconds
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &MarketStatusChecker::fetchIndexData);
    m_updateTimer->start(30000);
    
    // Fetch immediately on startup
    fetchIndexData();
    updateMarketStatus();
    qDebug() << "✅ Market Status Checker initialized";
}

void MarketStatusChecker::fetchIndexData()
{
    // Fetch NIFTY 50
    fetchIndexPrice("^NSEI", "NIFTY 50");
    
    // Fetch SENSEX
    fetchIndexPrice("^BSESN", "SENSEX");
}

void MarketStatusChecker::fetchIndexPrice(const QString &symbol, const QString &indexName)
{
    // Yahoo Finance real-time quote API
    QString url = QString("https://query1.finance.yahoo.com/v8/finance/chart/%1?interval=1m&range=1d").arg(symbol);
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    
    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("indexName", indexName);
    reply->setProperty("symbol", symbol);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            parseIndexResponse(data, reply->property("indexName").toString(),
                             reply->property("symbol").toString());
        } else {
            qWarning() << "❌ Failed to fetch" << reply->property("indexName").toString();
        }
        reply->deleteLater();
    });
}

void MarketStatusChecker::parseIndexResponse(const QByteArray &jsonData, 
                                            const QString &indexName,
                                            const QString &symbol)
{
    try {
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        QJsonObject response = doc.object();
        
        if (!response.contains("chart")) return;
        
        QJsonObject chart = response["chart"].toObject();
        QJsonArray result = chart["result"].toArray();
        
        if (result.isEmpty()) return;
        
        QJsonObject data = result[0].toObject();
        QJsonObject meta = data["meta"].toObject();
        
        // Get current price
        double currentPrice = meta["regularMarketPrice"].toDouble();
        double previousClose = meta["previousClose"].toDouble();
        
        // Calculate change
        double change = currentPrice - previousClose;
        double changePercent = (change / previousClose) * 100;
        
        // Format strings
        QString priceStr = QString::number(currentPrice, 'f', 2);
        QString changeStr = QString("%1 (%2%)")
            .arg(QString::number(change, 'f', 2))
            .arg(QString::number(changePercent, 'f', 2));
        
        // Determine color (green for up, red for down)
        QString color = change >= 0 ? "#10b981" : "#ef4444";
        
        // Emit signals
        if (indexName == "NIFTY 50") {
            emit niftyUpdated(priceStr, changeStr, color);
            qDebug() << "📈 NIFTY 50:" << priceStr << "(" << changeStr << ")";
        } else if (indexName == "SENSEX") {
            emit sensexUpdated(priceStr, changeStr, color);
            qDebug() << "📊 SENSEX:" << priceStr << "(" << changeStr << ")";
        }
        
    } catch (const std::exception &e) {
        qWarning() << "❌ Error parsing" << indexName << "data:" << e.what();
    }
}

void MarketStatusChecker::updateMarketStatus()
{
    // NSE market hours: 09:15 AM to 03:30 PM (Mon-Fri)
    QTime currentTime = QTime::currentTime();
    QDateTime now = QDateTime::currentDateTime();
    
    // Check if it's a weekday
    bool isWeekday = now.date().dayOfWeek() >= 1 && now.date().dayOfWeek() <= 5;
    
    // Market hours: 9:15 AM to 3:30 PM
    QTime marketOpen = QTime(9, 15, 0);
    QTime marketClose = QTime(15, 30, 0);
    
    bool wasMarketOpen = m_marketOpen;
    m_marketOpen = isWeekday && currentTime >= marketOpen && currentTime <= marketClose;
    
    if (m_marketOpen) {
        m_marketStatusText = "🟢 MARKET OPEN";
    } else {
        m_marketStatusText = "🔴 MARKET CLOSED";
    }
    
    // Emit signal if market status changed
    if (wasMarketOpen != m_marketOpen) {
        emit marketStatusChanged(m_marketOpen);
    }
}
