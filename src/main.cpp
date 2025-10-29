#include <QApplication>
#include "StockSenseApp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    StockSenseApp window;
    window.show();
    
    return app.exec();
}
