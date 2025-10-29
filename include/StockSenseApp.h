#ifndef STOCKSENSEAPP_H
#define STOCKSENSEAPP_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QTimer>
#include <QScrollArea>
#include <QFrame>
#include <QComboBox>
#include <QTableWidget>
#include <QCheckBox>
#include <QCloseEvent>
#include <QJsonObject>
#include <QMap>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>

#include "RealStockDataManager.h"
#include "RealNewsManager.h"
#include "CustomChartWidget.h"
#include "PredictionChartWidget.h"

class StockSenseApp : public QMainWindow
{
    Q_OBJECT

public:
    StockSenseApp(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLiveDataReceived(const QString &symbol, const QJsonObject &data);
    void onMarketStatusChanged(bool isOpen, const QString &statusText);
    void onNewsReceived(const QStringList &newsItems);
    void onIndexDataReceived(const QString &index, const QJsonObject &data);  // ADD THIS LINE
    void switchView(const QString &viewName);
    void selectStock(const QString &symbol);
    void updateUIWithLiveData(const QJsonObject &data);
    void updateStockData();
    void updateSentimentMeter();

private:
    void initializePointers();
    void updateChartTitleForStock(const QString &symbol);
    void forceUISync();
    void setupUI();
    QWidget *createSidebar();
    QWidget *createContentArea();
    QWidget *createHeader();
    QWidget *createDashboard();
    QWidget *createCustomChart();
    QFrame *createSentimentMeter();
    QFrame *createNewsPanel();
    QWidget *createPredictions();
    QWidget *createWatchlist();
    QWidget *createAlgorithmMonitor();
    QWidget *createSettings();
    void setupConnections();
    void setupTimer();
    void applyStyles();

    // UI Components
    QStackedWidget *m_contentStack;
    QWidget *m_dashboardWidget;
    QWidget *m_predictionsWidget;
    QWidget *m_watchlistWidget;
    QWidget *m_algorithmWidget;
    QWidget *m_settingsWidget;
    
    // Navigation buttons
    QPushButton *m_dashboardBtn;
    QPushButton *m_predictionsBtn;
    QPushButton *m_watchlistBtn;
    QPushButton *m_algorithmBtn;
    QPushButton *m_settingsBtn;
    
    // Search components
    QLineEdit *m_searchInput;
    QListWidget *m_stockSuggestions;
    
    // Price display components
    QLabel *m_currentPriceLabel;
    QLabel *m_priceChangeLabel;
    QLabel *m_trendIcon;
    QLabel *m_headerStockSymbol;
    QLabel *m_quickStatsLabel;
    
    // Sentiment components
    QLabel *m_sentimentScore;
    QLabel *m_sentimentLabel;
    QProgressBar *m_sentimentProgress;
    
    // News and chart components
    QListWidget *m_newsList;
    CustomChartWidget *m_customChart;
    QLabel *m_chartTitleLabel;
    QLabel *m_chartPriceLabel;
    QLabel *m_chartChangeLabel;
    QLabel *m_chartTrendIcon;
    
    // Data managers
    RealStockDataManager *m_realDataManager;
    RealNewsManager *m_newsManager;
    
    // Timer and data
    QTimer *m_updateTimer;
    QString m_currentStock = "RELIANCE";
    QMap<QString, QJsonObject> m_liveStockData;
    
    // Add member variables for NIFTY/SENSEX display
    QLabel *m_niftyValueLabel;
    QLabel *m_sensexValueLabel;
};

#endif // STOCKSENSEAPP_H
