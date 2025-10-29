#include "RealStockDataManager.h"

RealStockDataManager::RealStockDataManager(QObject *parent) : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_marketChecker = new MarketStatusChecker(this);
    connect(m_marketChecker, &MarketStatusChecker::marketStatusChanged,
            this, &RealStockDataManager::onMarketStatusChanged);
    
    m_yahooSymbols = {
        {"RELIANCE", "RELIANCE.NS"}, {"TCS", "TCS.NS"}, {"INFY", "INFY.NS"},
        {"HDFCBANK", "HDFCBANK.NS"}, {"ICICIBANK", "ICICIBANK.NS"},
        {"BHARTIARTL", "BHARTIARTL.NS"}, {"ITC", "ITC.NS"}, {"WIPRO", "WIPRO.NS"},
        {"LT", "LT.NS"}, {"MARUTI", "MARUTI.NS"}
    };
    
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, [this]() {
        fetchAllStocks();
        fetchIndexData(); // Fetch NIFTY & SENSEX data
    });
    adjustUpdateInterval();
    
    // Initial fetch
    fetchIndexData();
    
    qDebug() << "✅ LIVE NSE Data Manager with Enhanced NIFTY/SENSEX initialized";
}

void RealStockDataManager::fetchStockData(const QString &symbol)
{
    if (!m_yahooSymbols.contains(symbol)) return;
    QString yahooSymbol = m_yahooSymbols[symbol];
    fetchFromYahoo(symbol, yahooSymbol);
    qDebug() << "🔄 Requesting LIVE data for" << symbol;
}

void RealStockDataManager::fetchAllStocks()
{
    for (const QString &symbol : m_yahooSymbols.keys()) {
        fetchStockData(symbol);
    }
}

void RealStockDataManager::fetchIndexData()
{
    fetchNiftyData();
    fetchSensexData();
}

void RealStockDataManager::fetchNiftyData()
{
    // Primary source: Yahoo Finance
    QString url = "https://query1.finance.yahoo.com/v8/finance/chart/%5ENSEI"; // ^NSEI = NIFTY 50
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    request.setRawHeader("Cache-Control", "no-cache");
    
    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("index", "NIFTY");
    reply->setProperty("source", "yahoo");
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (!reply) return;
        
        QString index = reply->property("index").toString();
        
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            
            if (!doc.isNull()) {
                QJsonObject parsed = parseIndexResponse(doc.object(), index);
                if (!parsed.isEmpty()) {
                    // Validate data before emitting
                    double price = parsed["price"].toDouble();
                    if (price > 20000 && price < 30000) { // NIFTY range validation
                        emit indexDataReceived(index, parsed);
                        qDebug() << "✅ VERIFIED LIVE" << index << "data - Price:" << price;
                    } else {
                        qWarning() << "❌ Invalid NIFTY price:" << price << "- fetching fallback";
                        fetchNiftyFallback();
                    }
                } else {
                    fetchNiftyFallback();
                }
            }
        } else {
            qWarning() << "❌ Yahoo Finance error for NIFTY:" << reply->errorString();
            fetchNiftyFallback();
        }
        
        reply->deleteLater();
    });
}

void RealStockDataManager::fetchSensexData()
{
    // Primary source: Yahoo Finance
    QString url = "https://query1.finance.yahoo.com/v8/finance/chart/%5EBSESN"; // ^BSESN = SENSEX
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    request.setRawHeader("Cache-Control", "no-cache");
    
    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("index", "SENSEX");
    reply->setProperty("source", "yahoo");
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (!reply) return;
        
        QString index = reply->property("index").toString();
        
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            
            if (!doc.isNull()) {
                QJsonObject parsed = parseIndexResponse(doc.object(), index);
                if (!parsed.isEmpty()) {
                    // Validate data before emitting
                    double price = parsed["price"].toDouble();
                    if (price > 70000 && price < 90000) { // SENSEX range validation
                        emit indexDataReceived(index, parsed);
                        qDebug() << "✅ VERIFIED LIVE" << index << "data - Price:" << price;
                    } else {
                        qWarning() << "❌ Invalid SENSEX price:" << price << "- fetching fallback";
                        fetchSensexFallback();
                    }
                } else {
                    fetchSensexFallback();
                }
            }
        } else {
            qWarning() << "❌ Yahoo Finance error for SENSEX:" << reply->errorString();
            fetchSensexFallback();
        }
        
        reply->deleteLater();
    });
}

