#include "stocksense_app.h"
#include <QDebug>

StockSenseApp::StockSenseApp(QObject *parent)
    : QObject(parent)
{
    qDebug() << "StockSenseApp initialized";
}

void StockSenseApp::initialize()
{
    qDebug() << "StockSenseApp application logic initialized";
}

QString StockSenseApp::getVersion() const
{
    return "1.0.0";
}
StockData getCurrentStockData() {
    if (m_stockDatabase.contains(m_currentStock)) {
        return m_stockDatabase[m_currentStock];
    }
    return m_stockDatabase["RELIANCE"]; // Default fallback
}

