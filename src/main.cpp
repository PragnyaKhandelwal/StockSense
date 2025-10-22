#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QStackedWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QProgressBar>
#include <QScrollArea>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>
#include <QDateTime>
#include <QCloseEvent>
#include <QPainter>

// NO Qt Charts includes - we'll use custom painting

class CustomChartWidget : public QWidget
{
    Q_OBJECT

public:
    CustomChartWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setMinimumHeight(350);
        generateSampleData();

        // Update timer for animated chart
        m_animationTimer = new QTimer(this);
        connect(m_animationTimer, &QTimer::timeout, this, &CustomChartWidget::updateData);
        m_animationTimer->start(3000); // Update every 3 seconds
    }

    void setTimeframe(const QString &timeframe)
    {
        m_timeframe = timeframe;
        generateSampleData();
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Chart area
        QRect chartRect = rect().adjusted(60, 40, -40, -60);

        // Background
        painter.fillRect(rect(), QColor("#ffffff"));
        painter.setPen(QPen(QColor("#e5e7eb"), 1));
        painter.drawRect(chartRect);

        // Grid lines
        painter.setPen(QPen(QColor("#f1f5f9"), 1));
        for (int i = 1; i < 10; ++i)
        {
            int y = chartRect.top() + (chartRect.height() * i / 10);
            painter.drawLine(chartRect.left(), y, chartRect.right(), y);
        }

        for (int i = 1; i < m_dataPoints.size(); ++i)
        {
            int x = chartRect.left() + (chartRect.width() * i / (m_dataPoints.size() - 1));
            painter.drawLine(x, chartRect.top(), x, chartRect.bottom());
        }

        // Draw price line
        if (m_dataPoints.size() > 1)
        {
            painter.setPen(QPen(QColor("#3b82f6"), 3));

            for (int i = 0; i < m_dataPoints.size() - 1; ++i)
            {
                double x1 = chartRect.left() + (chartRect.width() * i / (m_dataPoints.size() - 1));
                double x2 = chartRect.left() + (chartRect.width() * (i + 1) / (m_dataPoints.size() - 1));

                double y1 = chartRect.bottom() - ((m_dataPoints[i] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
                double y2 = chartRect.bottom() - ((m_dataPoints[i + 1] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();

                painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
            }

            // Fill area under curve
            QPolygonF polygon;
            for (int i = 0; i < m_dataPoints.size(); ++i)
            {
                double x = chartRect.left() + (chartRect.width() * i / (m_dataPoints.size() - 1));
                double y = chartRect.bottom() - ((m_dataPoints[i] - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();
                polygon << QPointF(x, y);
            }
            polygon << QPointF(chartRect.right(), chartRect.bottom());
            polygon << QPointF(chartRect.left(), chartRect.bottom());

            QLinearGradient gradient(0, chartRect.top(), 0, chartRect.bottom());
            gradient.setColorAt(0, QColor(59, 130, 246, 100));
            gradient.setColorAt(1, QColor(59, 130, 246, 20));
            painter.setBrush(QBrush(gradient));
            painter.setPen(Qt::NoPen);
            painter.drawPolygon(polygon);
        }

        // Y-axis labels
        painter.setPen(QPen(QColor("#6b7280"), 1));
        painter.setFont(QFont("Arial", 10));

        for (int i = 0; i <= 10; ++i)
        {
            double price = m_minPrice + (m_maxPrice - m_minPrice) * i / 10;
            int y = chartRect.bottom() - (chartRect.height() * i / 10);
            painter.drawText(QRect(10, y - 10, 45, 20), Qt::AlignRight | Qt::AlignVCenter, QString("₹%1").arg(price, 0, 'f', 0));
        }

        // Current price indicator
        if (!m_dataPoints.isEmpty())
        {
            double currentPrice = m_dataPoints.last();
            double y = chartRect.bottom() - ((currentPrice - m_minPrice) / (m_maxPrice - m_minPrice)) * chartRect.height();

            painter.setPen(QPen(QColor("#10b981"), 2));
            painter.drawLine(chartRect.left(), y, chartRect.right(), y);

            // Price label
            painter.fillRect(QRect(chartRect.right() - 80, y - 12, 75, 24), QColor("#10b981"));
            painter.setPen(QPen(QColor("#ffffff"), 1));
            painter.drawText(QRect(chartRect.right() - 80, y - 12, 75, 24), Qt::AlignCenter, QString("₹%1").arg(currentPrice, 0, 'f', 2));
        }

        // Title
        painter.setPen(QPen(QColor("#1f2937"), 1));
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(QRect(60, 10, chartRect.width(), 30), Qt::AlignLeft | Qt::AlignVCenter,
                         QString("RELIANCE Stock Chart (%1)").arg(m_timeframe));
    }

private slots:
    void updateData()
    {
        // Add new data point
        if (!m_dataPoints.isEmpty())
        {
            double lastPrice = m_dataPoints.last();
            double newPrice = lastPrice + (QRandomGenerator::global()->bounded(200) - 100) * 0.5;
            newPrice = qMax(2500.0, qMin(3200.0, newPrice)); // Keep within bounds

            m_dataPoints.append(newPrice);

            // Keep only last 50 points
            if (m_dataPoints.size() > 50)
            {
                m_dataPoints.removeFirst();
            }

            updateMinMax();
            update();
        }
    }

private:
    void generateSampleData()
    {
        m_dataPoints.clear();

        int points = 30;
        if (m_timeframe == "1D")
            points = 24;
        else if (m_timeframe == "1W")
            points = 7;
        else if (m_timeframe == "1M")
            points = 30;
        else if (m_timeframe == "3M")
            points = 90;
        else if (m_timeframe == "1Y")
            points = 52;

        double basePrice = 2847.50;
        for (int i = 0; i < points; ++i)
        {
            double price = basePrice + (QRandomGenerator::global()->bounded(400) - 200) + sin(i * 0.2) * 80;
            m_dataPoints.append(price);
        }

        updateMinMax();
    }

    void updateMinMax()
    {
        if (m_dataPoints.isEmpty())
            return;

        m_minPrice = *std::min_element(m_dataPoints.begin(), m_dataPoints.end());
        m_maxPrice = *std::max_element(m_dataPoints.begin(), m_dataPoints.end());

        // Add some padding
        double padding = (m_maxPrice - m_minPrice) * 0.1;
        m_minPrice -= padding;
        m_maxPrice += padding;
    }

    QVector<double> m_dataPoints;
    double m_minPrice = 2500;
    double m_maxPrice = 3200;
    QString m_timeframe = "1M";
    QTimer *m_animationTimer;
};

class StockSenseApp : public QMainWindow
{
    Q_OBJECT

public:
    StockSenseApp(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        initializePointers();
        setupUI();
        setupConnections();
        setupTimer();
        applyStyles();

        setWindowTitle("StockSense - Professional Stock Analysis Platform");
        setMinimumSize(1400, 900);
        resize(1600, 1000);

        setAttribute(Qt::WA_DeleteOnClose, false);

        qDebug() << "StockSense with Custom Charts initialized successfully";
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (m_updateTimer && m_updateTimer->isActive())
        {
            m_updateTimer->stop();
        }
        qDebug() << "StockSense application closing gracefully";
        event->accept();
    }

private slots:
    void switchView(const QString &viewName)
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
        if (m_algorithmBtn)
            m_algorithmBtn->setChecked(viewName == "algorithm");

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
        else if (viewName == "algorithm" && m_algorithmWidget)
        {
            m_contentStack->setCurrentWidget(m_algorithmWidget);
        }

        qDebug() << "Switched to view:" << viewName;
    }

    void selectStock(const QString &symbol)
    {
        m_currentStock = symbol;
        if (m_searchInput)
            m_searchInput->clear();
        if (m_stockSuggestions)
            m_stockSuggestions->setVisible(false);
        updateStockData();
        qDebug() << "Selected stock:" << symbol;
    }

    void updateStockData()
    {
        try
        {
            double price = 2847.50 + (QRandomGenerator::global()->bounded(200) - 100);
            double change = (QRandomGenerator::global()->bounded(200) - 100) / 10.0;
            double changePercent = (change / price) * 100;

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
            }

            if (m_trendIcon)
            {
                m_trendIcon->setText(change > 0 ? "📈" : "📉");
            }

            updateQuickStats();
            updateSentimentMeter();
            updateNewsFeed();
        }
        catch (const std::exception &e)
        {
            qWarning() << "Error updating stock data:" << e.what();
        }
    }

    void updateQuickStats()
    {
        if (!m_quickStatsLabel)
            return;

        try
        {
            QStringList stats = {
                QString("Market Cap: ₹%1L Cr").arg(QRandomGenerator::global()->bounded(500, 2000)),
                QString("52W H/L: ₹%1/₹%2").arg(QRandomGenerator::global()->bounded(2500, 3500)).arg(QRandomGenerator::global()->bounded(1500, 2500)),
                QString("P/E: %1").arg(QRandomGenerator::global()->bounded(15, 35)),
                QString("Volume: %1M").arg(QRandomGenerator::global()->bounded(1, 10))};
            m_quickStatsLabel->setText(stats.join(" | "));
        }
        catch (const std::exception &e)
        {
            qWarning() << "Error updating quick stats:" << e.what();
        }
    }

    void updateSentimentMeter()
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
                QString label = sentiment >= 70 ? "Bullish" : sentiment >= 40 ? "Neutral"
                                                                              : "Bearish";
                QString color = sentiment >= 70 ? "#10b981" : sentiment >= 40 ? "#f59e0b"
                                                                              : "#ef4444";
                m_sentimentLabel->setText(label);
                m_sentimentLabel->setStyleSheet(QString("font-size: 20px; font-weight: 600; color: %1; text-transform: uppercase; letter-spacing: 0.1em;").arg(color));
            }
        }
        catch (const std::exception &e)
        {
            qWarning() << "Error updating sentiment meter:" << e.what();
        }
    }

    void updateNewsFeed()
    {
        if (!m_newsList)
            return;

        try
        {
            QStringList headlines = {
                QString("%1 Reports Strong Q%2 Earnings").arg(m_currentStock).arg(QRandomGenerator::global()->bounded(1, 5)),
                "Market Analysis: Tech Stocks Outlook",
                "Regulatory Updates Impact Trading",
                "Economic Indicators Show Growth",
                "Sector Performance Review"};

            QString headline = headlines[QRandomGenerator::global()->bounded(headlines.size())];
            QListWidgetItem *item = new QListWidgetItem(QString("🔴 %1 - %2")
                                                            .arg(headline)
                                                            .arg(QDateTime::currentDateTime().toString("hh:mm")));

            m_newsList->insertItem(0, item);

            while (m_newsList->count() > 10)
            {
                QListWidgetItem *oldItem = m_newsList->takeItem(m_newsList->count() - 1);
                delete oldItem;
            }
        }
        catch (const std::exception &e)
        {
            qWarning() << "Error updating news feed:" << e.what();
        }
    }

private:
    void initializePointers()
    {
        m_contentStack = nullptr;
        m_dashboardWidget = nullptr;
        m_predictionsWidget = nullptr;
        m_watchlistWidget = nullptr;
        m_algorithmWidget = nullptr;
        m_settingsWidget = nullptr;

        m_dashboardBtn = nullptr;
        m_predictionsBtn = nullptr;
        m_watchlistBtn = nullptr;
        m_algorithmBtn = nullptr;
        m_settingsBtn = nullptr;

        m_searchInput = nullptr;
        m_stockSuggestions = nullptr;
        m_currentPriceLabel = nullptr;
        m_priceChangeLabel = nullptr;
        m_trendIcon = nullptr;

        m_quickStatsLabel = nullptr;
        m_sentimentScore = nullptr;
        m_sentimentLabel = nullptr;
        m_sentimentProgress = nullptr;
        m_newsList = nullptr;

        m_customChart = nullptr;

        m_updateTimer = nullptr;
        m_currentStock = "RELIANCE";
    }

    void setupUI()
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

    QWidget *createSidebar()
    {
        QWidget *sidebar = new QWidget();
        sidebar->setObjectName("StockSidebar");
        sidebar->setFixedWidth(250);

        QVBoxLayout *layout = new QVBoxLayout(sidebar);
        layout->setContentsMargins(16, 24, 16, 24);
        layout->setSpacing(8);

        QLabel *logo = new QLabel("📈 StockSense");
        logo->setObjectName("logoLabel");
        logo->setAlignment(Qt::AlignCenter);

        QLabel *navTitle = new QLabel("NAVIGATION");
        navTitle->setObjectName("navigationTitle");

        m_dashboardBtn = new QPushButton("🏠 Dashboard");
        m_predictionsBtn = new QPushButton("🧠 Predictions");
        m_watchlistBtn = new QPushButton("⭐ Watchlist");
        m_algorithmBtn = new QPushButton("⚡ Algorithm Monitor");
        m_settingsBtn = new QPushButton("⚙️ Settings");

        QList<QPushButton *> navButtons = {m_dashboardBtn, m_predictionsBtn, m_watchlistBtn, m_algorithmBtn, m_settingsBtn};
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

        QLabel *statsTitle = new QLabel("QUICK STATS");
        statsTitle->setObjectName("navigationTitle");

        QFrame *statsFrame = new QFrame();
        statsFrame->setObjectName("quickStatsFrame");
        QVBoxLayout *statsLayout = new QVBoxLayout(statsFrame);

        QLabel *niftyLabel = new QLabel("NIFTY 50");
        QLabel *niftyValue = new QLabel("19,456.75 (+127.35)");
        niftyValue->setObjectName("stockValue");

        QLabel *sensexLabel = new QLabel("SENSEX");
        QLabel *sensexValue = new QLabel("65,498.25 (+456.78)");
        sensexValue->setObjectName("stockValue");

        statsLayout->addWidget(niftyLabel);
        statsLayout->addWidget(niftyValue);
        statsLayout->addWidget(sensexLabel);
        statsLayout->addWidget(sensexValue);

        layout->addWidget(logo);
        layout->addSpacing(20);
        layout->addWidget(navTitle);
        layout->addWidget(m_dashboardBtn);
        layout->addWidget(m_predictionsBtn);
        layout->addWidget(m_watchlistBtn);
        layout->addWidget(m_algorithmBtn);
        layout->addWidget(m_settingsBtn);
        layout->addSpacing(20);
        layout->addWidget(statsTitle);
        layout->addWidget(statsFrame);
        layout->addStretch();

        return sidebar;
    }

    QWidget *createContentArea()
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
        m_algorithmWidget = createAlgorithmMonitor();
        m_settingsWidget = createSettings();

        if (m_contentStack)
        {
            if (m_dashboardWidget)
                m_contentStack->addWidget(m_dashboardWidget);
            if (m_predictionsWidget)
                m_contentStack->addWidget(m_predictionsWidget);
            if (m_watchlistWidget)
                m_contentStack->addWidget(m_watchlistWidget);
            if (m_algorithmWidget)
                m_contentStack->addWidget(m_algorithmWidget);
            if (m_settingsWidget)
                m_contentStack->addWidget(m_settingsWidget);

            layout->addWidget(m_contentStack);
        }

        return contentArea;
    }