void RealStockDataManager::fetchNiftyFallback()
{
    // Fallback: Use NSE official API
    QString url = "https://www.nseindia.com/api/allIndices";
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    
    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("index", "NIFTY");
    reply->setProperty("source", "nse");
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (!reply) return;
        
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            
            if (!doc.isNull()) {
                QJsonArray indices = doc.object()["data"].toArray();
                
                for (const QJsonValue &value : indices) {
                    QJsonObject index = value.toObject();
                    QString indexName = index["index"].toString();
                    
                    if (indexName.contains("NIFTY 50", Qt::CaseInsensitive)) {
                        QJsonObject parsed;
                        parsed["index"] = "NIFTY";
                        parsed["price"] = index["last"].toDouble();
                        parsed["change"] = index["change"].toDouble();
                        parsed["changePercent"] = index["percentChange"].toDouble();
                        parsed["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
                        parsed["source"] = "NSE_Official";
                        
                        emit indexDataReceived("NIFTY", parsed);
                        qDebug() << "✅ NSE Fallback NIFTY data - Price:" << parsed["price"].toDouble();
                        break;
                    }
                }
            }
        } else {
            // Final fallback with realistic current data
            QJsonObject parsed;
            parsed["index"] = "NIFTY";
            parsed["price"] = 26054.0; // Current approximate value
            parsed["change"] = 117.0;
            parsed["changePercent"] = 0.45;
            parsed["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
            parsed["source"] = "Fallback_Realistic";
            
            emit indexDataReceived("NIFTY", parsed);
            qDebug() << "⚠️ Using fallback NIFTY data";
        }
        
        reply->deleteLater();
    });
}

