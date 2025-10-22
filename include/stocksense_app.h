#ifndef STOCKSENSE_APP_H
#define STOCKSENSE_APP_H

#include <QObject>
#include <QString>

class StockSenseApp : public QObject
{
    Q_OBJECT

public:
    explicit StockSenseApp(QObject *parent = nullptr);
    
    void initialize();
    QString getVersion() const;

signals:
    void dataUpdated();

private:
    // Application logic will go here
};

#endif // STOCKSENSE_APP_H
