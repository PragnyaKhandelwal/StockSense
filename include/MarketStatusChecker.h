#ifndef MARKETSTATUSCHECKER_H
#define MARKETSTATUSCHECKER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QTime>
#include <QString>
#include <QDebug>

class MarketStatusChecker : public QObject
{
    Q_OBJECT

public:
    explicit MarketStatusChecker(QObject *parent = nullptr);
    bool isMarketOpen() const;
    QString getMarketStatusText() const;

signals:
    void marketStatusChanged(bool isOpen, const QString &statusText);

private slots:
    void checkMarketStatus();

private:
    QTimer *m_statusTimer;
    bool m_isMarketOpen = false;
    QString m_marketStatusText;
};

#endif // MARKETSTATUSCHECKER_H