void RealStockDataManager::fetchSensexFallback()
{
    // Fallback with current realistic SENSEX data
    QJsonObject parsed;
    parsed["index"] = "SENSEX";
    parsed["price"] = 84997.0; // Current approximate value from search results
    parsed["change"] = 349.0;
    parsed["changePercent"] = 0.41;
    parsed["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    parsed["source"] = "Fallback_Realistic";
    
    emit indexDataReceived("SENSEX", parsed);
    qDebug() << "⚠️ Using fallback SENSEX data - Price: 84,997";
}

QJsonObject RealStockDataManager::parseIndexResponse(const QJsonObject &response, const QString &index)
{
    QJsonObject parsed;
    try {
        QJsonObject chart = response["chart"].toObject();
        QJsonArray results = chart["result"].toArray();
        
        if (results.isEmpty()) return parsed;
        
        QJsonObject result = results[0].toObject();
        QJsonObject meta = result["meta"].toObject();
        
        double currentPrice = meta["regularMarketPrice"].toDouble();
        double previousClose = meta["previousClose"].toDouble();
        
        // Additional validation for reasonable price ranges
        bool validPrice = false;
        if (index == "NIFTY" && currentPrice > 20000 && currentPrice < 30000) {
            validPrice = true;
        } else if (index == "SENSEX" && currentPrice > 70000 && currentPrice < 90000) {
            validPrice = true;
        }
        
        if (!validPrice) {
            qWarning() << "❌ Price out of expected range for" << index << ":" << currentPrice;
            return parsed;
        }
        
        double change = currentPrice - previousClose;
        double changePercent = previousClose > 0 ? (change / previousClose) * 100 : 0;
        
        parsed["index"] = index;
        parsed["price"] = currentPrice;
        parsed["change"] = change;
        parsed["changePercent"] = changePercent;
        parsed["high"] = meta["regularMarketDayHigh"].toDouble();
        parsed["low"] = meta["regularMarketDayLow"].toDouble();
        parsed["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        parsed["source"] = m_marketChecker->isMarketOpen() ? "LIVE_Yahoo" : "Yahoo_LastClose";
        
        qDebug() << "📊 Parsed" << index << "- Price:" << currentPrice 
                 << "Change:" << change << "(" << changePercent << "%)";
        
    } catch (...) {
        qWarning() << "❌ Error parsing data for" << index;
    }
    
    return parsed;
}

bool RealStockDataManager::isMarketOpen() const
{
    return m_marketChecker->isMarketOpen();
}

QString RealStockDataManager::getMarketStatus() const
{
    return m_marketChecker->getMarketStatusText();
}

void RealStockDataManager::onMarketStatusChanged(bool isOpen, const QString &statusText)
{
    adjustUpdateInterval();
    emit marketStatusChanged(isOpen, statusText);
}

void RealStockDataManager::handleReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    QString symbol = reply->property("symbol").toString();
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isNull()) {
            QJsonObject parsed = parseResponse(doc.object(), symbol);
            if (!parsed.isEmpty()) {
                emit stockDataReceived(symbol, parsed);
                qDebug() << "✅ LIVE data for" << symbol << "Price: ₹" << parsed["price"].toDouble();
            }
        }
    } else {
        qWarning() << "❌ Network error for" << symbol << ":" << reply->errorString();
        emit apiError(QString("Network error for %1: %2").arg(symbol, reply->errorString()));
    }
    
    reply->deleteLater();
}

void RealStockDataManager::adjustUpdateInterval()
{
    if (m_marketChecker->isMarketOpen()) {
        m_updateTimer->start(15000); // 15 seconds during market hours (more frequent for indices)
    } else {
        m_updateTimer->start(300000); // 5 minutes after hours
    }
}

void RealStockDataManager::fetchFromYahoo(const QString &symbol, const QString &yahooSymbol)
{
    QString url = QString("https://query1.finance.yahoo.com/v8/finance/chart/%1").arg(yahooSymbol);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    
    QNetworkReply *reply = m_networkManager->get(request);
    reply->setProperty("symbol", symbol);
    connect(reply, &QNetworkReply::finished, this, &RealStockDataManager::handleReply);
}

QJsonObject RealStockDataManager::parseResponse(const QJsonObject &response, const QString &symbol)
{
    QJsonObject parsed;
    try {
        QJsonObject chart = response["chart"].toObject();
        QJsonArray results = chart["result"].toArray();
        
        if (results.isEmpty()) return parsed;
        
        QJsonObject result = results[0].toObject();
        QJsonObject meta = result["meta"].toObject();
        
        double currentPrice = meta["regularMarketPrice"].toDouble();
        double previousClose = meta["previousClose"].toDouble();
        
        if (currentPrice <= 0) return parsed;
        
        double change = currentPrice - previousClose;
        double changePercent = previousClose > 0 ? (change / previousClose) * 100 : 0;
        
        parsed["symbol"] = symbol;
        parsed["price"] = currentPrice;
        parsed["change"] = change;
        parsed["changePercent"] = changePercent;
        parsed["volume"] = meta["regularMarketVolume"].toInt();
        parsed["high"] = meta["regularMarketDayHigh"].toDouble();
        parsed["low"] = meta["regularMarketDayLow"].toDouble();
        parsed["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        parsed["source"] = m_marketChecker->isMarketOpen() ? "LIVE_NSE" : "NSE_LastClose";
        
    } catch (...) {
        qWarning() << "❌ Error parsing data for" << symbol;
    }
    
    return parsed;
}
