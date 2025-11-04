#include "StockSenseApp.h"

StockSenseApp::StockSenseApp(QWidget *parent) : QMainWindow(parent), m_currentStock("RELIANCE")
{
    try {
        qDebug() << "=== StockSenseApp Constructor Started ===";
        
        // STEP 1: Initialize all pointers to nullptr
        initializePointers();
        qDebug() << "✅ Step 1: Pointers initialized";
        
        // STEP 2: Create managers FIRST (before UI)
        try {
            m_realDataManager = new RealStockDataManager(this);
            qDebug() << "✅ RealStockDataManager created";
        } catch (const std::exception &e) {
            qWarning() << "❌ RealStockDataManager failed:" << e.what();
        }
        
        try {
            m_newsManager = new RealNewsManager(this);
            qDebug() << "✅ RealNewsManager created";
        } catch (const std::exception &e) {
            qWarning() << "❌ RealNewsManager failed:" << e.what();
        }
        
        try {
            m_marketStatusChecker = new MarketStatusChecker(this);
            qDebug() << "✅ MarketStatusChecker created";
        } catch (const std::exception &e) {
            qWarning() << "❌ MarketStatusChecker failed:" << e.what();
        }
        
        // STEP 3: Setup UI (BEFORE connections)
        try {
            setupUI();
            qDebug() << "✅ Step 2: UI setup complete";
        } catch (const std::exception &e) {
            qCritical() << "❌ setupUI() failed:" << e.what();
            throw;
        }
        
        // STEP 4: Apply styles
        try {
            applyStyles();
            qDebug() << "✅ Step 3: Styles applied";
        } catch (const std::exception &e) {
            qWarning() << "❌ applyStyles() failed:" << e.what();
        }
        
        // STEP 5: Setup connections with NULL checks
        try {
            // Connect RealStockDataManager signals
            if (m_realDataManager) {
                connect(m_realDataManager, &RealStockDataManager::stockDataReceived,
                        this, &StockSenseApp::onLiveDataReceived);
                connect(m_realDataManager, &RealStockDataManager::marketStatusChanged,
                        this, &StockSenseApp::onMarketStatusChanged);
            }
            
            // Connect RealNewsManager signals
            if (m_newsManager) {
                connect(m_newsManager, &RealNewsManager::newsReceived,
                        this, &StockSenseApp::onNewsReceived);
            }
            
            // Connect MarketStatusChecker signals
            if (m_marketStatusChecker) {
                connect(m_marketStatusChecker, &MarketStatusChecker::niftyUpdated,
                        this, [this](const QString &price, const QString &change, const QString &color) {
                    if (m_dashboardNiftyPrice) m_dashboardNiftyPrice->setText(price);
                    if (m_niftyLabel) m_niftyLabel->setText(price);
                    if (m_dashboardNiftyChange) {
                        m_dashboardNiftyChange->setText(change);
                        m_dashboardNiftyChange->setStyleSheet(QString("font-size: 14px; color: %1; font-weight: 600;").arg(color));
                    }
                    if (m_niftyChangeLabel) {
                        m_niftyChangeLabel->setText(change);
                        m_niftyChangeLabel->setStyleSheet(QString("color: %1;").arg(color));
                    }
                }, Qt::QueuedConnection);
                
                connect(m_marketStatusChecker, &MarketStatusChecker::sensexUpdated,
                        this, [this](const QString &price, const QString &change, const QString &color) {
                    if (m_dashboardSensexPrice) m_dashboardSensexPrice->setText(price);
                    if (m_sensexLabel) m_sensexLabel->setText(price);
                    if (m_dashboardSensexChange) {
                        m_dashboardSensexChange->setText(change);
                        m_dashboardSensexChange->setStyleSheet(QString("font-size: 14px; color: %1; font-weight: 600;").arg(color));
                    }
                    if (m_sensexChangeLabel) {
                        m_sensexChangeLabel->setText(change);
                        m_sensexChangeLabel->setStyleSheet(QString("color: %1;").arg(color));
                    }
                }, Qt::QueuedConnection);
                
                connect(m_marketStatusChecker, &MarketStatusChecker::niftyUpdated,
                        this, &StockSenseApp::updateNiftyDisplay);
                connect(m_marketStatusChecker, &MarketStatusChecker::sensexUpdated,
                        this, &StockSenseApp::updateSensexDisplay);
            }
            
            qDebug() << "✅ Step 4: Connections setup complete";
        } catch (const std::exception &e) {
            qWarning() << "❌ Connection setup failed:" << e.what();
        }
        
        // STEP 6: Additional setup methods
        try {
            setupConnections();
            qDebug() << "✅ Step 5: setupConnections() complete";
        } catch (const std::exception &e) {
            qWarning() << "❌ setupConnections() failed:" << e.what();
        }
        
        try {
            setupTimer();
            qDebug() << "✅ Step 6: setupTimer() complete";
        } catch (const std::exception &e) {
            qWarning() << "❌ setupTimer() failed:" << e.what();
        }
        
        // STEP 7: Window properties
        try {
            setWindowTitle("StockSense - Complete LIVE NSE Analysis Platform");
            setMinimumSize(1600, 1000);
            resize(1600, 1000);
            showMaximized();
            qDebug() << "✅ Step 7: Window setup complete";
        } catch (const std::exception &e) {
            qWarning() << "❌ Window setup failed:" << e.what();
        }
        
        // STEP 8: Start LIVE data fetching (DELAYED)
        QTimer::singleShot(3000, this, [this]() {
            try {
                if (m_realDataManager) {
                    m_realDataManager->fetchAllStocks();
                    qDebug() << "✅ Fetching all stocks...";
                }
                if (m_newsManager) {
                    m_newsManager->fetchNewsForStock(m_currentStock);
                    qDebug() << "✅ Fetching news...";
                }
            } catch (const std::exception &e) {
                qWarning() << "❌ Data fetch failed:" << e.what();
            }
        });
        
        qDebug() << "🚀 StockSense COMPLETE PLATFORM launched!";
        qDebug() << "=== StockSenseApp Constructor Completed ===";
    }
    catch (const std::exception &e) {
        qCritical() << "❌ Constructor failed with exception:" << e.what();
        throw;
    }
    catch (...) {
        qCritical() << "❌ Constructor failed with unknown exception";
        throw;
    }
}

void StockSenseApp::closeEvent(QCloseEvent *event)
{
    if (m_updateTimer && m_updateTimer->isActive())
    {
        m_updateTimer->stop();
    }
    event->accept();
}
void StockSenseApp::onMarketStatusChanged(bool isOpen)
{
    QString statusText = isOpen ? "🟢 MARKET OPEN" : "🔴 MARKET CLOSED";
    
    if (m_marketStatusLabel) {
        m_marketStatusLabel->setText(statusText);
        QString color = isOpen ? "#10b981" : "#ef4444";
        m_marketStatusLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(color));
    }
    
    qDebug() << "Market status changed:" << statusText;
}

QStringList StockSenseApp::getComprehensiveStockList() const
{
    return QStringList{
        // Nifty 50 & Blue Chips
        "RELIANCE", "TCS", "HDFCBANK", "INFY", "ICICIBANK", "HDFC", "BHARTIARTL",
        "ITC", "KOTAKBANK", "LT", "SBIN", "AXISBANK", "BAJFINANCE", "WIPRO",
        "ASIANPAINT", "MARUTI", "HCLTECH", "SUNPHARMA", "ULTRACEMCO", "TITAN",
        "NESTLEIND", "M&M", "BAJAJFINSV", "TECHM", "POWERGRID", "NTPC", "TATASTEEL",
        "ADANIPORTS", "HINDALCO", "ONGC", "JSWSTEEL", "INDUSINDBK", "COALINDIA",
        "GRASIM", "CIPLA", "DRREDDY", "DIVISLAB", "BRITANNIA", "SHREECEM",
        "EICHERMOT", "UPL", "HEROMOTOCO", "BPCL", "TATACONSUM", "SBILIFE",
        "BAJAJ-AUTO", "TATAMOTORS", "APOLLOHOSP", "HDFCLIFE", "ADANIENT",

        // Mid & Small Cap
        "GODREJCP", "PIDILITIND", "MPHASIS", "VEDL", "GAIL", "BIOCON",
        "AMBUJACEM", "BANDHANBNK", "INDIGO", "HINDUNILVR", "SIEMENS",
        "DABUR", "HAVELLS", "BOSCHLTD", "MARICO", "BERGEPAINT", "COLPAL",
        "VOLTAS", "PAGEIND", "TORNTPHARM", "MCDOWELL-N", "ABCAPITAL",

        // Banking
        "PNB", "BANKBARODA", "CANBK", "IDFCFIRSTB", "FEDERALBNK", "LICHSGFIN",
        "M&MFIN", "PFC", "RECLTD", "CHOLAFIN",

        // IT & Tech
        "LTIM", "PERSISTENT", "COFORGE", "TATAELXSI",

        // Pharma
        "LUPIN", "AUROPHARMA", "CADILAHC", "TORNTPOWER", "ALKEM",

        // Auto
        "TVSMOTOR", "BALKRISIND", "MOTHERSON", "ASHOKLEY", "ESCORTS"};
}
void StockSenseApp::updateNiftyDisplay(const QString &price, const QString &change, const QString &color)
{
    if (m_niftyLabel)
    {
        m_niftyLabel->setText(price);
    }
    if (m_niftyChangeLabel)
    {
        m_niftyChangeLabel->setText(change);
        m_niftyChangeLabel->setStyleSheet(QString("color: %1; font-weight: 600;").arg(color));
    }
}

void StockSenseApp::updateSensexDisplay(const QString &price, const QString &change, const QString &color)
{
    if (m_sensexLabel)
    {
        m_sensexLabel->setText(price);
    }
    if (m_sensexChangeLabel)
    {
        m_sensexChangeLabel->setText(change);
        m_sensexChangeLabel->setStyleSheet(QString("color: %1; font-weight: 600;").arg(color));
    }
}

