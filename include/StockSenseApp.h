#ifndef STOCKSENSEAPP_H
#define STOCKSENSEAPP_H
#include <QDesktopServices>
#include <QUrl>

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
    void onMarketStatusChanged(bool isOpen);
    void onNewsReceived(const QStringList &newsItems);
    void switchView(const QString &viewName);
    void selectStock(const QString &symbol);
    void updateUIWithLiveData(const QJsonObject &data);
    void updateStockData();
    void updateSentimentMeter();
    void updateNiftyDisplay(const QString &price, const QString &change, const QString &color);
    void updateSensexDisplay(const QString &price, const QString &change, const QString &color);

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
    QWidget *createSettings();
    void setupConnections();
    void setupTimer();
    void applyStyles();

    // UI Components
    QStackedWidget *m_contentStack;
    QWidget *m_dashboardWidget;
    QWidget *m_predictionsWidget;
    QWidget *m_watchlistWidget;
    QWidget *m_settingsWidget;

    // Navigation buttons
    QPushButton *m_dashboardBtn;
    QPushButton *m_predictionsBtn;
    QPushButton *m_watchlistBtn;
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
    QLabel *m_predictionStockTitle;
    QLabel *m_predictionStockDetails;

private:
    QLabel *m_niftyLabel;
    QLabel *m_niftyChangeLabel;
    QLabel *m_sensexLabel;
    QLabel *m_sensexChangeLabel;
    MarketStatusChecker *m_marketStatusChecker;

    // Sentiment components
    QLabel *m_sentimentScore;
    QLabel *m_sentimentLabel;
    QProgressBar *m_sentimentProgress;

private:
    // Dashboard labels
    QLabel *m_dashboardNiftyPrice;
    QLabel *m_dashboardNiftyChange;
    QLabel *m_dashboardSensexPrice;
    QLabel *m_dashboardSensexChange;
    QLabel *m_marketStatusLabel;

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
    // Watchlist: symbol -> latest data
    QMap<QString, QJsonObject> m_watchlist;

    // Add member variables for NIFTY/SENSEX display
    QLabel *m_niftyValueLabel;
    QLabel *m_sensexValueLabel;
    QStringList getComprehensiveStockList() const;
    void addToWatchlist(const QString& symbol);
void removeFromWatchlist(const QString& symbol);
void refreshWatchlistTable();
private:
    QTableWidget* m_watchlistTable;

};

#endif // STOCKSENSEAPP_H
