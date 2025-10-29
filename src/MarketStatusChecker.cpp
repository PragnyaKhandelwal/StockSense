#include "MarketStatusChecker.h"

MarketStatusChecker::MarketStatusChecker(QObject *parent) : QObject(parent)
{
    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &MarketStatusChecker::checkMarketStatus);
    m_statusTimer->start(60000);
    checkMarketStatus();
}

bool MarketStatusChecker::isMarketOpen() const
{
    return m_isMarketOpen;
}

QString MarketStatusChecker::getMarketStatusText() const
{
    return m_marketStatusText;
}

void MarketStatusChecker::checkMarketStatus()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QTime time = currentTime.time();
    int dayOfWeek = currentTime.date().dayOfWeek();
    
    bool wasOpen = m_isMarketOpen;
    bool isWeekday = (dayOfWeek >= 1 && dayOfWeek <= 5);
    bool isMarketHours = (time >= QTime(9, 15) && time <= QTime(15, 30));
    
    m_isMarketOpen = isWeekday && isMarketHours;
    
    if (m_isMarketOpen) {
        m_marketStatusText = QString("🟢 MARKET OPEN - Live Trading");
    } else {
        if (!isWeekday) {
            m_marketStatusText = QString("🔴 MARKET CLOSED - Weekend");
        } else if (time < QTime(9, 15)) {
            QTime openTime(9, 15);
            int minutesToOpen = time.secsTo(openTime) / 60;
            m_marketStatusText = QString("🟡 MARKET CLOSED - Opens in %1 mins").arg(minutesToOpen);
        } else {
            m_marketStatusText = QString("🔴 MARKET CLOSED - After Hours");
        }
    }
    
    if (wasOpen != m_isMarketOpen) {
        emit marketStatusChanged(m_isMarketOpen, m_marketStatusText);
    }
    
    qDebug() << "📊 Market Status:" << m_marketStatusText;
}