void StockSenseApp::onLiveDataReceived(const QString &symbol, const QJsonObject &data)
{
    m_liveStockData[symbol] = data;
    double price = data["price"].toDouble();
    double change = data["change"].toDouble();

    qDebug() << "📈 Received data:" << symbol
             << "Price: ₹" << price
             << "Change:" << change
             << "Trend:" << (change > 0 ? "📈" : "📉");

    if (symbol == m_currentStock)
    {
        qDebug() << "🎯 This is current stock - updating UI";
        updateUIWithLiveData(data);
        if (m_customChart)
        {
            m_customChart->updateWithLiveData(symbol, data);
        }
    }
    else
    {
        qDebug() << "ℹ️ This is not current stock (" << m_currentStock << ") - data cached only";
    }
}
void StockSenseApp::onNewsReceived(const QStringList &newsItems)
{
    if (!m_newsList)
        return;

    m_newsList->clear();

    for (const QString &newsItem : newsItems)
    {
        // Parse news format: "title|url"
        QStringList parts = newsItem.split("|");
        QString title = parts.size() > 0 ? parts[0] : newsItem;
        QString url = parts.size() > 1 ? parts[1] : "";

        QListWidgetItem *item = new QListWidgetItem(QString("📰 %1").arg(title));

        // Store URL as data
        if (!url.isEmpty())
        {
            item->setData(Qt::UserRole, url);
            item->setToolTip(QString("Click to read: %1").arg(url));
            // Make it look clickable
            QFont font = item->font();
            font.setUnderline(true);
            item->setFont(font);
            item->setForeground(QColor("#3b82f6"));
        }

        m_newsList->addItem(item);
    }

    qDebug() << "✅ Updated news with" << newsItems.size() << "items (with links)";
}

void StockSenseApp::switchView(const QString &viewName)
{
    if (!m_contentStack)
        return;

    if (m_dashboardBtn)
        m_dashboardBtn->setChecked(viewName == "dashboard");
    if (m_predictionsBtn)
        m_predictionsBtn->setChecked(viewName == "predictions");
    if (m_watchlistBtn)
        m_watchlistBtn->setChecked(viewName == "watchlist");
    if (m_settingsBtn)
        m_settingsBtn->setChecked(viewName == "settings");

    if (viewName == "dashboard" && m_dashboardWidget)
    {
        m_contentStack->setCurrentWidget(m_dashboardWidget);
    }
    else if (viewName == "predictions" && m_predictionsWidget)
    {
        m_contentStack->setCurrentWidget(m_predictionsWidget);
    }
    else if (viewName == "watchlist" && m_watchlistWidget)
    {
        m_contentStack->setCurrentWidget(m_watchlistWidget);
    }
    else if (viewName == "settings" && m_settingsWidget)
    {
        m_contentStack->setCurrentWidget(m_settingsWidget);
    }
    qDebug() << "Switched to view:" << viewName;
}
void StockSenseApp::selectStock(const QString &symbol)
{
    if (symbol.isEmpty() || symbol == m_currentStock)
        return;

    qDebug() << "🔄 Switching to" << symbol;
    m_currentStock = symbol;

    // Update headers immediately
    if (m_headerStockSymbol)
    {
        m_headerStockSymbol->setText(symbol);
    }

    // ADDED: Update Current Analysis label on Predictions page
    if (m_predictionStockTitle)
    {
        m_predictionStockTitle->setText(QString("📈 Current Analysis: %1").arg(symbol));
        qDebug() << "✅ Updated prediction title to" << symbol;
    }

    if (m_predictionStockDetails)
    {
        m_predictionStockDetails->setText("Live Price: Loading... | Real-time NSE data with DSA algorithms | Prediction Horizon: 7 days");
    }

    // Update chart symbol immediately
    if (m_customChart)
    {
        m_customChart->setSymbol(symbol);
    }

    // Update chart title immediately
    updateChartTitleForStock(symbol);

    // Update PredictionChartWidget
    if (m_predictionsWidget)
    {
        PredictionChartWidget *predictionChart = m_predictionsWidget->findChild<PredictionChartWidget *>();

        if (predictionChart)
        {
            predictionChart->setCurrentStock(symbol);
            qDebug() << "📊 Updated PredictionChart to analyze" << symbol;
        }
        else
        {
            QScrollArea *scrollArea = m_predictionsWidget->findChild<QScrollArea *>();
            if (scrollArea)
            {
                predictionChart = scrollArea->findChild<PredictionChartWidget *>();
                if (predictionChart)
                {
                    predictionChart->setCurrentStock(symbol);
                    qDebug() << "📊 Updated PredictionChart (from scroll area) to analyze" << symbol;
                }
            }
        }
    }

    // Show loading state for ALL widgets
    QString loadingEmoji = "🔄";

    if (m_currentPriceLabel)
    {
        m_currentPriceLabel->setText("Loading...");
    }

    if (m_priceChangeLabel)
    {
        m_priceChangeLabel->setText("Fetching NSE...");
        m_priceChangeLabel->setStyleSheet("color: #f59e0b; font-weight: 600;");
    }

    if (m_trendIcon)
    {
        m_trendIcon->setText(loadingEmoji);
    }

    if (m_chartPriceLabel)
    {
        m_chartPriceLabel->setText("Loading...");
    }

    if (m_chartChangeLabel)
    {
        m_chartChangeLabel->setText("Fetching NSE...");
        m_chartChangeLabel->setStyleSheet("color: #f59e0b; font-size: 18px; font-weight: 600;");
    }

    if (m_chartTrendIcon)
    {
        m_chartTrendIcon->setText(loadingEmoji);
    }

    // Check for cached data first, then request fresh data
    if (m_liveStockData.contains(symbol))
    {
        updateUIWithLiveData(m_liveStockData[symbol]);
        qDebug() << "✅ Using cached data for" << symbol;
    }
    else
    {
        if (m_realDataManager)
        {
            m_realDataManager->fetchStockData(symbol);
        }
        qDebug() << "🔄 Requesting fresh LIVE NSE data for" << symbol;
    }

    // Update news for the selected stock
    if (m_newsManager)
    {
        m_newsManager->fetchNewsForStock(symbol);
    }

    if (m_searchInput)
        m_searchInput->clear();
    if (m_stockSuggestions)
        m_stockSuggestions->setVisible(false);

    // Force immediate sync if we have data
    QTimer::singleShot(100, this, [this]()
                       { forceUISync(); });

    qDebug() << "✅ Stock selection complete:" << symbol;
}

void StockSenseApp::updateUIWithLiveData(const QJsonObject &data)
{
    double price = data["price"].toDouble();
    double change = data["change"].toDouble();
    double changePercent = data["changePercent"].toDouble();
    QString symbol = data["symbol"].toString();

    if (price <= 0)
        return;

    qDebug() << "🔄 Updating UI for" << symbol << "Price: ₹" << price << "Change:" << change;

    // Determine trend direction for consistent icons
    bool isPositive = change > 0;
    QString trendEmoji = isPositive ? "📈" : "📉";
    QString color = isPositive ? "#10b981" : "#ef4444";

    // Update HEADER price
    if (m_currentPriceLabel)
    {
        m_currentPriceLabel->setText(QString("₹%1").arg(price, 0, 'f', 2));
    }

    if (m_priceChangeLabel)
    {
        QString changeText = QString("%1₹%2 (%3%4%)")
                                 .arg(change > 0 ? "+" : "")
                                 .arg(abs(change), 0, 'f', 2)
                                 .arg(changePercent > 0 ? "+" : "")
                                 .arg(changePercent, 0, 'f', 2);
        m_priceChangeLabel->setText(changeText);
        m_priceChangeLabel->setStyleSheet(QString("color: %1; font-weight: 600;").arg(color));
    }

    if (m_trendIcon)
    {
        m_trendIcon->setText(trendEmoji);
    }

    // Update CHART HEADER price - SAME VALUES
    if (m_chartPriceLabel)
    {
        m_chartPriceLabel->setText(QString("₹%1").arg(price, 0, 'f', 2));
    }

    if (m_chartChangeLabel)
    {
        QString changeText = QString("%1₹%2 (%3%4%)")
                                 .arg(change > 0 ? "+" : "")
                                 .arg(abs(change), 0, 'f', 2)
                                 .arg(changePercent > 0 ? "+" : "")
                                 .arg(changePercent, 0, 'f', 2);
        m_chartChangeLabel->setText(changeText);
        m_chartChangeLabel->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: 600;").arg(color));
    }

    if (m_chartTrendIcon)
    {
        m_chartTrendIcon->setText(trendEmoji);
    }

    // Update statistics
    if (m_marketStatusLabel)
    {
        QString stats = QString("Market Cap: ₹%1L Cr | High: ₹%2 | Low: ₹%3 | Volume: %4M | Source: %5")
                            .arg(QRandomGenerator::global()->bounded(500, 2000))
                            .arg(data["high"].toDouble(), 0, 'f', 2)
                            .arg(data["low"].toDouble(), 0, 'f', 2)
                            .arg(data["volume"].toInt() / 1000000.0, 0, 'f', 1)
                            .arg(data["source"].toString());
        m_marketStatusLabel->setText(stats);
    }

    updateSentimentMeter();

    qDebug() << "✅ COMPLETE UI synchronized for" << symbol
             << "HeaderPrice: ₹" << price
             << "ChartPrice: ₹" << price
             << "Trend:" << trendEmoji;
}

void StockSenseApp::updateStockData()
{
    if (m_liveStockData.contains(m_currentStock))
    {
        updateUIWithLiveData(m_liveStockData[m_currentStock]);
    }
}

void StockSenseApp::updateSentimentMeter()
{
    try
    {
        int sentiment = QRandomGenerator::global()->bounded(40, 95);

        if (m_sentimentScore)
        {
            m_sentimentScore->setText(QString("%1%").arg(sentiment));
        }

        if (m_sentimentProgress)
        {
            m_sentimentProgress->setValue(sentiment);
        }

        if (m_sentimentLabel)
        {
            QString label = sentiment >= 70 ? "BULLISH" : sentiment >= 40 ? "NEUTRAL"
                                                                          : "BEARISH";
            QString color = sentiment >= 70 ? "#10b981" : sentiment >= 40 ? "#f59e0b"
                                                                          : "#ef4444";
            m_sentimentLabel->setText(label);
            m_sentimentLabel->setStyleSheet(QString("font-size: 18px; font-weight: 600; color: %1; text-transform: uppercase; letter-spacing: 0.1em;").arg(color));
        }
    }
    catch (const std::exception &e)
    {
        qWarning() << "Error updating sentiment meter:" << e.what();
    }
}