QWidget* createHeader() {
    QWidget *header = new QWidget();
    header->setObjectName("Header");
    
    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(32, 20, 32, 20);
    layout->setSpacing(24);
    
    // Stock selector dropdown - NEW
    QComboBox *stockSelector = new QComboBox();
    stockSelector->setObjectName("stockSelector");
    stockSelector->addItems({"RELIANCE", "TCS", "INFY", "HDFCBANK", "ICICIBANK", "BHARTIARTL", "ITC", "WIPRO", "LT", "MARUTI"});
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
        QComboBox#stockSelector::down-arrow {
            image: none;
            border: none;
            width: 0px;
            height: 0px;
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
        QComboBox#stockSelector QAbstractItemView::item:hover {
            background-color: #f3f4f6;
        }
    )");
    
    // Connect stock selector to update function
    connect(stockSelector, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            [this](const QString &text) {
                selectStock(text);
            });
    
    // Current stock info
    QWidget *stockInfo = new QWidget();
    stockInfo->setMinimumWidth(300);
    QVBoxLayout *stockLayout = new QVBoxLayout(stockInfo);
    stockLayout->setSpacing(8);
    stockLayout->setContentsMargins(0, 0, 0, 0);
    
    QWidget *stockHeader = new QWidget();
    QHBoxLayout *stockHeaderLayout = new QHBoxLayout(stockHeader);
    stockHeaderLayout->setSpacing(12);
    stockHeaderLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *stockSymbol = new QLabel("RELIANCE");
    stockSymbol->setObjectName("stockSymbol");
    stockSymbol->setStyleSheet("font-size: 20px; font-weight: bold; color: #ffffff;");
    
    m_trendIcon = new QLabel("📈");
    m_trendIcon->setStyleSheet("font-size: 20px;");
    
    m_currentPriceLabel = new QLabel("₹2,847.50");
    m_currentPriceLabel->setObjectName("currentPrice");
    m_currentPriceLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #ffffff;");
    
    m_priceChangeLabel = new QLabel("+35.20 (+1.25%)");
    m_priceChangeLabel->setObjectName("priceChange");
    m_priceChangeLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #10b981;");
    
    stockHeaderLayout->addWidget(stockSymbol);
    stockHeaderLayout->addWidget(m_trendIcon);
    stockHeaderLayout->addWidget(m_currentPriceLabel);
    stockHeaderLayout->addWidget(m_priceChangeLabel);
    stockHeaderLayout->addStretch();
    
    stockLayout->addWidget(stockHeader);
    
    // Search widget
    QWidget *searchWidget = new QWidget();
    searchWidget->setMinimumWidth(450);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchWidget);
    searchLayout->setSpacing(0);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    
    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText("Search stocks, news, or analytics...");
    m_searchInput->setObjectName("searchInput");
    m_searchInput->setMinimumWidth(450);
    m_searchInput->setMinimumHeight(44);
    
    m_stockSuggestions = new QListWidget();
    m_stockSuggestions->setObjectName("stockSuggestions");
    m_stockSuggestions->setVisible(false);
    m_stockSuggestions->setMaximumHeight(220);
    
    QStringList stocks = {"RELIANCE", "TCS", "INFY", "HDFCBANK", "ICICIBANK", "BHARTIARTL", "ITC", "WIPRO", "LT", "MARUTI"};
    for (const QString &stock : stocks) {
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
    
    QLabel *networkLabel = new QLabel("Online");
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
    
    layout->addWidget(stockSelector);  // ADD STOCK SELECTOR FIRST
    layout->addWidget(stockInfo);
    layout->addWidget(searchWidget);
    layout->addStretch();
    layout->addWidget(networkStatus);
    layout->addWidget(userSection);
    
    return header;
}
    QWidget *createDashboard()
    {
        QWidget *dashboard = new QWidget();
        dashboard->setObjectName("Dashboard");

        QVBoxLayout *layout = new QVBoxLayout(dashboard);
        layout->setContentsMargins(40, 40, 40, 40);
        layout->setSpacing(40);

        // Stock ticker
        QFrame *tickerFrame = new QFrame();
        tickerFrame->setObjectName("stockTickerFrame");
        tickerFrame->setMinimumHeight(80);
        tickerFrame->setMaximumHeight(80);

        QHBoxLayout *tickerLayout = new QHBoxLayout(tickerFrame);
        tickerLayout->setContentsMargins(32, 20, 32, 20);

        QLabel *ticker = new QLabel("📊 Live Market: NIFTY 19,456 (+127) | SENSEX 65,498 (+456) | BANKNIFTY 43,287 (+289)");
        ticker->setObjectName("tickerLabel");
        ticker->setAlignment(Qt::AlignCenter);
        tickerLayout->addWidget(ticker);

        // Main content area
        QWidget *mainContent = new QWidget();
        QHBoxLayout *mainLayout = new QHBoxLayout(mainContent);
        mainLayout->setSpacing(40);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        // Left column with Custom Chart
        QWidget *leftColumn = new QWidget();
        leftColumn->setMinimumWidth(850);
        QVBoxLayout *leftLayout = new QVBoxLayout(leftColumn);
        leftLayout->setSpacing(32);
        leftLayout->setContentsMargins(0, 0, 0, 0);

        // Create Custom Chart Widget
        QWidget *chartWidget = createCustomChart();

        // Key statistics
        QFrame *statsFrame = new QFrame();
        statsFrame->setObjectName("statsFrame");
        statsFrame->setMinimumHeight(160);

        QVBoxLayout *statsLayout = new QVBoxLayout(statsFrame);
        statsLayout->setContentsMargins(32, 32, 32, 32);
        statsLayout->setSpacing(20);

        QLabel *statsTitle = new QLabel("📋 Key Statistics");
        statsTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");

        m_quickStatsLabel = new QLabel("Market Cap: ₹18.2L Cr | 52W H/L: ₹3,024/₹2,220 | P/E: 24.85 | Volume: 2.3M");
        m_quickStatsLabel->setStyleSheet("color: #6b7280; font-size: 16px; line-height: 1.6;");

        statsLayout->addWidget(statsTitle);
        statsLayout->addWidget(m_quickStatsLabel);

        leftLayout->addWidget(chartWidget);
        leftLayout->addWidget(statsFrame);

        // Right column
        QWidget *rightColumn = new QWidget();
        rightColumn->setMinimumWidth(480);
        rightColumn->setMaximumWidth(480);
        QVBoxLayout *rightLayout = new QVBoxLayout(rightColumn);
        rightLayout->setSpacing(32);
        rightLayout->setContentsMargins(0, 0, 0, 0);

        QFrame *sentimentFrame = createSentimentMeter();
        QFrame *newsFrame = createNewsPanel();

        rightLayout->addWidget(sentimentFrame);
        rightLayout->addWidget(newsFrame);
        rightLayout->addStretch();

        mainLayout->addWidget(leftColumn);
        mainLayout->addWidget(rightColumn);

        layout->addWidget(tickerFrame);
        layout->addWidget(mainContent);

        return dashboard;
    }

    QWidget *createCustomChart()
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

        // Chart header
        QWidget *chartHeader = new QWidget();
        QHBoxLayout *headerLayout = new QHBoxLayout(chartHeader);
        headerLayout->setContentsMargins(0, 0, 0, 0);

        QLabel *chartTitle = new QLabel("📈 RELIANCE Stock Chart");
        chartTitle->setStyleSheet("font-size: 24px; font-weight: 700; color: #1f2937;");

        QWidget *priceWidget = new QWidget();
        QHBoxLayout *priceLayout = new QHBoxLayout(priceWidget);
        priceLayout->setSpacing(16);
        priceLayout->setContentsMargins(0, 0, 0, 0);

        QLabel *livePrice = new QLabel("₹2,847.50");
        livePrice->setStyleSheet("font-size: 28px; font-weight: bold; color: #1f2937;");

        QLabel *liveChange = new QLabel("+35.20 (+1.25%)");
        liveChange->setStyleSheet("font-size: 18px; font-weight: 600; color: #10b981;");

        QLabel *liveTrend = new QLabel("📈");
        liveTrend->setStyleSheet("font-size: 24px;");

        priceLayout->addWidget(livePrice);
        priceLayout->addWidget(liveChange);
        priceLayout->addWidget(liveTrend);
        priceLayout->addStretch();

        headerLayout->addWidget(chartTitle);
        headerLayout->addStretch();
        headerLayout->addWidget(priceWidget);

        // Chart time controls
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

        // CREATE CUSTOM CHART
        m_customChart = new CustomChartWidget();

        // Connect timeframe buttons
        connect(btn1D, &QPushButton::clicked, [this]()
                { if (m_customChart) m_customChart->setTimeframe("1D"); });
        connect(btn1W, &QPushButton::clicked, [this]()
                { if (m_customChart) m_customChart->setTimeframe("1W"); });
        connect(btn1M, &QPushButton::clicked, [this]()
                { if (m_customChart) m_customChart->setTimeframe("1M"); });
        connect(btn3M, &QPushButton::clicked, [this]()
                { if (m_customChart) m_customChart->setTimeframe("3M"); });
        connect(btn1Y, &QPushButton::clicked, [this]()
                { if (m_customChart) m_customChart->setTimeframe("1Y"); });

        containerLayout->addWidget(chartHeader);
        containerLayout->addWidget(chartControls);
        containerLayout->addWidget(m_customChart);

        return chartContainer;
    }

    QFrame *createSentimentMeter()
    {
        QFrame *sentimentFrame = new QFrame();
        sentimentFrame->setObjectName("sentimentFrame");
        sentimentFrame->setMinimumHeight(360);
        sentimentFrame->setMaximumHeight(360);

        QVBoxLayout *layout = new QVBoxLayout(sentimentFrame);
        layout->setContentsMargins(32, 32, 32, 32);
        layout->setSpacing(28);

        QLabel *title = new QLabel("📊 Sentiment Analysis");
        title->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");

        QWidget *overallWidget = new QWidget();
        QVBoxLayout *overallLayout = new QVBoxLayout(overallWidget);
        overallLayout->setSpacing(12);
        overallLayout->setContentsMargins(0, 20, 0, 20);

        m_sentimentScore = new QLabel("72%");
        m_sentimentScore->setObjectName("sentimentScore");
        m_sentimentScore->setAlignment(Qt::AlignCenter);
        m_sentimentScore->setStyleSheet("font-size: 56px; font-weight: 800; color: #10b981;");

        m_sentimentLabel = new QLabel("Bullish");
        m_sentimentLabel->setObjectName("sentimentLabel");
        m_sentimentLabel->setAlignment(Qt::AlignCenter);
        m_sentimentLabel->setStyleSheet("font-size: 20px; font-weight: 600; color: #10b981; text-transform: uppercase; letter-spacing: 0.1em;");

        overallLayout->addWidget(m_sentimentScore);
        overallLayout->addWidget(m_sentimentLabel);

        QWidget *overallProgressWidget = new QWidget();
        QVBoxLayout *overallProgressLayout = new QVBoxLayout(overallProgressWidget);
        overallProgressLayout->setSpacing(8);

        QWidget *overallProgressHeader = new QWidget();
        QHBoxLayout *overallProgressHeaderLayout = new QHBoxLayout(overallProgressHeader);
        overallProgressHeaderLayout->setContentsMargins(0, 0, 0, 0);

        QLabel *overallLabel = new QLabel("Overall Sentiment");
        overallLabel->setStyleSheet("font-size: 16px; color: #374151; font-weight: 600;");

        QLabel *overallValue = new QLabel("72%");
        overallValue->setStyleSheet("font-size: 16px; color: #1f2937; font-weight: 700;");

        overallProgressHeaderLayout->addWidget(overallLabel);
        overallProgressHeaderLayout->addStretch();
        overallProgressHeaderLayout->addWidget(overallValue);

        m_sentimentProgress = new QProgressBar();
        m_sentimentProgress->setMinimum(0);
        m_sentimentProgress->setMaximum(100);
        m_sentimentProgress->setValue(72);
        m_sentimentProgress->setTextVisible(false);
        m_sentimentProgress->setMinimumHeight(15);
        m_sentimentProgress->setMaximumHeight(15);
        m_sentimentProgress->setObjectName("mainSentimentProgress");

        overallProgressLayout->addWidget(overallProgressHeader);
        overallProgressLayout->addWidget(m_sentimentProgress);

        layout->addWidget(title);
        layout->addWidget(overallWidget);
        layout->addWidget(overallProgressWidget);
        layout->addStretch();

        return sentimentFrame;
    }

    QFrame *createNewsPanel()
    {
        QFrame *newsFrame = new QFrame();
        newsFrame->setObjectName("newsFrame");
        newsFrame->setMinimumHeight(380);

        QVBoxLayout *layout = new QVBoxLayout(newsFrame);
        layout->setContentsMargins(32, 32, 32, 32);
        layout->setSpacing(20);

        QWidget *header = new QWidget();
        QHBoxLayout *headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(0, 0, 0, 0);

        QLabel *title = new QLabel("📰 News & Insights");
        title->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");

        QPushButton *refreshBtn = new QPushButton("↻");
        refreshBtn->setMaximumSize(32, 32);

        headerLayout->addWidget(title);
        headerLayout->addStretch();
        headerLayout->addWidget(refreshBtn);

        m_newsList = new QListWidget();
        m_newsList->setObjectName("newsList");
        m_newsList->setFrameShape(QFrame::NoFrame);

        QStringList initialNews = {
            "🔴 RELIANCE Reports Strong Q4 Earnings - 2h ago",
            "🟡 Market Analysis: Tech Stocks Outlook - 4h ago",
            "🔴 Regulatory Updates Impact Trading - 1d ago",
            "🟢 Economic Indicators Show Growth - 2d ago"};

        for (const QString &news : initialNews)
        {
            m_newsList->addItem(news);
        }

        layout->addWidget(header);
        layout->addWidget(m_newsList);

        return newsFrame;
    }

    // Simplified other widgets to prevent crashes

    QWidget* createPredictions() {
    QWidget *predictions = new QWidget();
    predictions->setObjectName("Predictions");
    
    // Use scroll area for better content management
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: transparent;");
    
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollWidget);
    layout->setContentsMargins(50, 50, 50, 50);  // Much larger margins
    layout->setSpacing(50);  // Much more spacing
    
    // Header section with more space
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setSpacing(20);  // Increased spacing
    headerLayout->setContentsMargins(0, 0, 0, 30);  // Bottom margin
    
    QLabel *title = new QLabel("🧠 AI Stock Predictions");
    title->setStyleSheet("font-size: 40px; font-weight: bold; color: #1f2937; margin-bottom: 16px;");  // Larger font
    
    QLabel *subtitle = new QLabel("Advanced data structures and algorithms for intelligent forecasting");
    subtitle->setStyleSheet("font-size: 20px; color: #6b7280; line-height: 1.6;");  // Larger font
    
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    
    // Time range controls with more space
    QWidget *timeRangeWidget = new QWidget();
    timeRangeWidget->setMinimumHeight(80);  // Fixed height
    QHBoxLayout *timeRangeLayout = new QHBoxLayout(timeRangeWidget);
    timeRangeLayout->setSpacing(25);  // Much more spacing
    timeRangeLayout->setContentsMargins(0, 20, 0, 20);  // Vertical margins
    
    QLabel *rangeLabel = new QLabel("Prediction Range:");
    rangeLabel->setStyleSheet("font-weight: 600; color: #374151; font-size: 18px;");  // Larger font
    
    QPushButton *btn7D = new QPushButton("7 Days");
    QPushButton *btn15D = new QPushButton("15 Days");
    QPushButton *btn30D = new QPushButton("30 Days");
    QPushButton *btn60D = new QPushButton("60 Days");
    
    btn30D->setChecked(true);
    QList<QPushButton*> rangeBtns = {btn7D, btn15D, btn30D, btn60D};
    for (auto btn : rangeBtns) {
        btn->setCheckable(true);
        btn->setMinimumWidth(120);  // Larger buttons
        btn->setMinimumHeight(50);  // Taller buttons
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: #f3f4f6;
                border: 2px solid #d1d5db;
                border-radius: 15px;
                color: #374151;
                padding: 15px 30px;
                font-size: 16px;
                font-weight: 600;
            }
            QPushButton:checked {
                background-color: #3b82f6;
                border-color: #3b82f6;
                color: #ffffff;
            }
            QPushButton:hover {
                background-color: #e5e7eb;
                transform: translateY(-2px);
            }
            QPushButton:checked:hover {
                background-color: #2563eb;
            }
        )");
    }
    
    timeRangeLayout->addWidget(rangeLabel);
    timeRangeLayout->addWidget(btn7D);
    timeRangeLayout->addWidget(btn15D);
    timeRangeLayout->addWidget(btn30D);
    timeRangeLayout->addWidget(btn60D);
    timeRangeLayout->addStretch();
    
    // Price summary cards with much more space
    QWidget *priceSummary = new QWidget();
    priceSummary->setMinimumHeight(250);  // Fixed height
    QHBoxLayout *summaryLayout = new QHBoxLayout(priceSummary);
    summaryLayout->setSpacing(40);  // Much more spacing
    summaryLayout->setContentsMargins(0, 20, 0, 20);  // Vertical margins
    
    QStringList cardTitles = {"Current Price", "AI Prediction (30D)", "Expected Change"};
    QStringList cardValues = {"₹2,847.50", "₹2,945.80", "+3.45%"};
    QStringList cardSubtitles = {"As of today", "Confidence: 82%", "₹+98.30"};
    QStringList cardColors = {"#f8fafc", "#eff6ff", "#f0fdf4"};
    QStringList cardBorders = {"#e5e7eb", "#bfdbfe", "#bbf7d0"};
    QStringList cardTextColors = {"#1f2937", "#1d4ed8", "#059669"};
    
    for (int i = 0; i < 3; ++i) {
        QFrame *card = new QFrame();
        card->setMinimumHeight(200);  // Larger height
        card->setStyleSheet(QString(R"(
            QFrame {
                background-color: %1;
                border: 3px solid %2;
                border-radius: 25px;
                padding: 40px;
            }
            QFrame:hover {
                border-color: #3b82f6;
                box-shadow: 0 12px 35px -8px rgba(59, 130, 246, 0.4);
                transform: translateY(-5px);
            }
        )").arg(cardColors[i]).arg(cardBorders[i]));
        
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(20);  // More spacing
        
        QLabel *cardTitle = new QLabel(cardTitles[i]);
        cardTitle->setStyleSheet("font-size: 18px; color: #6b7280; font-weight: 600;");  // Larger font
        cardTitle->setAlignment(Qt::AlignCenter);
        
        QLabel *cardValue = new QLabel(cardValues[i]);
        cardValue->setStyleSheet(QString("font-size: 48px; font-weight: bold; color: %1;").arg(cardTextColors[i]));  // Larger font
        cardValue->setAlignment(Qt::AlignCenter);
        
        QLabel *cardSubtitle = new QLabel(cardSubtitles[i]);
        cardSubtitle->setStyleSheet(QString("font-size: 16px; color: %1; font-weight: 500;").arg(cardTextColors[i]));  // Larger font
        cardSubtitle->setAlignment(Qt::AlignCenter);
        
        cardLayout->addWidget(cardTitle);
        cardLayout->addWidget(cardValue);
        cardLayout->addWidget(cardSubtitle);
        
        summaryLayout->addWidget(card);
    }
    
    // Chart visualization with much more space
    QFrame *chartFrame = new QFrame();
    chartFrame->setMinimumHeight(500);  // Much larger height
    chartFrame->setStyleSheet(R"(
        QFrame {
            background-color: #ffffff;
            border: 2px solid #e5e7eb;
            border-radius: 25px;
            padding: 40px;
        }
    )");
    
    QVBoxLayout *chartLayout = new QVBoxLayout(chartFrame);
    chartLayout->setSpacing(30);  // More spacing
    
    QLabel *chartTitle = new QLabel("📈 Prediction Chart & Analysis");
    chartTitle->setStyleSheet("font-size: 28px; font-weight: 700; color: #1f2937;");  // Larger font
    
    QWidget *chartArea = new QWidget();
    chartArea->setMinimumHeight(400);  // Much larger height
    chartArea->setStyleSheet(R"(
        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, 
                    stop: 0 #f8fafc, stop: 0.5 #ffffff, stop: 1 #f1f5f9);
        border: 2px solid #f1f5f9;
        border-radius: 20px;
    )");
    
    QVBoxLayout *chartAreaLayout = new QVBoxLayout(chartArea);
    chartAreaLayout->setContentsMargins(80, 80, 80, 80);  // Much larger margins
    
    QLabel *chartVisual = new QLabel("📊 AI Prediction Visualization\n\n🔹 Historical Price Data (30 days)\n🔸 Machine Learning Forecast\n🔹 Confidence Intervals\n🔸 Technical Indicators\n🔹 Market Sentiment Integration\n🔸 Deep Learning Models");
    chartVisual->setAlignment(Qt::AlignCenter);
    chartVisual->setStyleSheet("color: #6b7280; font-size: 20px; line-height: 2.2; font-weight: 500;");  // Larger font
    chartVisual->setWordWrap(true);
    
    chartAreaLayout->addWidget(chartVisual);
    
    chartLayout->addWidget(chartTitle);
    chartLayout->addWidget(chartArea);
    
    // Analysis models section with more space
    QWidget *modelsSection = new QWidget();
    modelsSection->setMinimumHeight(300);  // Fixed height
    QVBoxLayout *modelsLayout = new QVBoxLayout(modelsSection);
    modelsLayout->setSpacing(30);  // More spacing
    modelsLayout->setContentsMargins(0, 30, 0, 0);  // Top margin
    
    QLabel *modelsTitle = new QLabel("🤖 Analysis Models");
    modelsTitle->setStyleSheet("font-size: 24px; font-weight: 700; color: #1f2937;");  // Larger font
    
    QWidget *modelsGrid = new QWidget();
    QHBoxLayout *modelsGridLayout = new QHBoxLayout(modelsGrid);
    modelsGridLayout->setSpacing(30);  // More spacing
    
    QStringList modelNames = {"Moving Average", "Fibonacci Analysis", "Multi-Indicator AI"};
    QStringList modelIcons = {"📈", "🎯", "🧠"};
    QStringList modelAccuracy = {"78%", "74%", "82%"};
    QStringList modelComplexity = {"O(n)", "O(1)", "O(n log n)"};
    QStringList modelDescriptions = {"Sliding Window Analysis", "Dynamic Programming", "Ensemble Methods"};
    
    for (int i = 0; i < 3; ++i) {
        QFrame *modelCard = new QFrame();
        modelCard->setMinimumHeight(200);  // Larger height
        modelCard->setStyleSheet(R"(
            QFrame {
                background-color: #ffffff;
                border: 2px solid #e5e7eb;
                border-radius: 18px;
                padding: 30px;
            }
            QFrame:hover {
                border-color: #3b82f6;
                box-shadow: 0 8px 25px -8px rgba(59, 130, 246, 0.2);
                transform: translateY(-3px);
            }
        )");
        
        QVBoxLayout *modelLayout = new QVBoxLayout(modelCard);
        modelLayout->setSpacing(18);  // More spacing
        
        QWidget *modelHeader = new QWidget();
        QHBoxLayout *modelHeaderLayout = new QHBoxLayout(modelHeader);
        modelHeaderLayout->setSpacing(12);
        
        QLabel *modelIcon = new QLabel(modelIcons[i]);
        modelIcon->setStyleSheet("font-size: 30px;");  // Larger icon
        
        QLabel *modelName = new QLabel(modelNames[i]);
        modelName->setStyleSheet("font-size: 18px; font-weight: 600; color: #1f2937;");  // Larger font
        
        modelHeaderLayout->addWidget(modelIcon);
        modelHeaderLayout->addWidget(modelName);
        modelHeaderLayout->addStretch();
        
        QLabel *accuracy = new QLabel(QString("Accuracy: %1").arg(modelAccuracy[i]));
        accuracy->setStyleSheet("font-size: 16px; font-weight: 600; color: #10b981;");  // Larger font
        
        QLabel *complexity = new QLabel(modelDescriptions[i] + " - " + modelComplexity[i]);
        complexity->setStyleSheet("font-size: 14px; color: #8b5cf6; font-family: 'Courier New', monospace; font-weight: 500;");
        
        if (i == 2) {
            QLabel *selected = new QLabel("✓ Selected");
            selected->setStyleSheet("font-size: 14px; color: #059669; font-weight: 700; background-color: #dcfce7; padding: 8px 16px; border-radius: 8px;");
            modelLayout->addWidget(selected);
        }
        
        modelLayout->addWidget(modelHeader);
        modelLayout->addWidget(accuracy);
        modelLayout->addWidget(complexity);
        
        modelsGridLayout->addWidget(modelCard);
    }
    
    modelsLayout->addWidget(modelsTitle);
    modelsLayout->addWidget(modelsGrid);
    
    layout->addWidget(headerWidget);
    layout->addWidget(timeRangeWidget);
    layout->addWidget(priceSummary);
    layout->addWidget(chartFrame);
    layout->addWidget(modelsSection);
    layout->addStretch();  // Add stretch at bottom
    
    scrollArea->setWidget(scrollWidget);
    
    QVBoxLayout *predictionsLayout = new QVBoxLayout(predictions);
    predictionsLayout->addWidget(scrollArea);
    
    return predictions;
}

    QWidget *createWatchlist()
    {
        QWidget *watchlist = new QWidget();
        watchlist->setObjectName("Watchlist");

        QVBoxLayout *layout = new QVBoxLayout(watchlist);
        layout->setContentsMargins(40, 40, 40, 40);
        layout->setSpacing(40);

        // Header
        QWidget *headerWidget = new QWidget();
        QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

        QLabel *title = new QLabel("⭐ My Watchlist");
        title->setStyleSheet("font-size: 32px; font-weight: bold; color: #1f2937;");

        QPushButton *addBtn = new QPushButton("+ Add Stock");
        addBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #10b981;
            border: 1px solid #10b981;
            border-radius: 8px;
            color: #ffffff;
            font-weight: 600;
            padding: 12px 24px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #059669;
        }
    )");

        headerLayout->addWidget(title);
        headerLayout->addStretch();
        headerLayout->addWidget(addBtn);

        // Portfolio summary
        QWidget *summaryWidget = new QWidget();
        QHBoxLayout *summaryLayout = new QHBoxLayout(summaryWidget);
        summaryLayout->setSpacing(40);

        QStringList summaryLabels = {"Total Value", "Total Change", "Performance", "Holdings"};
        QStringList summaryValues = {"$24,567.89", "+$1,234.56", "+5.29%", "4 Stocks"};
        QStringList summaryColors = {"#1f2937", "#10b981", "#10b981", "#6b7280"};

        for (int i = 0; i < 4; ++i)
        {
            QWidget *summaryCard = new QWidget();
            QVBoxLayout *cardLayout = new QVBoxLayout(summaryCard);
            cardLayout->setSpacing(8);

            QLabel *label = new QLabel(summaryLabels[i]);
            label->setStyleSheet("font-size: 14px; color: #6b7280; font-weight: 500;");

            QLabel *value = new QLabel(summaryValues[i]);
            value->setStyleSheet(QString("font-size: 28px; font-weight: bold; color: %1;").arg(summaryColors[i]));

            cardLayout->addWidget(label);
            cardLayout->addWidget(value);

            summaryLayout->addWidget(summaryCard);
        }
        summaryLayout->addStretch();

        // Watchlist items
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollArea->setStyleSheet("background-color: transparent;");

        QWidget *scrollWidget = new QWidget();
        QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
        scrollLayout->setSpacing(16);

        QStringList stocks = {"RELIANCE", "TCS", "INFY", "HDFCBANK"};
        QStringList companies = {"Reliance Industries Ltd.", "Tata Consultancy Services", "Infosys Ltd.", "HDFC Bank Ltd."};
        QStringList prices = {"₹2,847.50", "₹4,125.30", "₹1,789.45", "₹1,642.80"};
        QStringList changes = {"+₹35.20 (+1.25%)", "-₹28.50 (-0.69%)", "+₹22.15 (+1.25%)", "-₹12.30 (-0.74%)"};
        QStringList holdings = {"$8,542.50", "$12,375.90", "$3,649.49", "$5,000.00"};
        QStringList addedDates = {"Jan 15, 2024", "Jan 12, 2024", "Jan 18, 2024", "Jan 20, 2024"};

        for (int i = 0; i < stocks.size(); ++i)
        {
            QFrame *itemFrame = new QFrame();
            itemFrame->setStyleSheet(R"(
            QFrame {
                background-color: #ffffff;
                border: 1px solid #e5e7eb;
                border-radius: 12px;
                padding: 20px;
            }
            QFrame:hover {
                border-color: #3b82f6;
                box-shadow: 0 4px 6px -1px rgba(59, 130, 246, 0.1);
            }
        )");

            QHBoxLayout *itemLayout = new QHBoxLayout(itemFrame);
            itemLayout->setSpacing(20);

            // Stock info section
            QWidget *stockInfo = new QWidget();
            QVBoxLayout *stockLayout = new QVBoxLayout(stockInfo);
            stockLayout->setSpacing(6);

            QWidget *stockHeader = new QWidget();
            QHBoxLayout *stockHeaderLayout = new QHBoxLayout(stockHeader);
            stockHeaderLayout->setSpacing(8);

            QLabel *stockSymbol = new QLabel(stocks[i]);
            stockSymbol->setStyleSheet("font-size: 18px; font-weight: bold; color: #1f2937;");

            QLabel *heartIcon = new QLabel("⭐");
            heartIcon->setStyleSheet("font-size: 16px;");

            stockHeaderLayout->addWidget(stockSymbol);
            stockHeaderLayout->addWidget(heartIcon);
            stockHeaderLayout->addStretch();

            QLabel *companyName = new QLabel(companies[i]);
            companyName->setStyleSheet("font-size: 14px; color: #6b7280;");

            QLabel *addedDate = new QLabel(QString("Added: %1").arg(addedDates[i]));
            addedDate->setStyleSheet("font-size: 12px; color: #9ca3af;");

            stockLayout->addWidget(stockHeader);
            stockLayout->addWidget(companyName);
            stockLayout->addWidget(addedDate);

            // Price info section
            QWidget *priceInfo = new QWidget();
            QVBoxLayout *priceLayout = new QVBoxLayout(priceInfo);
            priceLayout->setSpacing(6);

            QLabel *price = new QLabel(prices[i]);
            price->setStyleSheet("font-size: 20px; font-weight: bold; color: #1f2937;");
            price->setAlignment(Qt::AlignRight);

            QLabel *change = new QLabel(changes[i]);
            QString changeColor = changes[i].startsWith("+") ? "#10b981" : "#ef4444";
            change->setStyleSheet(QString("font-size: 16px; font-weight: 600; color: %1;").arg(changeColor));
            change->setAlignment(Qt::AlignRight);

            QLabel *holding = new QLabel(QString("Holdings: %1").arg(holdings[i]));
            holding->setStyleSheet("font-size: 14px; color: #6b7280;");
            holding->setAlignment(Qt::AlignRight);

            priceLayout->addWidget(price);
            priceLayout->addWidget(change);
            priceLayout->addWidget(holding);

            // Remove button
            QPushButton *removeBtn = new QPushButton("Remove");
            removeBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #f3f4f6;
                border: 1px solid #d1d5db;
                border-radius: 6px;
                color: #374151;
                font-weight: 500;
                padding: 8px 16px;
                font-size: 12px;
                max-width: 80px;
            }
            QPushButton:hover {
                background-color: #ef4444;
                border-color: #ef4444;
                color: #ffffff;
            }
        )");

            itemLayout->addWidget(stockInfo);
            itemLayout->addStretch();
            itemLayout->addWidget(priceInfo);
            itemLayout->addWidget(removeBtn);

            scrollLayout->addWidget(itemFrame);
        }

        scrollLayout->addStretch();
        scrollArea->setWidget(scrollWidget);

        layout->addWidget(headerWidget);
        layout->addWidget(summaryWidget);
        layout->addWidget(scrollArea);

        return watchlist;
    }
    QWidget* createAlgorithmMonitor() {
    QWidget *monitor = new QWidget();
    monitor->setObjectName("AlgorithmMonitor");
    
    // Use scroll area for better content management
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: transparent;");
    
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollWidget);
    layout->setContentsMargins(50, 50, 50, 50);  // Much larger margins
    layout->setSpacing(50);  // Much more spacing
    
    // Header with system status - more spacious
    QWidget *headerWidget = new QWidget();
    headerWidget->setMinimumHeight(100);  // Fixed height
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 20, 0, 20);  // Vertical margins
    
    QLabel *title = new QLabel("📊 Algorithm Performance Monitor");
    title->setStyleSheet("font-size: 36px; font-weight: bold; color: #1f2937;");  // Larger font
    
    QWidget *statusBadges = new QWidget();
    QHBoxLayout *badgesLayout = new QHBoxLayout(statusBadges);
    badgesLayout->setSpacing(20);  // More spacing
    
    QLabel *cpuBadge = new QLabel("💻 CPU: 43.2%");
    cpuBadge->setStyleSheet(R"(
        background-color: #eff6ff;
        color: #1d4ed8;
        border: 2px solid #bfdbfe;
        border-radius: 12px;
        padding: 12px 24px;
        font-size: 16px;
        font-weight: 600;
    )");
    
    QLabel *ramBadge = new QLabel("💾 RAM: 67.8%");
    ramBadge->setStyleSheet(R"(
        background-color: #f0fdf4;
        color: #15803d;
        border: 2px solid #bbf7d0;
        border-radius: 12px;
        padding: 12px 24px;
        font-size: 16px;
        font-weight: 600;
    )");
    
    badgesLayout->addWidget(cpuBadge);
    badgesLayout->addWidget(ramBadge);
    
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(statusBadges);
    
    // System overview cards with more space
    QWidget *overviewWidget = new QWidget();
    overviewWidget->setMinimumHeight(200);  // Fixed height
    QGridLayout *overviewLayout = new QGridLayout(overviewWidget);
    overviewLayout->setSpacing(30);  // More spacing
    overviewLayout->setContentsMargins(0, 20, 0, 20);  // Vertical margins
    
    QStringList overviewLabels = {"Active Algorithms", "Optimizing", "Operations/sec", "Cache Hit Rate"};
    QStringList overviewValues = {"3", "1", "1,915", "92.5%"};
    QStringList overviewColors = {"#3b82f6", "#f59e0b", "#10b981", "#8b5cf6"};
    
    for (int i = 0; i < 4; ++i) {
        QFrame *overviewCard = new QFrame();
        overviewCard->setMinimumHeight(150);  // Larger height
        overviewCard->setStyleSheet(QString(R"(
            QFrame {
                background-color: #ffffff;
                border: 3px solid %1;
                border-radius: 20px;
                padding: 35px;
            }
            QFrame:hover {
                transform: translateY(-5px);
                box-shadow: 0 15px 35px -10px rgba(0, 0, 0, 0.2);
            }
        )").arg(overviewColors[i]));
        
        QVBoxLayout *cardLayout = new QVBoxLayout(overviewCard);
        cardLayout->setSpacing(15);  // More spacing
        
        QLabel *value = new QLabel(overviewValues[i]);
        value->setStyleSheet(QString("font-size: 44px; font-weight: bold; color: %1;").arg(overviewColors[i]));  // Larger font
        value->setAlignment(Qt::AlignCenter);
        
        QLabel *label = new QLabel(overviewLabels[i]);
        label->setStyleSheet("font-size: 16px; color: #6b7280; font-weight: 600;");  // Larger font
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);
        
        cardLayout->addWidget(value);
        cardLayout->addWidget(label);
        
        overviewLayout->addWidget(overviewCard, 0, i);
    }
    
    // Algorithm details section with more space
    QLabel *algorithmsTitle = new QLabel("🔬 Algorithm Details");
    algorithmsTitle->setStyleSheet("font-size: 24px; font-weight: 700; color: #1f2937; margin: 20px 0px;");  // Larger font
    
    QWidget *algorithmsContainer = new QWidget();
    QVBoxLayout *algorithmsLayout = new QVBoxLayout(algorithmsContainer);
    algorithmsLayout->setSpacing(25);  // More spacing
    
    QStringList algorithmNames = {"Binary Search Tree", "Hash Table Lookup", "Graph Traversal", "Dynamic Programming"};
    QStringList algorithmIcons = {"🌳", "🗃️", "🔗", "📈"};
    QStringList algorithmComplexity = {"O(log n)", "O(1)", "O(V + E)", "O(n²)"};
    QStringList algorithmStatus = {"running", "running", "optimizing", "running"};
    QStringList algorithmThroughput = {"450 ops/s", "1200 ops/s", "180 ops/s", "85 ops/s"};
    QStringList algorithmCacheHit = {"94%", "98%", "87%", "91%"};
    
    for (int i = 0; i < algorithmNames.size(); ++i) {
        QFrame *algorithmCard = new QFrame();
        algorithmCard->setMinimumHeight(180);  // Larger height
        algorithmCard->setStyleSheet(R"(
            QFrame {
                background-color: #ffffff;
                border: 2px solid #e5e7eb;
                border-radius: 20px;
                padding: 35px;
            }
            QFrame:hover {
                border-color: #3b82f6;
                box-shadow: 0 8px 25px -8px rgba(59, 130, 246, 0.2);
                transform: translateY(-3px);
            }
        )");
        
        QVBoxLayout *algorithmLayout = new QVBoxLayout(algorithmCard);
        algorithmLayout->setSpacing(25);  // More spacing
        
        // Algorithm header with more space
        QWidget *algorithmHeader = new QWidget();
        QHBoxLayout *algorithmHeaderLayout = new QHBoxLayout(algorithmHeader);
        algorithmHeaderLayout->setSpacing(15);  // More spacing
        
        QLabel *icon = new QLabel(algorithmIcons[i]);
        icon->setStyleSheet("font-size: 32px;");  // Larger icon
        
        QLabel *name = new QLabel(algorithmNames[i]);
        name->setStyleSheet("font-size: 22px; font-weight: 600; color: #1f2937;");  // Larger font
        
        QLabel *status = new QLabel(algorithmStatus[i]);
        QString statusColor = algorithmStatus[i] == "running" ? "#10b981" : "#f59e0b";
        status->setStyleSheet(QString(R"(
            background-color: %1;
            color: #ffffff;
            border-radius: 10px;
            padding: 8px 20px;
            font-size: 14px;
            font-weight: 600;
        )").arg(statusColor));
        
        QLabel *complexity = new QLabel(algorithmComplexity[i]);
        complexity->setStyleSheet("font-size: 16px; color: #8b5cf6; font-family: 'Courier New', monospace; font-weight: 600;");  // Larger font
        
        algorithmHeaderLayout->addWidget(icon);
        algorithmHeaderLayout->addWidget(name);
        algorithmHeaderLayout->addWidget(status);
        algorithmHeaderLayout->addStretch();
        algorithmHeaderLayout->addWidget(complexity);
        
        // Performance metrics with more space
        QWidget *metricsWidget = new QWidget();
        QGridLayout *metricsLayout = new QGridLayout(metricsWidget);
        metricsLayout->setSpacing(25);  // More spacing
        
        QStringList metricLabels = {"CPU Usage", "Memory", "Throughput", "Cache Hit"};
        QStringList metricValues = {"8.2%", "15.4%", algorithmThroughput[i], algorithmCacheHit[i]};
        int metricProgresses[] = {8, 15, 38, i == 0 ? 94 : i == 1 ? 98 : i == 2 ? 87 : 91};
        
        for (int j = 0; j < 4; ++j) {
            QWidget *metricWidget = new QWidget();
            QVBoxLayout *metricLayout = new QVBoxLayout(metricWidget);
            metricLayout->setSpacing(8);  // More spacing
            
            QWidget *metricHeader = new QWidget();
            QHBoxLayout *metricHeaderLayout = new QHBoxLayout(metricHeader);
            metricHeaderLayout->setContentsMargins(0, 0, 0, 0);
            
            QLabel *metricLabel = new QLabel(metricLabels[j]);
            metricLabel->setStyleSheet("font-size: 14px; color: #6b7280; font-weight: 600;");  // Larger font
            
            QLabel *metricValue = new QLabel(metricValues[j]);
            metricValue->setStyleSheet("font-size: 14px; color: #1f2937; font-weight: 700;");  // Larger font
            
            metricHeaderLayout->addWidget(metricLabel);
            metricHeaderLayout->addStretch();
            metricHeaderLayout->addWidget(metricValue);
            
            QProgressBar *metricProgress = new QProgressBar();
            metricProgress->setValue(metricProgresses[j]);
            metricProgress->setTextVisible(false);
            metricProgress->setMinimumHeight(10);  // Larger height
            metricProgress->setStyleSheet(R"(
                QProgressBar {
                    background-color: #f1f5f9;
                    border: none;
                    border-radius: 5px;
                }
                QProgressBar::chunk {
                    background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #3b82f6, stop: 1 #1d4ed8);
                    border-radius: 5px;
                }
            )");
            
            metricLayout->addWidget(metricHeader);
            metricLayout->addWidget(metricProgress);
            
            metricsLayout->addWidget(metricWidget, 0, j);
        }
        
        algorithmLayout->addWidget(algorithmHeader);
        algorithmLayout->addWidget(metricsWidget);
        
        algorithmsLayout->addWidget(algorithmCard);
    }
    
    // Performance summary with more space
    QFrame *summaryFrame = new QFrame();
    summaryFrame->setMinimumHeight(200);  // Larger height
    summaryFrame->setStyleSheet(R"(
        QFrame {
            background-color: #ffffff;
            border: 2px solid #e5e7eb;
            border-radius: 20px;
            padding: 40px;
        }
    )");
    
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryFrame);
    summaryLayout->setSpacing(25);  // More spacing
    
    QLabel *summaryTitle = new QLabel("⚡ System Performance Summary");
    summaryTitle->setStyleSheet("font-size: 24px; font-weight: 700; color: #1f2937;");  // Larger font
    
    QWidget *summaryStats = new QWidget();
    QGridLayout *summaryStatsLayout = new QGridLayout(summaryStats);
    summaryStatsLayout->setSpacing(40);  // More spacing
    
    QStringList summaryLabels = {"Total CPU:", "Total Memory:", "Avg Latency:", "System Status:"};
    QStringList summaryValues = {"43.2%", "67.8%", "6.7ms", "Optimal"};
    
    for (int i = 0; i < 4; ++i) {
        QWidget *statWidget = new QWidget();
        QHBoxLayout *statLayout = new QHBoxLayout(statWidget);
        statLayout->setContentsMargins(0, 0, 0, 0);
        
        QLabel *label = new QLabel(summaryLabels[i]);
        label->setStyleSheet("font-size: 18px; color: #6b7280; font-weight: 600;");  // Larger font
        
        QLabel *value = new QLabel(summaryValues[i]);
        if (i == 3) {
            value->setStyleSheet(R"(
                background-color: #dcfce7;
                color: #15803d;
                border: 2px solid #bbf7d0;
                border-radius: 10px;
                padding: 10px 20px;
                font-size: 16px;
                font-weight: 700;
            )");
        } else {
            value->setStyleSheet("font-size: 18px; color: #1f2937; font-weight: 700;");  // Larger font
        }
        
        statLayout->addWidget(label);
        statLayout->addStretch();
        statLayout->addWidget(value);
        
        summaryStatsLayout->addWidget(statWidget, 0, i);
    }
    
    summaryLayout->addWidget(summaryTitle);
    summaryLayout->addWidget(summaryStats);
    
    layout->addWidget(headerWidget);
    layout->addWidget(overviewWidget);
    layout->addWidget(algorithmsTitle);
    layout->addWidget(algorithmsContainer);
    layout->addWidget(summaryFrame);
    layout->addStretch();  // Add stretch at bottom
    
    scrollArea->setWidget(scrollWidget);
    
    QVBoxLayout *monitorLayout = new QVBoxLayout(monitor);
    monitorLayout->addWidget(scrollArea);
    
    return monitor;
}
    QWidget *createSettings()
    {
        QWidget *settings = new QWidget();
        settings->setObjectName("Settings");

        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollArea->setStyleSheet("background-color: transparent;");

        QWidget *scrollWidget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(scrollWidget);
        layout->setContentsMargins(40, 40, 40, 40);
        layout->setSpacing(40);

        // Title
        QLabel *title = new QLabel("⚙️ Application Settings");
        title->setStyleSheet("font-size: 32px; font-weight: bold; color: #1f2937; margin-bottom: 24px;");

        // Notifications card
        QFrame *notificationsCard = new QFrame();
        notificationsCard->setStyleSheet(R"(
        QFrame {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 16px;
            padding: 24px;
        }
    )");

        QVBoxLayout *notifLayout = new QVBoxLayout(notificationsCard);
        notifLayout->setSpacing(20);

        QWidget *notifHeader = new QWidget();
        QHBoxLayout *notifHeaderLayout = new QHBoxLayout(notifHeader);
        notifHeaderLayout->setSpacing(12);
        notifHeaderLayout->addWidget(new QLabel("🔔"));
        QLabel *notifTitle = new QLabel("Notifications");
        notifTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");
        notifHeaderLayout->addWidget(notifTitle);
        notifHeaderLayout->addStretch();

        QStringList notifLabels = {"General Notifications", "Price Alerts", "Breaking News Alerts"};
        QStringList notifDescriptions = {
            "Receive notifications for important updates",
            "Get notified when stocks reach target prices",
            "Receive notifications for breaking financial news"};
        bool notifDefaults[] = {true, true, false};

        for (int i = 0; i < 3; ++i)
        {
            QWidget *notifSetting = new QWidget();
            QHBoxLayout *notifSettingLayout = new QHBoxLayout(notifSetting);

            QWidget *notifInfo = new QWidget();
            QVBoxLayout *notifInfoLayout = new QVBoxLayout(notifInfo);
            notifInfoLayout->setSpacing(4);

            QLabel *notifLabel = new QLabel(notifLabels[i]);
            notifLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #1f2937;");

            QLabel *notifDesc = new QLabel(notifDescriptions[i]);
            notifDesc->setStyleSheet("font-size: 14px; color: #6b7280;");

            notifInfoLayout->addWidget(notifLabel);
            notifInfoLayout->addWidget(notifDesc);

            QCheckBox *notifCheck = new QCheckBox();
            notifCheck->setChecked(notifDefaults[i]);
            notifCheck->setStyleSheet(R"(
            QCheckBox::indicator {
                width: 20px;
                height: 20px;
                border-radius: 6px;
                border: 2px solid #d1d5db;
                background-color: #ffffff;
            }
            QCheckBox::indicator:checked {
                background-color: #3b82f6;
                border-color: #3b82f6;
            }
        )");

            notifSettingLayout->addWidget(notifInfo);
            notifSettingLayout->addStretch();
            notifSettingLayout->addWidget(notifCheck);

            notifLayout->addWidget(notifSetting);
        }

        notifLayout->insertWidget(0, notifHeader);

        // Data & Display card
        QFrame *dataCard = new QFrame();
        dataCard->setStyleSheet(R"(
        QFrame {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 16px;
            padding: 24px;
        }
    )");

        QVBoxLayout *dataLayout = new QVBoxLayout(dataCard);
        dataLayout->setSpacing(20);

        QWidget *dataHeader = new QWidget();
        QHBoxLayout *dataHeaderLayout = new QHBoxLayout(dataHeader);
        dataHeaderLayout->setSpacing(12);
        dataHeaderLayout->addWidget(new QLabel("🔄"));
        QLabel *dataTitle = new QLabel("Data & Display");
        dataTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");
        dataHeaderLayout->addWidget(dataTitle);
        dataHeaderLayout->addStretch();

        // Refresh interval
        QWidget *refreshWidget = new QWidget();
        QHBoxLayout *refreshLayout = new QHBoxLayout(refreshWidget);

        QWidget *refreshInfo = new QWidget();
        QVBoxLayout *refreshInfoLayout = new QVBoxLayout(refreshInfo);
        refreshInfoLayout->setSpacing(4);

        QLabel *refreshLabel = new QLabel("Refresh Interval");
        refreshLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #1f2937;");

        QLabel *refreshDesc = new QLabel("How often to refresh stock prices and data");
        refreshDesc->setStyleSheet("font-size: 14px; color: #6b7280;");

        refreshInfoLayout->addWidget(refreshLabel);
        refreshInfoLayout->addWidget(refreshDesc);

        QComboBox *refreshCombo = new QComboBox();
        refreshCombo->addItems({"5 seconds", "15 seconds", "30 seconds", "1 minute", "5 minutes"});
        refreshCombo->setCurrentText("30 seconds");
        refreshCombo->setStyleSheet(R"(
        QComboBox {
            background-color: #ffffff;
            border: 1px solid #d1d5db;
            border-radius: 8px;
            padding: 8px 12px;
            color: #374151;
            font-size: 14px;
            min-width: 140px;
        }
        QComboBox:hover {
            border-color: #9ca3af;
        }
    )");

        refreshLayout->addWidget(refreshInfo);
        refreshLayout->addStretch();
        refreshLayout->addWidget(refreshCombo);

        // Currency setting
        QWidget *currencyWidget = new QWidget();
        QHBoxLayout *currencyLayout = new QHBoxLayout(currencyWidget);
        currencyLayout->addWidget(new QLabel("Currency"));
        currencyLayout->addStretch();

        QComboBox *currencyCombo = new QComboBox();
        currencyCombo->addItems({"INR (₹)", "USD ($)", "EUR (€)", "GBP (£)"});
        currencyCombo->setStyleSheet(R"(
        QComboBox {
            background-color: #ffffff;
            border: 1px solid #d1d5db;
            border-radius: 8px;
            padding: 8px 12px;
            color: #374151;
            font-size: 14px;
            min-width: 120px;
        }
    )");
        currencyLayout->addWidget(currencyCombo);

        dataLayout->addWidget(dataHeader);
        dataLayout->addWidget(refreshWidget);
        dataLayout->addWidget(currencyWidget);

        // Privacy & Security card
        QFrame *privacyCard = new QFrame();
        privacyCard->setStyleSheet(R"(
        QFrame {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 16px;
            padding: 24px;
        }
    )");

        QVBoxLayout *privacyLayout = new QVBoxLayout(privacyCard);
        privacyLayout->setSpacing(20);

        QWidget *privacyHeader = new QWidget();
        QHBoxLayout *privacyHeaderLayout = new QHBoxLayout(privacyHeader);
        privacyHeaderLayout->setSpacing(12);
        privacyHeaderLayout->addWidget(new QLabel("🔒"));
        QLabel *privacyTitle = new QLabel("Privacy & Security");
        privacyTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #1f2937;");
        privacyHeaderLayout->addWidget(privacyTitle);
        privacyHeaderLayout->addStretch();

        QStringList privacyLabels = {"Share Analytics Data", "Crash Reports"};
        QStringList privacyDescriptions = {
            "Help improve the app by sharing anonymous usage data",
            "Send crash reports to help fix issues"};
        bool privacyDefaults[] = {true, true};

        for (int i = 0; i < 2; ++i)
        {
            QWidget *privacySetting = new QWidget();
            QHBoxLayout *privacySettingLayout = new QHBoxLayout(privacySetting);

            QWidget *privacyInfo = new QWidget();
            QVBoxLayout *privacyInfoLayout = new QVBoxLayout(privacyInfo);
            privacyInfoLayout->setSpacing(4);

            QLabel *privacyLabel = new QLabel(privacyLabels[i]);
            privacyLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #1f2937;");

            QLabel *privacyDesc = new QLabel(privacyDescriptions[i]);
            privacyDesc->setStyleSheet("font-size: 14px; color: #6b7280;");

            privacyInfoLayout->addWidget(privacyLabel);
            privacyInfoLayout->addWidget(privacyDesc);

            QCheckBox *privacyCheck = new QCheckBox();
            privacyCheck->setChecked(privacyDefaults[i]);
            privacyCheck->setStyleSheet(R"(
            QCheckBox::indicator {
                width: 20px;
                height: 20px;
                border-radius: 6px;
                border: 2px solid #d1d5db;
                background-color: #ffffff;
            }
            QCheckBox::indicator:checked {
                background-color: #3b82f6;
                border-color: #3b82f6;
            }
        )");

            privacySettingLayout->addWidget(privacyInfo);
            privacySettingLayout->addStretch();
            privacySettingLayout->addWidget(privacyCheck);

            privacyLayout->addWidget(privacySetting);
        }

        privacyLayout->insertWidget(0, privacyHeader);

        // Action buttons
        QWidget *actionButtons = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionButtons);
        actionLayout->addStretch();

        QPushButton *resetBtn = new QPushButton("Reset to Defaults");
        resetBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f3f4f6;
            border: 1px solid #d1d5db;
            border-radius: 8px;
            color: #374151;
            font-weight: 500;
            padding: 12px 24px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #e5e7eb;
        }
    )");

        QPushButton *saveBtn = new QPushButton("💾 Save Settings");
        saveBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #10b981;
            border: 1px solid #10b981;
            border-radius: 8px;
            color: #ffffff;
            font-weight: 600;
            padding: 12px 24px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #059669;
        }
    )");

        actionLayout->addWidget(resetBtn);
        actionLayout->addWidget(saveBtn);

        layout->addWidget(title);
        layout->addWidget(notificationsCard);
        layout->addWidget(dataCard);
        layout->addWidget(privacyCard);
        layout->addWidget(actionButtons);

        scrollArea->setWidget(scrollWidget);

        QVBoxLayout *settingsLayout = new QVBoxLayout(settings);
        settingsLayout->addWidget(scrollArea);

        return settings;
    }
    void setupConnections()
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
        if (m_algorithmBtn)
        {
            connect(m_algorithmBtn, &QPushButton::clicked, [this]()
                    { switchView("algorithm"); });
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
    }

    void setupTimer()
    {
        m_updateTimer = new QTimer(this);
        if (m_updateTimer)
        {
            connect(m_updateTimer, &QTimer::timeout, this, &StockSenseApp::updateStockData);
            m_updateTimer->start(5000); // Update every 5 seconds
        }
    }

    void applyStyles()
    {
        QString styleSheet = R"(

/* === StockSense Custom Charts Stylesheet === */

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
    padding: 16px 0px;
    border-bottom: 1px solid #f1f5f9;
    color: #374151;
    font-size: 14px;
    line-height: 1.6;
}

