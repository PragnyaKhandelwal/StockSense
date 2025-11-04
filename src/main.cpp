#include <QApplication>
#include "StockSenseApp.h"
#include <QDebug>
#include <exception>

int main(int argc, char *argv[])
{
    try {
        QApplication app(argc, argv);
        
        qDebug() << "=== StockSense Application Starting ===";
        
        StockSenseApp window;
        
        qDebug() << "✅ Application window created successfully";
        
        window.show();
        
        qDebug() << "✅ Application window shown";
        
        return app.exec();
    } 
    catch (const std::exception &e) {
        qCritical() << "❌ Exception caught:" << e.what();
        return -1;
    }
    catch (...) {
        qCritical() << "❌ Unknown exception caught";
        return -1;
    }
}