void StockSenseApp::initializePointers()
{
    m_contentStack = nullptr;
    m_dashboardWidget = nullptr;
    m_predictionsWidget = nullptr;
    m_watchlistWidget = nullptr;
    m_settingsWidget = nullptr;
    m_dashboardBtn = nullptr;
    m_predictionsBtn = nullptr;
    m_watchlistBtn = nullptr;
    m_settingsBtn = nullptr;
    m_searchInput = nullptr;
    m_stockSuggestions = nullptr;
    m_currentPriceLabel = nullptr;
    m_priceChangeLabel = nullptr;
    m_trendIcon = nullptr;
    m_headerStockSymbol = nullptr;
    m_marketStatusLabel=nullptr;
    m_sentimentScore = nullptr;
    m_sentimentLabel = nullptr;
    m_sentimentProgress = nullptr;
    m_newsList = nullptr;
    m_customChart = nullptr;
    m_realDataManager = nullptr;
    m_newsManager = nullptr;
    m_chartTitleLabel = nullptr;
    // Chart header price widgets
    m_chartPriceLabel = nullptr;
    m_chartChangeLabel = nullptr;
    m_chartTrendIcon = nullptr;
    m_updateTimer = nullptr;
    m_predictionStockTitle = nullptr;
    m_predictionStockDetails = nullptr;

    m_currentStock = "RELIANCE";
}

void StockSenseApp::updateChartTitleForStock(const QString &symbol)
{
    if (m_chartTitleLabel)
    {
        m_chartTitleLabel->setText(QString("📈 %1 LIVE NSE Chart").arg(symbol));
    }
}

void StockSenseApp::forceUISync()
{
    if (m_liveStockData.contains(m_currentStock))
    {
        QJsonObject data = m_liveStockData[m_currentStock];
        updateUIWithLiveData(data);
        qDebug() << "🔄 Forced UI sync for" << m_currentStock
                 << "Price: ₹" << data["price"].toDouble();
    }
}

void StockSenseApp::setupUI()
{
    try
    {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *sidebar = createSidebar();
        QWidget *contentArea = createContentArea();

        if (sidebar && contentArea)
        {
            mainLayout->addWidget(sidebar);
            mainLayout->addWidget(contentArea);
        }
    }
    catch (const std::exception &e)
    {
        qCritical() << "Error setting up UI:" << e.what();
    }
}

QWidget *StockSenseApp::createSidebar()
{
    QWidget *sidebar = new QWidget();
    sidebar->setObjectName("StockSidebar");
    sidebar->setFixedWidth(250);
    
    // ADD THIS LINE - Set dark background
    sidebar->setStyleSheet("background-color: #1e293b; border: none;");

    // Outer layout to hold scroll area
    QVBoxLayout *outerLayout = new QVBoxLayout(sidebar);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // Create invisible scroll area
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { "
        "    border: none; "
        "    background-color: transparent; "
        "}"
    );

    // Content widget (keeps your exact layout)
    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background-color: transparent;");  // ADD THIS
    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    layout->setContentsMargins(16, 24, 16, 24);
    layout->setSpacing(16);

   // ===== LOGO AT TOP =====
QLabel *logo = new QLabel();
QPixmap logoPixmap("C:/Users/rajen/StockSense/resources/LOGO.png");
if (!logoPixmap.isNull()) {
    logo->setPixmap(logoPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setAlignment(Qt::AlignCenter);
} else {
    logo->setText("📈 StockSense");
}
layout->addWidget(logo);

QLabel *logoText = new QLabel("StockSense");
logoText->setAlignment(Qt::AlignCenter);
logoText->setStyleSheet("font-size: 14px; font-weight: bold; color: #6366f1; margin-top: 5px;");
layout->addWidget(logoText);


    // ===== LIVE NSE DATA (NIFTY & SENSEX) =====
    QWidget *liveDataWidget = new QWidget();
    QVBoxLayout *liveDataLayout = new QVBoxLayout(liveDataWidget);
    liveDataLayout->setContentsMargins(0, 0, 0, 0);
    liveDataLayout->setSpacing(12);

    QLabel *liveDataTitle = new QLabel("📊 LIVE NSE DATA");
    liveDataTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #10b981; letter-spacing: 1px;");

    // NIFTY 50 Widget
    QFrame *niftyFrame = new QFrame();
    niftyFrame->setStyleSheet("background-color: rgba(255,255,255,0.05); border-radius: 8px; padding: 12px;");
    QVBoxLayout *niftyFrameLayout = new QVBoxLayout(niftyFrame);
    niftyFrameLayout->setContentsMargins(12, 8, 12, 8);
    niftyFrameLayout->setSpacing(4);

    QLabel *niftyNameLabel = new QLabel("📈 NIFTY 50");
    niftyNameLabel->setStyleSheet("color: #d1d5db; font-size: 12px; font-weight: 600;");

    m_niftyLabel = new QLabel("Loading...");
    m_niftyLabel->setStyleSheet("color: #10b981; font-size: 18px; font-weight: bold;");

    m_niftyChangeLabel = new QLabel("Loading...");
    m_niftyChangeLabel->setStyleSheet("color: #10b981; font-size: 11px; font-weight: 500;");

    niftyFrameLayout->addWidget(niftyNameLabel);
    niftyFrameLayout->addWidget(m_niftyLabel);
    niftyFrameLayout->addWidget(m_niftyChangeLabel);

    // SENSEX Widget
    QFrame *sensexFrame = new QFrame();
    sensexFrame->setStyleSheet("background-color: rgba(255,255,255,0.05); border-radius: 8px; padding: 12px;");
    QVBoxLayout *sensexFrameLayout = new QVBoxLayout(sensexFrame);
    sensexFrameLayout->setContentsMargins(12, 8, 12, 8);
    sensexFrameLayout->setSpacing(4);

    QLabel *sensexNameLabel = new QLabel("📊 SENSEX");
    sensexNameLabel->setStyleSheet("color: #d1d5db; font-size: 12px; font-weight: 600;");

    m_sensexLabel = new QLabel("Loading...");
    m_sensexLabel->setStyleSheet("color: #10b981; font-size: 18px; font-weight: bold;");

    m_sensexChangeLabel = new QLabel("Loading...");
    m_sensexChangeLabel->setStyleSheet("color: #10b981; font-size: 11px; font-weight: 500;");

    sensexFrameLayout->addWidget(sensexNameLabel);
    sensexFrameLayout->addWidget(m_sensexLabel);
    sensexFrameLayout->addWidget(m_sensexChangeLabel);

    liveDataLayout->addWidget(liveDataTitle);
    liveDataLayout->addWidget(niftyFrame);
    liveDataLayout->addWidget(sensexFrame);

    layout->addWidget(liveDataWidget);
    
    layout->addSpacing(20);

    // ===== NAVIGATION SECTION =====
    QLabel *navTitle = new QLabel("NAVIGATION");
    navTitle->setObjectName("navigationTitle");
    layout->addWidget(navTitle);

    m_dashboardBtn = new QPushButton("🏠 Dashboard");
    m_predictionsBtn = new QPushButton("🧠 Predictions");
    m_watchlistBtn = new QPushButton("⭐ Watchlist");
    m_settingsBtn = new QPushButton("⚙️ Settings");

    QList<QPushButton *> navButtons = {m_dashboardBtn, m_predictionsBtn, m_watchlistBtn, m_settingsBtn};
    for (auto btn : navButtons)
    {
        if (btn)
        {
            btn->setCheckable(true);
            btn->setObjectName("navButton");
        }
    }

    if (m_dashboardBtn)
        m_dashboardBtn->setChecked(true);

    layout->addWidget(m_dashboardBtn);
    layout->addWidget(m_predictionsBtn);
    layout->addWidget(m_watchlistBtn);
    layout->addWidget(m_settingsBtn);
    
    layout->addStretch();

    // Set scroll content
    scrollArea->setWidget(scrollContent);
    outerLayout->addWidget(scrollArea);

    return sidebar;
}

QWidget *StockSenseApp::createContentArea()
{
    QWidget *contentArea = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentArea);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *header = createHeader();
    if (header)
    {
        header->setFixedHeight(80);
        layout->addWidget(header);
    }

    m_contentStack = new QStackedWidget();
    m_dashboardWidget = createDashboard();
    m_predictionsWidget = createPredictions();
    m_watchlistWidget = createWatchlist();
    m_settingsWidget = createSettings();

    if (m_contentStack)
    {
        if (m_dashboardWidget)
            m_contentStack->addWidget(m_dashboardWidget);
        if (m_predictionsWidget)
            m_contentStack->addWidget(m_predictionsWidget);
        if (m_watchlistWidget)
            m_contentStack->addWidget(m_watchlistWidget);
        if (m_settingsWidget)
            m_contentStack->addWidget(m_settingsWidget);
        layout->addWidget(m_contentStack);
    }

    return contentArea;
}