QListWidget#newsList::item:hover {
    background-color: #f9fafb;
    border-radius: 8px;
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
        qDebug() << "Custom Charts stylesheet applied successfully";
    }

private:
    // UI Components
    QStackedWidget *m_contentStack;
    QWidget *m_dashboardWidget;
    QWidget *m_predictionsWidget;
    QWidget *m_watchlistWidget;
    QWidget *m_algorithmWidget;
    QWidget *m_settingsWidget;

    // Sidebar buttons
    QPushButton *m_dashboardBtn;
    QPushButton *m_predictionsBtn;
    QPushButton *m_watchlistBtn;
    QPushButton *m_algorithmBtn;
    QPushButton *m_settingsBtn;

    // Header components
    QLineEdit *m_searchInput;
    QListWidget *m_stockSuggestions;
    QLabel *m_currentPriceLabel;
    QLabel *m_priceChangeLabel;
    QLabel *m_trendIcon;

    // Dashboard components
    QLabel *m_quickStatsLabel;
    QLabel *m_sentimentScore;
    QLabel *m_sentimentLabel;
    QProgressBar *m_sentimentProgress;
    QListWidget *m_newsList;

    // Custom Chart
    CustomChartWidget *m_customChart;

    // Data
    QString m_currentStock;
    QTimer *m_updateTimer;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("StockSense");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("StockSense Analytics");
    app.setOrganizationDomain("stocksense.ai");

    try
    {
        StockSenseApp window;
        window.show();

        qDebug() << "🚀 StockSense with Custom Charts launched successfully!";
        qDebug() << "📊 Features: Hand-painted Charts | Interactive Timeframes | Live Updates";
        qDebug() << "⚡ Qt Version:" << QT_VERSION_STR;

        return app.exec();
    }
    catch (const std::exception &e)
    {
        qCritical() << "Fatal error:" << e.what();
        return -1;
    }
}

#include "main.moc"