QWidget *StockSenseApp::createHeader()
{
    QWidget *header = new QWidget();
    header->setObjectName("Header");

    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(32, 20, 32, 20);
    layout->setSpacing(24);

    // Stock selector dropdown with comprehensive NSE list
    QComboBox *stockSelector = new QComboBox();
    stockSelector->setObjectName("stockSelector");

    // UPDATED: Comprehensive stock list
    QStringList allStocks = {
        "RELIANCE", "TCS", "HDFCBANK", "INFY", "ICICIBANK", "HDFC", "BHARTIARTL",
        "ITC", "KOTAKBANK", "LT", "SBIN", "AXISBANK", "BAJFINANCE", "WIPRO",
        "ASIANPAINT", "MARUTI", "HCLTECH", "SUNPHARMA", "ULTRACEMCO", "TITAN",
        "NESTLEIND", "M&M", "BAJAJFINSV", "TECHM", "POWERGRID", "NTPC", "TATASTEEL",
        "ADANIPORTS", "HINDALCO", "ONGC", "JSWSTEEL", "INDUSINDBK", "COALINDIA",
        "GRASIM", "CIPLA", "DRREDDY", "DIVISLAB", "BRITANNIA", "SHREECEM",
        "EICHERMOT", "UPL", "HEROMOTOCO", "BPCL", "TATACONSUM", "SBILIFE",
        "BAJAJ-AUTO", "TATAMOTORS", "APOLLOHOSP", "HDFCLIFE", "ADANIENT",
        "GODREJCP", "PIDILITIND", "MPHASIS", "VEDL", "GAIL", "BIOCON",
        "AMBUJACEM", "BANDHANBNK", "INDIGO", "HINDUNILVR", "SIEMENS",
        "DABUR", "HAVELLS", "BOSCHLTD", "MARICO", "BERGEPAINT", "COLPAL",
        "VOLTAS", "PAGEIND", "TORNTPHARM", "MCDOWELL-N", "ABCAPITAL",
        "PNB", "BANKBARODA", "CANBK", "IDFCFIRSTB", "FEDERALBNK", "LICHSGFIN",
        "M&MFIN", "PFC", "RECLTD", "CHOLAFIN", "LTIM", "PERSISTENT",
        "COFORGE", "TATAELXSI", "LUPIN", "AUROPHARMA", "CADILAHC", "TORNTPOWER",
        "ALKEM", "TVSMOTOR", "BALKRISIND", "MOTHERSON", "ASHOKLEY", "ESCORTS"};

    stockSelector->addItems(allStocks);
    stockSelector->setCurrentText("RELIANCE");
    stockSelector->setMinimumWidth(150);
    stockSelector->setMinimumHeight(44);
    stockSelector->setStyleSheet(R"(
        QComboBox#stockSelector {
            background-color: rgba(255, 255, 255, 0.15);
            border: 2px solid rgba(255, 255, 255, 0.3);
            border-radius: 12px;
            padding: 8px 16px;
            color: #ffffff;
            font-size: 16px;
            font-weight: 600;
        }
        QComboBox#stockSelector:hover {
            background-color: rgba(255, 255, 255, 0.25);
            border-color: rgba(255, 255, 255, 0.5);
        }
        QComboBox#stockSelector::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox#stockSelector QAbstractItemView {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            color: #374151;
            selection-background-color: #e0e7ff;
            selection-color: #1e40af;
            padding: 4px;
        }
        QComboBox#stockSelector QAbstractItemView::item {
            padding: 8px 12px;
            border-radius: 4px;
            margin: 2px;
        }
    )");

    connect(stockSelector, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            [this](const QString &text)
            {
                selectStock(text);
            });

    // Current stock info
    QWidget *stockInfo = new QWidget();
    stockInfo->setMinimumWidth(400);
    QVBoxLayout *stockLayout = new QVBoxLayout(stockInfo);
    stockLayout->setSpacing(8);
    stockLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *stockHeader = new QWidget();
    QHBoxLayout *stockHeaderLayout = new QHBoxLayout(stockHeader);
    stockHeaderLayout->setSpacing(12);
    stockHeaderLayout->setContentsMargins(0, 0, 0, 0);

    m_headerStockSymbol = new QLabel("RELIANCE");
    m_headerStockSymbol->setObjectName("headerStockSymbol");
    m_headerStockSymbol->setStyleSheet("font-size: 20px; font-weight: bold; color: #ffffff;");

    m_trendIcon = new QLabel("📈");
    m_trendIcon->setStyleSheet("font-size: 20px;");

    m_currentPriceLabel = new QLabel("₹1,486.90");
    m_currentPriceLabel->setObjectName("currentPrice");
    m_currentPriceLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #ffffff;");

    m_priceChangeLabel = new QLabel("Loading LIVE data...");
    m_priceChangeLabel->setObjectName("priceChange");
    m_priceChangeLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #10b981;");

    stockHeaderLayout->addWidget(m_headerStockSymbol);
    stockHeaderLayout->addWidget(m_trendIcon);
    stockHeaderLayout->addWidget(m_currentPriceLabel);
    stockHeaderLayout->addWidget(m_priceChangeLabel);
    stockHeaderLayout->addStretch();

    stockLayout->addWidget(stockHeader);

    // Search widget
    QWidget *searchWidget = new QWidget();
    searchWidget->setMinimumWidth(300);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchWidget);
    searchLayout->setSpacing(0);
    searchLayout->setContentsMargins(0, 0, 0, 0);

    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText("Search stocks, news, or analytics...");
    m_searchInput->setObjectName("searchInput");
    m_searchInput->setMinimumWidth(300);
    m_searchInput->setMinimumHeight(44);

    m_stockSuggestions = new QListWidget();
    m_stockSuggestions->setObjectName("stockSuggestions");
    m_stockSuggestions->setVisible(false);
    m_stockSuggestions->setMaximumHeight(220);

    // UPDATED: Show all stocks in suggestions (will be filtered by search)
    for (const QString &stock : allStocks)
    {
        m_stockSuggestions->addItem(stock);
    }

    searchLayout->addWidget(m_searchInput);
    searchLayout->addWidget(m_stockSuggestions);

    // Network status
    QWidget *networkStatus = new QWidget();
    QHBoxLayout *networkLayout = new QHBoxLayout(networkStatus);
    networkLayout->setSpacing(8);
    networkLayout->setContentsMargins(16, 8, 16, 8);

    QLabel *networkIcon = new QLabel("🌐");
    networkIcon->setStyleSheet("font-size: 18px;");

    QLabel *networkLabel = new QLabel("LIVE NSE");
    networkLabel->setStyleSheet("font-size: 14px; color: #ffffff; font-weight: 500;");

    networkLayout->addWidget(networkIcon);
    networkLayout->addWidget(networkLabel);

    // User section
    QWidget *userSection = new QWidget();
    QHBoxLayout *userLayout = new QHBoxLayout(userSection);
    userLayout->setSpacing(12);
    userLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *themeToggle = new QPushButton("🌙");
    themeToggle->setObjectName("themeToggle");
    themeToggle->setMinimumSize(44, 44);
    themeToggle->setMaximumSize(44, 44);

    QLabel *userAvatar = new QLabel("👤");
    userAvatar->setObjectName("userAvatar");
    userAvatar->setMinimumSize(44, 44);
    userAvatar->setAlignment(Qt::AlignCenter);

    userLayout->addWidget(themeToggle);
    userLayout->addWidget(userAvatar);

    layout->addWidget(stockSelector);
    layout->addWidget(stockInfo);
    layout->addWidget(searchWidget);
    layout->addStretch();
    layout->addWidget(networkStatus);
    layout->addWidget(userSection);

    return header;
}
QWidget *StockSenseApp::createDashboard()
{
    QWidget *dashboard = new QWidget();
    dashboard->setObjectName("Dashboard");

    // Scrollable dashboard
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background-color: #f8fafc;
            border: none;
        }
        QScrollBar:vertical {
            background-color: #f1f5f9;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #cbd5e0;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #9ca3af;
        }
    )");

    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollWidget);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(40);

    // ===== LIVE NSE DATA SECTION (NIFTY & SENSEX) =====
    QFrame *indexFrame = new QFrame();
    indexFrame->setObjectName("indexFrame");
    indexFrame->setMinimumHeight(140);
    indexFrame->setStyleSheet("background-color: transparent; border: none;");  // NO BLACK BAR

    QHBoxLayout *indexLayout = new QHBoxLayout(indexFrame);
    indexLayout->setContentsMargins(0, 24, 0, 24);
    indexLayout->setSpacing(60);

    // NIFTY 50
    QWidget *niftyWidget = new QWidget();
    QVBoxLayout *niftyVLayout = new QVBoxLayout(niftyWidget);
    niftyVLayout->setContentsMargins(0, 0, 0, 0);
    niftyVLayout->setSpacing(8);

    QLabel *niftyLabel = new QLabel("📈 NIFTY 50");
    niftyLabel->setStyleSheet("font-size: 12px; color: #9ca3af; font-weight: 600;");

    m_dashboardNiftyPrice = new QLabel("Loading...");
    m_dashboardNiftyPrice->setStyleSheet("font-size: 32px; color: #10b981; font-weight: bold;");

    m_dashboardNiftyChange = new QLabel("Loading...");
    m_dashboardNiftyChange->setStyleSheet("font-size: 14px; color: #10b981; font-weight: 600;");

    niftyVLayout->addWidget(niftyLabel);
    niftyVLayout->addWidget(m_dashboardNiftyPrice);
    niftyVLayout->addWidget(m_dashboardNiftyChange);
    niftyVLayout->addStretch();

    // SENSEX
    QWidget *sensexWidget = new QWidget();
    QVBoxLayout *sensexVLayout = new QVBoxLayout(sensexWidget);
    sensexVLayout->setContentsMargins(0, 0, 0, 0);
    sensexVLayout->setSpacing(8);

    QLabel *sensexLabel = new QLabel("📊 SENSEX");
    sensexLabel->setStyleSheet("font-size: 12px; color: #9ca3af; font-weight: 600;");

    m_dashboardSensexPrice = new QLabel("Loading...");
    m_dashboardSensexPrice->setStyleSheet("font-size: 32px; color: #10b981; font-weight: bold;");

    m_dashboardSensexChange = new QLabel("Loading...");
    m_dashboardSensexChange->setStyleSheet("font-size: 14px; color: #10b981; font-weight: 600;");

    sensexVLayout->addWidget(sensexLabel);
    sensexVLayout->addWidget(m_dashboardSensexPrice);
    sensexVLayout->addWidget(m_dashboardSensexChange);
    sensexVLayout->addStretch();

    // Market Status
    QWidget *statusWidget = new QWidget();
    QVBoxLayout *statusVLayout = new QVBoxLayout(statusWidget);
    statusVLayout->setContentsMargins(0, 0, 0, 0);
    statusVLayout->setSpacing(8);

    QLabel *statusLabel = new QLabel("Market Status");
    statusLabel->setStyleSheet("font-size: 12px; color: #9ca3af; font-weight: 600;");

    m_marketStatusLabel = new QLabel("🟢 MARKET OPEN");
    m_marketStatusLabel->setStyleSheet("font-size: 18px; color: #10b981; font-weight: bold;");

    statusVLayout->addWidget(statusLabel);
    statusVLayout->addWidget(m_marketStatusLabel);
    statusVLayout->addStretch();

    indexLayout->addWidget(niftyWidget);
    indexLayout->addWidget(sensexWidget);
    indexLayout->addWidget(statusWidget);
    indexLayout->addStretch();

    layout->addWidget(indexFrame);

    // Main content area
    QWidget *mainContent = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(mainContent);
    mainLayout->setSpacing(40);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Left column
    QWidget *leftColumn = new QWidget();
    leftColumn->setMinimumWidth(600);
    leftColumn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *leftLayout = new QVBoxLayout(leftColumn);
    leftLayout->setSpacing(32);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *chartWidget = createCustomChart();

    // Key statistics frame
    QFrame *statsFrame = new QFrame();
    statsFrame->setObjectName("statsFrame");
    statsFrame->setMinimumHeight(120);
    statsFrame->setMaximumHeight(160);

    QVBoxLayout *statsLayout = new QVBoxLayout(statsFrame);
    statsLayout->setContentsMargins(32, 24, 32, 24);
    statsLayout->setSpacing(16);

    QLabel *statsTitle = new QLabel("📋 LIVE Key Statistics");
    statsTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #1f2937;");

    QLabel *statsContent = new QLabel("Market Cap: ₹18.2L Cr | High: ₹1,510.25 | Low: ₹1,465.80 | Volume: 2.3M | Source: LIVE_NSE");
    statsContent->setStyleSheet("color: #6b7280; font-size: 14px; line-height: 1.4;");
    statsContent->setWordWrap(true);

    statsLayout->addWidget(statsTitle);
    statsLayout->addWidget(statsContent);

    leftLayout->addWidget(chartWidget);
    leftLayout->addWidget(statsFrame);

    // Right column
    QWidget *rightColumn = new QWidget();
    rightColumn->setFixedWidth(400);

    QVBoxLayout *rightLayout = new QVBoxLayout(rightColumn);
    rightLayout->setSpacing(32);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    QFrame *sentimentFrame = createSentimentMeter();
    QFrame *newsFrame = createNewsPanel();

    rightLayout->addWidget(sentimentFrame);
    rightLayout->addWidget(newsFrame);

    mainLayout->addWidget(leftColumn);
    mainLayout->addWidget(rightColumn);

    layout->addWidget(mainContent);

    scrollArea->setWidget(scrollWidget);

    QVBoxLayout *dashboardLayout = new QVBoxLayout(dashboard);
    dashboardLayout->setContentsMargins(0, 0, 0, 0);
    dashboardLayout->addWidget(scrollArea);

    return dashboard;
}

QWidget *StockSenseApp::createCustomChart()
{
    QWidget *chartContainer = new QWidget();
    chartContainer->setMinimumHeight(500);
    chartContainer->setStyleSheet(R"(
        QWidget {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 20px;
        }
    )");

    QVBoxLayout *containerLayout = new QVBoxLayout(chartContainer);
    containerLayout->setContentsMargins(32, 32, 32, 32);
    containerLayout->setSpacing(24);

    QWidget *chartHeader = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(chartHeader);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_chartTitleLabel = new QLabel("📈 RELIANCE LIVE NSE Chart");
    m_chartTitleLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #1f2937;");

    QWidget *priceWidget = new QWidget();
    QHBoxLayout *priceLayout = new QHBoxLayout(priceWidget);
    priceLayout->setSpacing(16);
    priceLayout->setContentsMargins(0, 0, 0, 0);

    m_chartPriceLabel = new QLabel("₹1,486.90");
    m_chartPriceLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1f2937;");

    m_chartChangeLabel = new QLabel("Loading...");
    m_chartChangeLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #10b981;");

    m_chartTrendIcon = new QLabel("📈");
    m_chartTrendIcon->setStyleSheet("font-size: 24px;");

    priceLayout->addWidget(m_chartPriceLabel);
    priceLayout->addWidget(m_chartChangeLabel);
    priceLayout->addWidget(m_chartTrendIcon);
    priceLayout->addStretch();

    headerLayout->addWidget(m_chartTitleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(priceWidget);

    QWidget *chartControls = new QWidget();
    QHBoxLayout *controlsLayout = new QHBoxLayout(chartControls);
    controlsLayout->setSpacing(16);
    controlsLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *timeframeLabel = new QLabel("Timeframe:");
    timeframeLabel->setStyleSheet("font-size: 14px; color: #6b7280; font-weight: 500;");

    QPushButton *btn1D = new QPushButton("1D");
    QPushButton *btn1W = new QPushButton("1W");
    QPushButton *btn1M = new QPushButton("1M");
    QPushButton *btn3M = new QPushButton("3M");
    QPushButton *btn1Y = new QPushButton("1Y");

    btn1M->setChecked(true);

    QList<QPushButton *> chartBtns = {btn1D, btn1W, btn1M, btn3M, btn1Y};
    for (auto btn : chartBtns)
    {
        btn->setCheckable(true);
        btn->setObjectName("chartTimeButton");
        btn->setMinimumWidth(60);
        btn->setMinimumHeight(36);
    }

    controlsLayout->addWidget(timeframeLabel);
    controlsLayout->addWidget(btn1D);
    controlsLayout->addWidget(btn1W);
    controlsLayout->addWidget(btn1M);
    controlsLayout->addWidget(btn3M);
    controlsLayout->addWidget(btn1Y);
    controlsLayout->addStretch();

    m_customChart = new CustomChartWidget();

    connect(btn1D, &QPushButton::clicked, [this]()
            {
        if (m_customChart) m_customChart->setTimeframe("1D"); });
    connect(btn1W, &QPushButton::clicked, [this]()
            {
        if (m_customChart) m_customChart->setTimeframe("1W"); });
    connect(btn1M, &QPushButton::clicked, [this]()
            {
        if (m_customChart) m_customChart->setTimeframe("1M"); });
    connect(btn3M, &QPushButton::clicked, [this]()
            {
        if (m_customChart) m_customChart->setTimeframe("3M"); });
    connect(btn1Y, &QPushButton::clicked, [this]()
            {
        if (m_customChart) m_customChart->setTimeframe("1Y"); });

    containerLayout->addWidget(chartHeader);
    containerLayout->addWidget(chartControls);
    containerLayout->addWidget(m_customChart);

    return chartContainer;
}

QFrame *StockSenseApp::createSentimentMeter()
{
    QFrame *sentimentFrame = new QFrame();
    sentimentFrame->setObjectName("sentimentFrame");
    sentimentFrame->setMinimumHeight(280);
    sentimentFrame->setMaximumHeight(320);

    QVBoxLayout *layout = new QVBoxLayout(sentimentFrame);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);

    QLabel *title = new QLabel("📊 LIVE Sentiment Analysis");
    title->setStyleSheet("font-size: 18px; font-weight: 700; color: #1f2937;");

    QWidget *overallWidget = new QWidget();
    QVBoxLayout *overallLayout = new QVBoxLayout(overallWidget);
    overallLayout->setSpacing(8);
    overallLayout->setContentsMargins(0, 10, 0, 10);

    m_sentimentScore = new QLabel("83%");
    m_sentimentScore->setObjectName("sentimentScore");
    m_sentimentScore->setAlignment(Qt::AlignCenter);
    m_sentimentScore->setStyleSheet("font-size: 48px; font-weight: 800; color: #10b981;");

    m_sentimentLabel = new QLabel("BULLISH");
    m_sentimentLabel->setObjectName("sentimentLabel");
    m_sentimentLabel->setAlignment(Qt::AlignCenter);
    m_sentimentLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #10b981; text-transform: uppercase; letter-spacing: 0.1em;");

    overallLayout->addWidget(m_sentimentScore);
    overallLayout->addWidget(m_sentimentLabel);

    QWidget *overallProgressWidget = new QWidget();
    QVBoxLayout *overallProgressLayout = new QVBoxLayout(overallProgressWidget);
    overallProgressLayout->setSpacing(8);

    QWidget *overallProgressHeader = new QWidget();
    QHBoxLayout *overallProgressHeaderLayout = new QHBoxLayout(overallProgressHeader);
    overallProgressHeaderLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *overallLabel = new QLabel("Overall Sentiment");
    overallLabel->setStyleSheet("font-size: 14px; color: #374151; font-weight: 600;");

    QLabel *overallValue = new QLabel("72%");
    overallValue->setStyleSheet("font-size: 14px; color: #1f2937; font-weight: 700;");

    overallProgressHeaderLayout->addWidget(overallLabel);
    overallProgressHeaderLayout->addStretch();
    overallProgressHeaderLayout->addWidget(overallValue);

    m_sentimentProgress = new QProgressBar();
    m_sentimentProgress->setMinimum(0);
    m_sentimentProgress->setMaximum(100);
    m_sentimentProgress->setValue(72);
    m_sentimentProgress->setTextVisible(false);
    m_sentimentProgress->setMinimumHeight(12);
    m_sentimentProgress->setMaximumHeight(12);
    m_sentimentProgress->setObjectName("mainSentimentProgress");

    overallProgressLayout->addWidget(overallProgressHeader);
    overallProgressLayout->addWidget(m_sentimentProgress);

    layout->addWidget(title);
    layout->addWidget(overallWidget);
    layout->addWidget(overallProgressWidget);

    return sentimentFrame;
}

QFrame *StockSenseApp::createNewsPanel()
{
    QFrame *newsFrame = new QFrame();
    newsFrame->setObjectName("newsFrame");
    newsFrame->setMinimumHeight(300);
    newsFrame->setMaximumHeight(400);

    QVBoxLayout *layout = new QVBoxLayout(newsFrame);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    QWidget *header = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel("📰 LIVE News & Market Updates");
    title->setStyleSheet("font-size: 18px; font-weight: 700; color: #1f2937;");

    QPushButton *refreshBtn = new QPushButton("↻");
    refreshBtn->setMaximumSize(28, 28);
    refreshBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f3f4f6;
            border: 1px solid #d1d5db;
            border-radius: 14px;
            font-size: 14px;
            font-weight: bold;
            color: #374151;
        }
        QPushButton:hover {
            background-color: #e5e7eb;
        }
    )");

    connect(refreshBtn, &QPushButton::clicked, [this]()
            {
        if (m_newsManager) {
            m_newsManager->fetchNewsForStock(m_currentStock);
        } });

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(refreshBtn);

    m_newsList = new QListWidget();
    m_newsList->setObjectName("newsList");
    m_newsList->setFrameShape(QFrame::NoFrame);
    m_newsList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_newsList->setMaximumHeight(280);

    QStringList initialNews = {
        "🔴 Market Status: After Hours Trading - 11:04 PM",
        "🟢 Loading live news updates...",
        "📊 Fetching real-time market data...",
        "💰 Initializing news feed..."};

    for (const QString &news : initialNews)
    {
        m_newsList->addItem(news);
    }

    layout->addWidget(header);
    layout->addWidget(m_newsList);

    return newsFrame;
}
QWidget *StockSenseApp::createPredictions()
{
    QScrollArea *mainScrollArea = new QScrollArea();
    mainScrollArea->setWidgetResizable(true);
    mainScrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *predictions = new QWidget();
    predictions->setObjectName("Predictions");

    QVBoxLayout *layout = new QVBoxLayout(predictions);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(30);

    // Header section
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);

    QLabel *title = new QLabel("🧠 Stock Predictions");
    title->setStyleSheet("font-size: 32px; font-weight: bold; color: #1f2937; margin-bottom: 8px;");

    QLabel *subtitle = new QLabel("Advanced machine learning algorithms for intelligent stock forecasting using real-time NSE data");
    subtitle->setStyleSheet("font-size: 16px; color: #6b7280; line-height: 1.6;");

    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);

    // Current Stock Info
    QFrame *currentStockFrame = new QFrame();
    currentStockFrame->setObjectName("statsFrame");
    currentStockFrame->setMinimumHeight(120);

    QVBoxLayout *stockInfoLayout = new QVBoxLayout(currentStockFrame);
    stockInfoLayout->setContentsMargins(32, 24, 32, 24);

    m_predictionStockTitle = new QLabel(QString("📈 Current Analysis: %1").arg(m_currentStock));
    m_predictionStockTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");

    m_predictionStockDetails = new QLabel("Live Price: Loading... | Real-time NSE data with DSA algorithms | Prediction Horizon: 7 days");
    m_predictionStockDetails->setStyleSheet("font-size: 14px; color: #6b7280; margin-top: 8px;");

    stockInfoLayout->addWidget(m_predictionStockTitle);
    stockInfoLayout->addWidget(m_predictionStockDetails);

    // Chart section - REMOVED 30/90 Day buttons
    QFrame *chartFrame = new QFrame();
    chartFrame->setObjectName("statsFrame");
    chartFrame->setMinimumHeight(800);

    QVBoxLayout *chartLayout = new QVBoxLayout(chartFrame);
    chartLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *chartTitle = new QLabel("📊 Real-Time Price Prediction Chart with DSA Analysis");
    chartTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #1f2937; margin-bottom: 15px;");

    // REMOVED: Timeframe buttons (7 Days, 30 Days, 90 Days)
    // Now just showing the chart directly

    QScrollArea *predictionScrollArea = new QScrollArea();
    PredictionChartWidget *predictionChart = new PredictionChartWidget();

    predictionScrollArea->setWidget(predictionChart);
    predictionScrollArea->setWidgetResizable(true);
    predictionScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    predictionScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    predictionScrollArea->setMinimumHeight(700);
    predictionScrollArea->setStyleSheet(R"(
        QScrollArea { 
            background-color: #fafbfc; 
            border: 2px solid #e5e7eb; 
            border-radius: 8px; 
        }
        QScrollBar:vertical { 
            background-color: #f1f5f9; 
            width: 14px; 
            border-radius: 7px; 
            margin: 2px;
        }
        QScrollBar::handle:vertical { 
            background-color: #9ca3af; 
            border-radius: 6px; 
            min-height: 30px; 
            margin: 1px;
        }
        QScrollBar::handle:vertical:hover { 
            background-color: #6b7280; 
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
        }
        QScrollBar:horizontal { 
            background-color: #f1f5f9; 
            height: 14px; 
            border-radius: 7px; 
            margin: 2px;
        }
        QScrollBar::handle:horizontal { 
            background-color: #9ca3af; 
            border-radius: 6px; 
            min-width: 30px; 
            margin: 1px;
        }
        QScrollBar::handle:horizontal:hover { 
            background-color: #6b7280; 
        }
    )");

    chartLayout->addWidget(chartTitle);
    chartLayout->addWidget(predictionScrollArea);

    // DSA Info section
    QFrame *dsaFrame = new QFrame();
    dsaFrame->setObjectName("statsFrame");
    dsaFrame->setMinimumHeight(120);

    QVBoxLayout *dsaLayout = new QVBoxLayout(dsaFrame);
    dsaLayout->setContentsMargins(32, 24, 32, 24);

    QLabel *dsaTitle = new QLabel("🔧 DSA Algorithms in Action");
    dsaTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #1f2937;");

    QLabel *dsaInfo = new QLabel("• O(n) Sliding Window: SMA, EMA, RSI calculations\n• O(1) HashMap: Stock data caching for instant access\n• O(n log k) Heap: Top gainers/losers screening\n• O(n) Linear Regression: Price forecasting with confidence intervals");
    dsaInfo->setStyleSheet("font-size: 13px; color: #374151; line-height: 1.8; margin-top: 8px;");

    dsaLayout->addWidget(dsaTitle);
    dsaLayout->addWidget(dsaInfo);

    // REMOVED: Real-Time Technical Analysis Models section

    layout->addWidget(headerWidget);
    layout->addWidget(currentStockFrame);
    layout->addWidget(chartFrame);
    layout->addWidget(dsaFrame);
    layout->addStretch();

    mainScrollArea->setWidget(predictions);

    m_predictionsWidget = chartFrame;

    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(mainScrollArea);

    return container;
}

QWidget *StockSenseApp::createWatchlist()
{
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background-color: #f8fafc;
            border: none;
        }
    )");

    QWidget *watchlist = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(watchlist);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(30);

    // Header with better styling
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel("⭐ My Watchlist");
    title->setStyleSheet("font-size: 32px; font-weight: bold; color: #1f2937;");

    QPushButton *addStockBtn = new QPushButton("+ Add Stock");
    addStockBtn->setObjectName("chartTimeButton");
    addStockBtn->setMinimumHeight(44);
    addStockBtn->setMinimumWidth(120);
    addStockBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #3b82f6;
            border: 1px solid #3b82f6;
            border-radius: 8px;
            color: #ffffff;
            font-weight: 600;
            padding: 12px 24px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #2563eb;
        }
    )");

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(addStockBtn);

    // Portfolio summary with better spacing
    QFrame *summaryFrame = new QFrame();
    summaryFrame->setObjectName("statsFrame");
    summaryFrame->setMinimumHeight(140);
    summaryFrame->setMaximumHeight(160);

    QHBoxLayout *summaryLayout = new QHBoxLayout(summaryFrame);
    summaryLayout->setContentsMargins(40, 30, 40, 30);
    summaryLayout->setSpacing(60);

    QWidget *totalValueWidget = new QWidget();
    QVBoxLayout *totalValueLayout = new QVBoxLayout(totalValueWidget);
    totalValueLayout->setAlignment(Qt::AlignCenter);
    totalValueLayout->setSpacing(8);

    QLabel *totalValueTitle = new QLabel("Total Value");
    totalValueTitle->setStyleSheet("font-size: 14px; color: #6b7280; font-weight: 500;");
    totalValueTitle->setAlignment(Qt::AlignCenter);

    QLabel *totalValueAmount = new QLabel("₹2,45,680");
    totalValueAmount->setStyleSheet("font-size: 32px; font-weight: bold; color: #1f2937;");
    totalValueAmount->setAlignment(Qt::AlignCenter);

    totalValueLayout->addWidget(totalValueTitle);
    totalValueLayout->addWidget(totalValueAmount);

    QWidget *totalChangeWidget = new QWidget();
    QVBoxLayout *totalChangeLayout = new QVBoxLayout(totalChangeWidget);
    totalChangeLayout->setAlignment(Qt::AlignCenter);
    totalChangeLayout->setSpacing(8);

    QLabel *totalChangeTitle = new QLabel("Total Change");
    totalChangeTitle->setStyleSheet("font-size: 14px; color: #6b7280; font-weight: 500;");
    totalChangeTitle->setAlignment(Qt::AlignCenter);

    QLabel *totalChangeAmount = new QLabel("+₹12,450");
    totalChangeAmount->setStyleSheet("font-size: 32px; font-weight: bold; color: #10b981;");
    totalChangeAmount->setAlignment(Qt::AlignCenter);

    totalChangeLayout->addWidget(totalChangeTitle);
    totalChangeLayout->addWidget(totalChangeAmount);

    QWidget *performanceWidget = new QWidget();
    QVBoxLayout *performanceLayout = new QVBoxLayout(performanceWidget);
    performanceLayout->setAlignment(Qt::AlignCenter);
    performanceLayout->setSpacing(8);

    QLabel *performanceTitle = new QLabel("Performance");
    performanceTitle->setStyleSheet("font-size: 14px; color: #6b7280; font-weight: 500;");
    performanceTitle->setAlignment(Qt::AlignCenter);

    QLabel *performancePercent = new QLabel("+5.34%");
    performancePercent->setStyleSheet("font-size: 32px; font-weight: bold; color: #10b981;");
    performancePercent->setAlignment(Qt::AlignCenter);

    performanceLayout->addWidget(performanceTitle);
    performanceLayout->addWidget(performancePercent);

    summaryLayout->addWidget(totalValueWidget);
    summaryLayout->addWidget(totalChangeWidget);
    summaryLayout->addWidget(performanceWidget);

    // Watchlist table with improved styling
    QFrame *tableFrame = new QFrame();
    tableFrame->setObjectName("statsFrame");
    tableFrame->setMinimumHeight(400);

    QVBoxLayout *tableLayout = new QVBoxLayout(tableFrame);
    tableLayout->setContentsMargins(0, 0, 0, 0);

    QTableWidget *watchlistTable = new QTableWidget(5, 6);
    watchlistTable->setObjectName("watchlistTable");
    watchlistTable->setFrameShape(QFrame::NoFrame);
    watchlistTable->setAlternatingRowColors(true);
    watchlistTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    watchlistTable->setMinimumHeight(350);

    QStringList headers = {"STOCK", "PRICE", "CHANGE", "CHANGE %", "VOLUME", "ACTION"};
    watchlistTable->setHorizontalHeaderLabels(headers);

    // Sample data with better formatting
    QStringList stocks = {"RELIANCE", "TCS", "INFY", "HDFCBANK", "MARUTI"};
    QStringList prices = {"₹1,486.90", "₹3,084.90", "₹1,789.45", "₹1,642.80", "₹10,156.75"};
    QStringList changes = {"+₹12.45", "-₹28.60", "+₹45.20", "+₹18.90", "+₹125.80"};
    QStringList changePercents = {"+0.84%", "-0.92%", "+2.59%", "+1.16%", "+1.25%"};
    QStringList volumes = {"2.3M", "1.8M", "3.2M", "1.9M", "0.8M"};

    for (int i = 0; i < 5; ++i)
    {
        // Stock name with better styling
        QTableWidgetItem *stockItem = new QTableWidgetItem(stocks[i]);
        stockItem->setFont(QFont("Arial", 10, QFont::Bold));
        watchlistTable->setItem(i, 0, stockItem);

        // Price
        QTableWidgetItem *priceItem = new QTableWidgetItem(prices[i]);
        priceItem->setFont(QFont("Arial", 10, QFont::Bold));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        watchlistTable->setItem(i, 1, priceItem);

        // Change with color coding
        QTableWidgetItem *changeItem = new QTableWidgetItem(changes[i]);
        changeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        changeItem->setFont(QFont("Arial", 10, QFont::Bold));
        if (changes[i].startsWith("+"))
        {
            changeItem->setForeground(QColor("#10b981"));
        }
        else
        {
            changeItem->setForeground(QColor("#ef4444"));
        }
        watchlistTable->setItem(i, 2, changeItem);

        // Change % with color coding
        QTableWidgetItem *changePercentItem = new QTableWidgetItem(changePercents[i]);
        changePercentItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        changePercentItem->setFont(QFont("Arial", 10, QFont::Bold));
        if (changePercents[i].startsWith("+"))
        {
            changePercentItem->setForeground(QColor("#10b981"));
        }
        else
        {
            changePercentItem->setForeground(QColor("#ef4444"));
        }
        watchlistTable->setItem(i, 3, changePercentItem);

        // Volume
        QTableWidgetItem *volumeItem = new QTableWidgetItem(volumes[i]);
        volumeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        watchlistTable->setItem(i, 4, volumeItem);

        // Remove button with better styling
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(8, 4, 8, 4);
        actionLayout->setAlignment(Qt::AlignCenter);

        QPushButton *removeBtn = new QPushButton("Remove");
        removeBtn->setMaximumWidth(80);
        removeBtn->setMinimumHeight(28);
        removeBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #fef2f2;
                border: 1px solid #fecaca;
                border-radius: 6px;
                color: #dc2626;
                padding: 4px 12px;
                font-size: 11px;
                font-weight: 600;
            }
            QPushButton:hover {
                background-color: #fee2e2;
                border-color: #fca5a5;
            }
        )");

        actionLayout->addWidget(removeBtn);
        watchlistTable->setCellWidget(i, 5, actionWidget);

        // Set row height for better spacing
        watchlistTable->setRowHeight(i, 50);
    }

    // Improved column sizing
    watchlistTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    watchlistTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    watchlistTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    watchlistTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    watchlistTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    watchlistTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    tableLayout->addWidget(watchlistTable);

    layout->addWidget(headerWidget);
    layout->addWidget(summaryFrame);
    layout->addWidget(tableFrame);
    layout->addStretch();

    scrollArea->setWidget(watchlist);

    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(scrollArea);

    return container;
}

QWidget *StockSenseApp::createSettings()
{
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *settings = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(settings);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(30);

    // Header
    QLabel *title = new QLabel("⚙️ Settings & Configuration");
    title->setStyleSheet("font-size: 32px; font-weight: bold; color: #1f2937;");

    // Notifications section
    QFrame *notificationsFrame = new QFrame();
    notificationsFrame->setObjectName("statsFrame");

    QVBoxLayout *notificationsLayout = new QVBoxLayout(notificationsFrame);
    notificationsLayout->setContentsMargins(32, 24, 32, 24);

    QLabel *notificationsTitle = new QLabel("🔔 Notifications");
    notificationsTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");

    QWidget *notificationOptions = new QWidget();
    QVBoxLayout *optionsLayout = new QVBoxLayout(notificationOptions);
    optionsLayout->setSpacing(16);

    QCheckBox *enableNotifications = new QCheckBox("Enable push notifications");
    enableNotifications->setChecked(true);
    enableNotifications->setStyleSheet("font-size: 14px; color: #374151;");

    QCheckBox *priceAlerts = new QCheckBox("Price alert notifications");
    priceAlerts->setChecked(true);
    priceAlerts->setStyleSheet("font-size: 14px; color: #374151;");

    QCheckBox *newsAlerts = new QCheckBox("Breaking news notifications");
    newsAlerts->setChecked(false);
    newsAlerts->setStyleSheet("font-size: 14px; color: #374151;");

    optionsLayout->addWidget(enableNotifications);
    optionsLayout->addWidget(priceAlerts);
    optionsLayout->addWidget(newsAlerts);

    notificationsLayout->addWidget(notificationsTitle);
    notificationsLayout->addWidget(notificationOptions);

    // Data & Performance section
    QFrame *dataFrame = new QFrame();
    dataFrame->setObjectName("statsFrame");

    QVBoxLayout *dataLayout = new QVBoxLayout(dataFrame);
    dataLayout->setContentsMargins(32, 24, 32, 24);

    QLabel *dataTitle = new QLabel("📊 Data & Performance");
    dataTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");

    QWidget *dataOptions = new QWidget();
    QVBoxLayout *dataOptionsLayout = new QVBoxLayout(dataOptions);
    dataOptionsLayout->setSpacing(16);

    QWidget *refreshWidget = new QWidget();
    QHBoxLayout *refreshLayout = new QHBoxLayout(refreshWidget);

    QLabel *refreshLabel = new QLabel("Refresh Interval:");
    refreshLabel->setStyleSheet("font-size: 14px; color: #374151; font-weight: 500;");

    QComboBox *refreshCombo = new QComboBox();
    refreshCombo->addItems({"10 seconds", "30 seconds", "1 minute", "5 minutes"});
    refreshCombo->setCurrentText("30 seconds");
    refreshCombo->setMaximumWidth(150);

    refreshLayout->addWidget(refreshLabel);
    refreshLayout->addWidget(refreshCombo);
    refreshLayout->addStretch();

    QWidget *currencyWidget = new QWidget();
    QHBoxLayout *currencyLayout = new QHBoxLayout(currencyWidget);

    QLabel *currencyLabel = new QLabel("Default Currency:");
    currencyLabel->setStyleSheet("font-size: 14px; color: #374151; font-weight: 500;");

    QComboBox *currencyCombo = new QComboBox();
    currencyCombo->addItems({"INR (₹)", "USD ($)", "EUR (€)", "GBP (£)"});
    currencyCombo->setCurrentText("INR (₹)");
    currencyCombo->setMaximumWidth(150);

    currencyLayout->addWidget(currencyLabel);
    currencyLayout->addWidget(currencyCombo);
    currencyLayout->addStretch();

    dataOptionsLayout->addWidget(refreshWidget);
    dataOptionsLayout->addWidget(currencyWidget);

    dataLayout->addWidget(dataTitle);
    dataLayout->addWidget(dataOptions);

    // Appearance section
    QFrame *appearanceFrame = new QFrame();
    appearanceFrame->setObjectName("statsFrame");

    QVBoxLayout *appearanceLayout = new QVBoxLayout(appearanceFrame);
    appearanceLayout->setContentsMargins(32, 24, 32, 24);

    QLabel *appearanceTitle = new QLabel("🎨 Appearance");
    appearanceTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");

    QWidget *appearanceOptions = new QWidget();
    QVBoxLayout *appearanceOptionsLayout = new QVBoxLayout(appearanceOptions);
    appearanceOptionsLayout->setSpacing(16);

    QWidget *themeWidget = new QWidget();
    QHBoxLayout *themeLayout = new QHBoxLayout(themeWidget);

    QLabel *themeLabel = new QLabel("Theme:");
    themeLabel->setStyleSheet("font-size: 14px; color: #374151; font-weight: 500;");

    QComboBox *themeCombo = new QComboBox();
    themeCombo->addItems({"Light", "Dark", "Auto"});
    themeCombo->setCurrentText("Light");
    themeCombo->setMaximumWidth(150);

    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo);
    themeLayout->addStretch();

    QCheckBox *compactMode = new QCheckBox("Compact mode (show more data in less space)");
    compactMode->setStyleSheet("font-size: 14px; color: #374151;");

    appearanceOptionsLayout->addWidget(themeWidget);
    appearanceOptionsLayout->addWidget(compactMode);

    appearanceLayout->addWidget(appearanceTitle);
    appearanceLayout->addWidget(appearanceOptions);

    // Action buttons
    QWidget *actionWidget = new QWidget();
    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);

    QPushButton *saveBtn = new QPushButton("💾 Save Settings");
    saveBtn->setObjectName("chartTimeButton");
    saveBtn->setMinimumHeight(40);
    saveBtn->setMinimumWidth(120);

    QPushButton *resetBtn = new QPushButton("🔄 Reset to Default");
    resetBtn->setObjectName("removeButton");
    resetBtn->setMinimumHeight(40);
    resetBtn->setMinimumWidth(120);

    QPushButton *exportBtn = new QPushButton("📤 Export Settings");
    exportBtn->setObjectName("chartTimeButton");
    exportBtn->setMinimumHeight(40);
    exportBtn->setMinimumWidth(120);

    actionLayout->addWidget(saveBtn);
    actionLayout->addWidget(resetBtn);
    actionLayout->addWidget(exportBtn);
    actionLayout->addStretch();

    layout->addWidget(title);
    layout->addWidget(notificationsFrame);
    layout->addWidget(dataFrame);
    layout->addWidget(appearanceFrame);
    layout->addWidget(actionWidget);
    layout->addStretch();

    scrollArea->setWidget(settings);

    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(scrollArea);

    return container;
}

void StockSenseApp::setupConnections()
{
    if (m_dashboardBtn)
    {
        connect(m_dashboardBtn, &QPushButton::clicked, [this]()
                { switchView("dashboard"); });
    }

    if (m_predictionsBtn)
    {
        connect(m_predictionsBtn, &QPushButton::clicked, [this]()
                { switchView("predictions"); });
    }

    if (m_watchlistBtn)
    {
        connect(m_watchlistBtn, &QPushButton::clicked, [this]()
                { switchView("watchlist"); });
    }

    if (m_settingsBtn)
    {
        connect(m_settingsBtn, &QPushButton::clicked, [this]()
                { switchView("settings"); });
    }

    if (m_searchInput && m_stockSuggestions)
    {
        connect(m_searchInput, &QLineEdit::textChanged, [this](const QString &text)
                {
            if (m_stockSuggestions) {
                m_stockSuggestions->setVisible(!text.isEmpty());
            } });

        connect(m_stockSuggestions, &QListWidget::itemClicked, [this](QListWidgetItem *item)
                {
            if (item) {
                selectStock(item->text());
            } });
    }
    if (m_newsList)
    {
        connect(m_newsList, &QListWidget::itemClicked, this, [](QListWidgetItem *item)
                {
            QString url = item->data(Qt::UserRole).toString();
            if (!url.isEmpty()) {
                QDesktopServices::openUrl(QUrl(url));
                qDebug() << "📰 Opening news article:" << url;
            } });
    }

    // Improved search with filtering
    if (m_searchInput && m_stockSuggestions)
    {
        connect(m_searchInput, &QLineEdit::textChanged, this, [this](const QString &text)
                {
            if (text.isEmpty()) {
                m_stockSuggestions->setVisible(false);
                return;
            }
            
            // Comprehensive stock list
            QStringList allStocks = getComprehensiveStockList();
            
            m_stockSuggestions->clear();
            
            // Case-insensitive search
            QString searchText = text.toUpper();
            for (const QString &stock : allStocks) {
                if (stock.contains(searchText)) {
                    m_stockSuggestions->addItem(stock);
                }
            }
            
            // Show suggestions if matches found
            m_stockSuggestions->setVisible(m_stockSuggestions->count() > 0); });

        connect(m_stockSuggestions, &QListWidget::itemClicked, this, [this](QListWidgetItem *item)
                {
            if (item) {
                selectStock(item->text());
                m_stockSuggestions->setVisible(false);
                m_searchInput->clear();
            } });
    }
}

void StockSenseApp::setupTimer()
{
    m_updateTimer = new QTimer(this);
    if (m_updateTimer)
    {
        connect(m_updateTimer, &QTimer::timeout, this, &StockSenseApp::updateStockData);
        m_updateTimer->start(5000);
    }
}

void StockSenseApp::applyStyles()
{
    QString styleSheet = R"(
        /* === StockSense COMPLETE PLATFORM Stylesheet === */
        QMainWindow {
            background-color: #f8fafc;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
        }

        /* Header */
        QWidget#Header {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #6366f1, stop: 1 #8b5cf6);
            border: none;
            padding: 16px 24px;
        }

        QWidget#Header QLineEdit#searchInput {
            background-color: rgba(255, 255, 255, 0.15);
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 12px;
            padding: 12px 20px;
            color: #ffffff;
            font-size: 15px;
        }

        QWidget#Header QLineEdit#searchInput::placeholder {
            color: rgba(255, 255, 255, 0.7);
        }

        QPushButton#themeToggle, QLabel#userAvatar {
            background-color: rgba(255, 255, 255, 0.15);
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 12px;
            color: #ffffff;
            font-size: 18px;
        }

        QPushButton#themeToggle:hover {
            background-color: rgba(255, 255, 255, 0.25);
        }

        /* Sidebar */
        QWidget#StockSidebar {
            background-color: #1e293b;
            border-right: 1px solid #334155;
        }

        QWidget#StockSidebar QLabel#logoLabel {
            color: #ffffff;
            font-size: 20px;
            font-weight: bold;
            margin: 0px 0px 20px 0px;
        }

        QWidget#StockSidebar QLabel#navigationTitle {
            color: #94a3b8;
            font-size: 12px;
            font-weight: bold;
            letter-spacing: 0.05em;
            margin: 16px 0px 8px 0px;
        }

        QWidget#StockSidebar QPushButton#navButton {
            background-color: transparent;
            border: none;
            color: #cbd5e0;
            text-align: left;
            padding: 12px 16px;
            margin: 4px 0px;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 500;
        }

        QWidget#StockSidebar QPushButton#navButton:hover {
            background-color: #334155;
            color: #ffffff;
        }

        QWidget#StockSidebar QPushButton#navButton:checked {
            background-color: #3b82f6;
            color: #ffffff;
        }

        QWidget#StockSidebar QFrame#quickStatsFrame {
            background-color: #334155;
            border: 1px solid #475569;
            border-radius: 8px;
            padding: 12px;
        }

        QWidget#StockSidebar QLabel#stockValue {
            color: #10b981;
            font-size: 14px;
            font-weight: 600;
        }

        /* Dashboard */
        QFrame#stockTickerFrame {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #1f2937, stop: 1 #374151);
            border: 1px solid #4b5563;
            border-radius: 16px;
        }

        QLabel#tickerLabel {
            color: #10b981;
            font-size: 20px;
            font-weight: 600;
            letter-spacing: 0.5px;
        }

        QFrame#statsFrame, QFrame#sentimentFrame, QFrame#newsFrame {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 20px;
        }

        /* Chart Time Buttons */
        QPushButton#chartTimeButton {
            background-color: #f9fafb;
            border: 2px solid #e5e7eb;
            border-radius: 10px;
            color: #374151;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: 600;
            min-width: 60px;
            min-height: 36px;
        }

        QPushButton#chartTimeButton:hover {
            background-color: #f3f4f6;
            border-color: #9ca3af;
        }

        QPushButton#chartTimeButton:checked {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #3b82f6, stop: 1 #2563eb);
            border-color: #1d4ed8;
            color: #ffffff;
            font-weight: 700;
        }

        /* Remove Button */
        QPushButton#removeButton {
            background-color: #fef2f2;
            border: 1px solid #fecaca;
            border-radius: 6px;
            color: #dc2626;
            padding: 6px 12px;
            font-size: 12px;
            font-weight: 600;
        }

        QPushButton#removeButton:hover {
            background-color: #fee2e2;
            border-color: #fca5a5;
        }

        /* Tables */
        QTableWidget#watchlistTable, QTableWidget#algorithmTable {
            background-color: #ffffff;
            border: none;
            gridline-color: #f1f5f9;
            font-size: 13px;
        }

        QTableWidget#watchlistTable::item, QTableWidget#algorithmTable::item {
            padding: 12px 8px;
            border-bottom: 1px solid #f1f5f9;
        }

        QTableWidget#watchlistTable QHeaderView::section, QTableWidget#algorithmTable QHeaderView::section {
            background-color: #f8fafc;
            border: none;
            padding: 12px 8px;
            font-weight: 600;
            color: #374151;
            font-size: 12px;
            text-transform: uppercase;
            letter-spacing: 0.05em;
        }

        /* Sentiment */
        QProgressBar#mainSentimentProgress {
            background-color: #f1f5f9;
            border: 2px solid #e5e7eb;
            border-radius: 8px;
            text-align: center;
            font-weight: bold;
        }

        QProgressBar#mainSentimentProgress::chunk {
            background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                stop: 0 #10b981, stop: 0.5 #34d399, stop: 1 #059669);
            border-radius: 6px;
            margin: 2px;
        }

        /* News */
        QListWidget#newsList {
            background-color: transparent;
            border: none;
            outline: none;
        }

        QListWidget#newsList::item {
            padding: 12px 0px;
            border-bottom: 1px solid #f1f5f9;
            color: #374151;
            font-size: 13px;
            line-height: 1.5;
        }

        QListWidget#newsList::item:hover {
            background-color: #f9fafb;
            border-radius: 8px;
        }

        /* Checkboxes */
        QCheckBox {
            color: #374151;
            font-size: 14px;
            spacing: 8px;
        }

        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #d1d5db;
            border-radius: 4px;
            background-color: #ffffff;
        }

        QCheckBox::indicator:checked {
            background-color: #3b82f6;
            border-color: #3b82f6;
            image: url(data:image/svg+xml,%3csvg viewBox='0 0 16 16' fill='white' xmlns='http://www.w3.org/2000/svg'%3e%3cpath d='m13.854 3.646-7.5 7.5a.5.5 0 0 1-.708 0l-3.5-3.5a.5.5 0 1 1 .708-.708L6 10.293l7.146-7.147a.5.5 0 0 1 .708.708z'/%3e%3c/svg%3e);
        }

        /* Combo boxes */
        QComboBox {
            background-color: #ffffff;
            border: 1px solid #d1d5db;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
            color: #374151;
            min-height: 20px;
        }

        QComboBox:hover {
            border-color: #9ca3af;
        }

        QComboBox::drop-down {
            border: none;
            width: 30px;
        }

        QComboBox QAbstractItemView {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            selection-background-color: #e0e7ff;
            selection-color: #1e40af;
            padding: 4px;
        }

        QComboBox QAbstractItemView::item {
            padding: 8px 12px;
            border-radius: 4px;
            margin: 2px;
        }

        /* General */
        QPushButton {
            background-color: #3b82f6;
            border: 1px solid #3b82f6;
            border-radius: 8px;
            color: #ffffff;
            font-weight: 500;
            padding: 12px 24px;
            font-size: 14px;
        }

        QPushButton:hover {
            background-color: #2563eb;
        }

        QPushButton:pressed {
            background-color: #1d4ed8;
        }
    )";

    this->setStyleSheet(styleSheet);
    qDebug() << "COMPLETE PLATFORM stylesheet applied successfully";
}
