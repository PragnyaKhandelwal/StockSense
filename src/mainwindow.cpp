#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QFrame>
#include <QScrollArea>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QDateTime>
#include <QRandomGenerator>
#include <QtCharts>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>
#include <QtCharts/QScatterSeries>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QApplication>
#include <QScreen>


class QChart;
class QChartView;
class QLineSeries;
class QAreaSeries;
class QBarSeries;
class QBarSet;
class QDateTimeAxis;
class QValueAxis;
class QCandlestickSeries;
class QCandlestickSet;
class QScatterSeries;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_stackedWidget(nullptr)
    , m_scrollArea(nullptr)
    , m_tickerTimer(new QTimer(this))
    , m_selectedStock("RELIANCE")
    , m_darkTheme(false)
{
    ui->setupUi(this);
    
    setWindowTitle("StockSense - Professional Stock Analysis Platform");
    setMinimumSize(1600, 1000);
    resize(1800, 1200);
    
    // Initialize data
    m_watchlist = {"RELIANCE", "TCS", "INFY", "HDFCBANK", "ICICIBANK", "BHARTIARTL", "ITC", "WIPRO"};
    
    setupUI();
    applyTheme();
    
    // Start real-time updates
    m_tickerTimer->setInterval(3000); // 3 seconds
    connect(m_tickerTimer, &QTimer::timeout, this, &MainWindow::updateTicker);
    m_tickerTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // Create main layout structure matching React App.tsx
    auto centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    
    auto mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Header Section (matches Header.tsx)
    setupHeader(mainLayout);
    
    // Main Content Area
    auto contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    
    // Sidebar (matches StockSidebar.tsx)
    setupSidebar(contentLayout);
    
    // Main Content with ScrollArea
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setStyleSheet("QScrollArea { border: none; background: #f8fafc; }");
    
    // Stacked Widget for different pages
    m_stackedWidget = new QStackedWidget;
    m_scrollArea->setWidget(m_stackedWidget);
    
    contentLayout->addWidget(m_scrollArea, 1);
    mainLayout->addLayout(contentLayout, 1);
    
    // Setup all pages
    setupDashboardPage();      // Index 0
    setupWatchlistPage();      // Index 1
    setupPredictionsPage();    // Index 2
    setupPortfolioPage();      // Index 3
    setupNewsPage();           // Index 4
    setupScannerPage();        // Index 5
    setupSettingsPage();       // Index 6
    
    // Start with Dashboard
    m_stackedWidget->setCurrentIndex(0);
}

void MainWindow::setupHeader(QVBoxLayout* parentLayout)
{
    auto headerWidget = new QWidget;
    headerWidget->setFixedHeight(70);
    headerWidget->setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "             stop:0 #667eea, stop:1 #764ba2); "
        "  border: none; "
        "}"
    );
    
    auto headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);
    headerLayout->setSpacing(20);
    
    // Logo and Title
    auto logoLayout = new QHBoxLayout;
    auto logoLabel = new QLabel("�");
    logoLabel->setStyleSheet("font-size: 28px;");
    
    auto titleLabel = new QLabel("StockSense");
    titleLabel->setStyleSheet(
        "font-size: 24px; "
        "font-weight: bold; "
        "color: white; "
        "margin-left: 10px;"
    );
    
    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(titleLabel);
    logoLayout->addStretch();
    
    // Search Bar (matches Header.tsx search)
    auto searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Search stocks, news, or analytics...");
    searchEdit->setFixedWidth(400);
    searchEdit->setStyleSheet(
        "QLineEdit { "
        "  padding: 12px 16px; "
        "  border: 2px solid rgba(255,255,255,0.3); "
        "  border-radius: 25px; "
        "  background: rgba(255,255,255,0.15); "
        "  color: white; "
        "  font-size: 14px; "
        "  backdrop-filter: blur(10px); "
        "}"
        "QLineEdit::placeholder { color: rgba(255,255,255,0.7); }"
        "QLineEdit:focus { "
        "  border-color: rgba(255,255,255,0.8); "
        "  background: rgba(255,255,255,0.25); "
        "}"
    );
    
    // Header Controls
    auto controlsLayout = new QHBoxLayout;
    
    // Market Status Indicator
    auto marketStatusLabel = new QLabel("🟢 Market Open");
    marketStatusLabel->setStyleSheet(
        "color: white; "
        "font-size: 12px; "
        "font-weight: bold; "
        "padding: 6px 12px; "
        "background: rgba(255,255,255,0.2); "
        "border-radius: 15px;"
    );
    
    // Theme Toggle Button
    auto themeToggle = new QPushButton("🌙");
    themeToggle->setFixedSize(40, 40);
    themeToggle->setStyleSheet(
        "QPushButton { "
        "  background: rgba(255,255,255,0.2); "
        "  border: 2px solid rgba(255,255,255,0.3); "
        "  border-radius: 20px; "
        "  color: white; "
        "  font-size: 16px; "
        "}"
        "QPushButton:hover { "
        "  background: rgba(255,255,255,0.3); "
        "  border-color: rgba(255,255,255,0.5); "
        "}"
    );
    connect(themeToggle, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    
    // Notifications Button
    auto notifButton = new QPushButton("🔔");
    notifButton->setFixedSize(40, 40);
    notifButton->setStyleSheet(themeToggle->styleSheet());
    
    // User Profile
    auto profileButton = new QPushButton("👤");
    profileButton->setFixedSize(40, 40);
    profileButton->setStyleSheet(themeToggle->styleSheet());
    
    controlsLayout->addWidget(marketStatusLabel);
    controlsLayout->addSpacing(15);
    controlsLayout->addWidget(themeToggle);
    controlsLayout->addWidget(notifButton);
    controlsLayout->addWidget(profileButton);
    
    headerLayout->addLayout(logoLayout);
    headerLayout->addWidget(searchEdit);
    headerLayout->addStretch();
    headerLayout->addLayout(controlsLayout);
    
    parentLayout->addWidget(headerWidget);
}

void MainWindow::setupSidebar(QHBoxLayout* parentLayout)
{
    auto sidebarWidget = new QWidget;
    sidebarWidget->setFixedWidth(280);
    sidebarWidget->setStyleSheet(
        "QWidget { "
        "  background: #1e293b; "
        "  border-right: 1px solid #334155; "
        "}"
    );
    
    auto sidebarLayout = new QVBoxLayout(sidebarWidget);
    sidebarLayout->setContentsMargins(15, 20, 15, 20);
    sidebarLayout->setSpacing(8);
    
    // Navigation Header
    auto navHeaderLabel = new QLabel("Navigation");
    navHeaderLabel->setStyleSheet(
        "color: #94a3b8; "
        "font-size: 12px; "
        "font-weight: bold; "
        "text-transform: uppercase; "
        "letter-spacing: 1px; "
        "margin-bottom: 10px;"
    );
    sidebarLayout->addWidget(navHeaderLabel);
    
    // Navigation Buttons (matches StockSidebar.tsx)
    struct NavItem {
        QString icon;
        QString text;
        QString description;
        std::function<void()> action;
    };
    
    std::vector<NavItem> navItems = {
        {"📊", "Dashboard", "Market Overview & Analytics", [this]() { showDashboard(); }},
        {"⭐", "Watchlist", "Track Favorite Stocks", [this]() { showWatchlist(); }},
        {"💼", "Portfolio", "Manage Holdings", [this]() { showPortfolio(); }},
        {"🔮", "Predictions", "AI Market Forecasts", [this]() { showPredictions(); }},
        {"📰", "News & Sentiment", "Market News & Analysis", [this]() { showNews(); }},
        {"🔍", "Scanner", "Stock Screener & Scanner", [this]() { showScanner(); }},
        {"⚙️", "Settings", "App Preferences", [this]() { showSettings(); }}
    };
    
    m_navButtons.clear();
    for (size_t i = 0; i < navItems.size(); ++i) {
        auto button = createNavButton(navItems[i].icon, navItems[i].text, navItems[i].description);
        connect(button, &QPushButton::clicked, navItems[i].action);
        sidebarLayout->addWidget(button);
        m_navButtons.append(button);
        
        if (i == 0) button->setProperty("selected", true); // Dashboard selected by default
    }
    
    sidebarLayout->addStretch();
    
    // Quick Market Stats in Sidebar
    auto quickStatsLabel = new QLabel("Quick Stats");
    quickStatsLabel->setStyleSheet(navHeaderLabel->styleSheet());
    sidebarLayout->addWidget(quickStatsLabel);
    
    // Market Indices
    QStringList indices = {"NIFTY 50", "SENSEX", "BANK NIFTY"};
    QList<double> values = {19456.75, 65498.25, 43287.90};
    QList<double> changes = {127.35, 289.65, -156.20};
    
    for (int i = 0; i < indices.size(); ++i) {
        auto indexWidget = new QWidget;
        auto indexLayout = new QVBoxLayout(indexWidget);
        indexLayout->setContentsMargins(12, 8, 12, 8);
        indexLayout->setSpacing(2);
        
        auto nameLabel = new QLabel(indices[i]);
        nameLabel->setStyleSheet("color: #cbd5e1; font-size: 11px; font-weight: 500;");
        
        auto valueLabel = new QLabel(QString("%1").arg(values[i], 0, 'f', 2));
        valueLabel->setStyleSheet("color: white; font-size: 13px; font-weight: bold;");
        
        double changePercent = (changes[i] / values[i]) * 100;
        QString changeText = QString("%1%2 (%3%4%)")
                                .arg(changes[i] >= 0 ? "+" : "")
                                .arg(changes[i], 0, 'f', 2)
                                .arg(changePercent >= 0 ? "+" : "")
                                .arg(changePercent, 0, 'f', 2);
        
        auto changeLabel = new QLabel(changeText);
        QString color = changes[i] >= 0 ? "#10b981" : "#ef4444";
        changeLabel->setStyleSheet(QString("color: %1; font-size: 10px; font-weight: bold;").arg(color));
        
        indexLayout->addWidget(nameLabel);
        indexLayout->addWidget(valueLabel);
        indexLayout->addWidget(changeLabel);
        
        indexWidget->setStyleSheet(
            "QWidget { "
            "  background: rgba(255,255,255,0.05); "
            "  border-radius: 6px; "
            "  margin: 2px 0px; "
            "}"
            "QWidget:hover { background: rgba(255,255,255,0.1); }"
        );
        
        sidebarLayout->addWidget(indexWidget);
    }
    
    parentLayout->addWidget(sidebarWidget);
}

QPushButton* MainWindow::createNavButton(const QString& icon, const QString& text, const QString& description)
{
    auto button = new QPushButton;
    button->setFixedHeight(60);
    button->setCheckable(true);
    
    auto layout = new QHBoxLayout(button);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(12);
    
    auto iconLabel = new QLabel(icon);
    iconLabel->setStyleSheet("font-size: 20px;");
    iconLabel->setFixedWidth(24);
    
    auto textLayout = new QVBoxLayout;
    textLayout->setSpacing(2);
    
    auto titleLabel = new QLabel(text);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #e2e8f0;");
    
    auto descLabel = new QLabel(description);
    descLabel->setStyleSheet("font-size: 11px; color: #94a3b8;");
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(descLabel);
    
    layout->addWidget(iconLabel);
    layout->addLayout(textLayout);
    layout->addStretch();
    
    button->setStyleSheet(
        "QPushButton { "
        "  background: transparent; "
        "  border: none; "
        "  border-radius: 8px; "
        "  text-align: left; "
        "  padding: 0px; "
        "}"
        "QPushButton:hover { "
        "  background: rgba(255,255,255,0.1); "
        "}"
        "QPushButton:checked { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "             stop:0 #3b82f6, stop:1 #1d4ed8); "
        "  border-left: 3px solid #60a5fa; "
        "}"
    );
    
    return button;
}
    
void MainWindow::setupDashboardPage()
{
    auto dashboardPage = new QWidget;
    auto dashboardLayout = new QVBoxLayout(dashboardPage);
    dashboardLayout->setContentsMargins(25, 25, 25, 25);
    dashboardLayout->setSpacing(20);

    // Page Title
    auto titleLabel = new QLabel("📈 Dashboard");
    titleLabel->setStyleSheet(
        "font-size: 28px; "
        "font-weight: bold; "
        "color: #1e293b; "
        "margin-bottom: 10px;"
    );
    dashboardLayout->addWidget(titleLabel);

    // Market Ticker
    setupMarketTicker(dashboardLayout);

    // Main content grid
    auto mainGrid = new QGridLayout;
    mainGrid->setSpacing(20);

    // Left Column
    auto leftColumn = new QVBoxLayout;
    leftColumn->setSpacing(20);
    leftColumn->addWidget(setupAdvancedStockChart());
    leftColumn->addWidget(setupTechnicalIndicators());
    mainGrid->addLayout(leftColumn, 0, 0, 1, 2); // Spans 2 columns

    // Right Column
    auto rightColumn = new QVBoxLayout;
    rightColumn->setSpacing(20);
    rightColumn->addWidget(setupQuickStatsPanel());
    rightColumn->addWidget(setupMarketOverview());
    mainGrid->addLayout(rightColumn, 0, 2);

    // Second Row
    mainGrid->addWidget(setupSentimentAnalysis(), 1, 0);
    mainGrid->addWidget(setupGainersLosers(), 1, 1);
    mainGrid->addWidget(setupNewsPanel(), 1, 2);
    
    // Third Row - System Internals
    mainGrid->addWidget(setupDataStructuresAnalyzer(), 2, 0);
    mainGrid->addWidget(setupAlgorithmMonitor(), 2, 1);
    mainGrid->addWidget(setupPerformanceMetrics(), 2, 2);

    mainGrid->setColumnStretch(0, 2);
    mainGrid->setColumnStretch(1, 2);
    mainGrid->setColumnStretch(2, 1);

    dashboardLayout->addLayout(mainGrid);
    dashboardLayout->addStretch();

    m_stackedWidget->addWidget(dashboardPage);
}

void MainWindow::setupMarketTicker(QVBoxLayout* parentLayout)
{
    auto tickerCard = createDashboardCard("🎯 Live Market Ticker", "#1e40af");
    auto tickerLayout = qobject_cast<QVBoxLayout*>(tickerCard->layout());
    
    // Scrolling ticker container
    auto tickerContainer = new QWidget;
    tickerContainer->setFixedHeight(80);
    tickerContainer->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "           stop:0 #f1f5f9, stop:0.5 #e2e8f0, stop:1 #f1f5f9); "
        "border-radius: 8px; "
        "border: 1px solid #cbd5e1;"
    );
    
    auto tickerScrollLayout = new QHBoxLayout(tickerContainer);
    tickerScrollLayout->setSpacing(20);
    tickerScrollLayout->setContentsMargins(15, 10, 15, 10);
    
    // Top Indian stocks for ticker
    QStringList tickerStocks = {
        "RELIANCE", "TCS", "HDFCBANK", "INFY", "ICICIBANK", 
        "BHARTIARTL", "ITC", "KOTAKBANK", "LT", "ASIANPAINT",
        "HINDUNILVR", "AXISBANK", "MARUTI", "SUNPHARMA", "ULTRACEMCO"
    };
    
    for (const QString& stock : tickerStocks) {
        auto stockItem = createTickerItem(stock);
        tickerScrollLayout->addWidget(stockItem);
    }
    
    tickerScrollLayout->addStretch();
    tickerLayout->addWidget(tickerContainer);
    parentLayout->addWidget(tickerCard);
}

QWidget* MainWindow::createTickerItem(const QString& symbol)
{
    auto widget = new QWidget;
    widget->setFixedSize(120, 60);
    
    auto layout = new QVBoxLayout(widget);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(2);
    
    auto symbolLabel = new QLabel(symbol);
    symbolLabel->setStyleSheet("font-weight: bold; font-size: 12px; color: #1e293b;");
    symbolLabel->setAlignment(Qt::AlignCenter);
    
    // Generate realistic stock price
    double basePrice = 500 + QRandomGenerator::global()->bounded(4000);
    auto priceLabel = new QLabel(QString("₹%1").arg(basePrice, 0, 'f', 2));
    priceLabel->setStyleSheet("font-size: 11px; color: #475569; font-weight: 500;");
    priceLabel->setAlignment(Qt::AlignCenter);
    
    // Generate price change
    double change = (QRandomGenerator::global()->bounded(200) - 100) / 10.0;
    double changePercent = (change / basePrice) * 100;
    QString changeText = QString("%1%2 (%3%4%)")
                            .arg(change >= 0 ? "+" : "")
                            .arg(change, 0, 'f', 2)
                            .arg(changePercent >= 0 ? "+" : "")
                            .arg(changePercent, 0, 'f', 2);
    
    auto changeLabel = new QLabel(changeText);
    QString color = change >= 0 ? "#059669" : "#dc2626";
    changeLabel->setStyleSheet(QString("font-size: 9px; color: %1; font-weight: bold;").arg(color));
    changeLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(symbolLabel);
    layout->addWidget(priceLabel);
    layout->addWidget(changeLabel);
    
    widget->setStyleSheet(
        "QWidget { "
        "  background: white; "
        "  border: 1px solid #e2e8f0; "
        "  border-radius: 6px; "
        "  margin: 2px; "
        "}"
        "QWidget:hover { "
        "  background: #f8fafc; "
        "  border-color: #3b82f6; "
        "  transform: scale(1.02); "
        "}"
    );
    
    return widget;
}

QWidget* MainWindow::setupAdvancedStockChart()
{
    auto chartCard = createDashboardCard(QString("📈 %1 - Advanced Analysis").arg(m_selectedStock), "#059669");
    auto chartLayout = qobject_cast<QVBoxLayout*>(chartCard->layout());
    
    // Chart Controls Bar
    auto controlsWidget = new QWidget;
    auto controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(12);
    
    // Stock Selector
    auto stockCombo = new QComboBox;
    stockCombo->addItems(m_watchlist);
    stockCombo->setCurrentText(m_selectedStock);
    stockCombo->setStyleSheet(
        "QComboBox { "
        "  padding: 8px 12px; "
        "  border: 2px solid #3b82f6; "
        "  border-radius: 8px; "
        "  background: white; "
        "  font-weight: bold; "
        "  min-width: 120px; "
        "}"
    );
    
    // Time Range Buttons
    QStringList timeRanges = {"1D", "1W", "1M", "3M", "6M", "1Y", "5Y", "MAX"};
    auto timeButtonGroup = new QButtonGroup(this);
    auto timeButtonsWidget = new QWidget;
    auto timeButtonsLayout = new QHBoxLayout(timeButtonsWidget);
    timeButtonsLayout->setSpacing(4);
    
    for (int i = 0; i < timeRanges.size(); ++i) {
        auto button = new QPushButton(timeRanges[i]);
        button->setCheckable(true);
        button->setFixedSize(40, 32);
        button->setStyleSheet(
            "QPushButton { "
            "  border: 1px solid #cbd5e1; "
            "  border-radius: 6px; "
            "  background: white; "
            "  font-size: 11px; "
            "}"
            "QPushButton:checked { "
            "  background: #3b82f6; "
            "  color: white; "
            "  border-color: #3b82f6; "
            "}"
            "QPushButton:hover:!checked { "
            "  background: #f1f5f9; "
            "}"
        );
        timeButtonGroup->addButton(button, i);
        timeButtonsLayout->addWidget(button);
        
        if (i == 2) button->setChecked(true); // Default to 1M
    }
    
    // Chart Type Selector
    auto chartTypeCombo = new QComboBox;
    chartTypeCombo->addItems({"📈 Line", "📊 Bars", "🕯️ Candlestick", "📉 Area"});
    chartTypeCombo->setStyleSheet(stockCombo->styleSheet());
    
    controlsLayout->addWidget(new QLabel("Stock:"));
    controlsLayout->addWidget(stockCombo);
    controlsLayout->addSpacing(20);
    controlsLayout->addWidget(timeButtonsWidget);
    controlsLayout->addStretch();
    controlsLayout->addWidget(new QLabel("Chart:"));
    controlsLayout->addWidget(chartTypeCombo);
    
    chartLayout->addWidget(controlsWidget);
    
    // Main Chart Area with QtCharts
    auto chartView = createAdvancedChartView();
    chartView->setMinimumHeight(450);
    chartLayout->addWidget(chartView);
    
    // Technical Indicators Panel
    auto indicatorsWidget = setupTechnicalIndicators();
    chartLayout->addWidget(indicatorsWidget);
    
    return chartCard;
}

QChartView* MainWindow::createAdvancedChartView()
{
    auto chart = new QChart();
    chart->setTitle(QString("%1 Stock Analysis").arg(m_selectedStock));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeLight);
    
    // Create candlestick series
    auto candlestickSeries = new QCandlestickSeries();
    candlestickSeries->setName("Price");
    candlestickSeries->setIncreasingColor(QColor("#10b981"));
    candlestickSeries->setDecreasingColor(QColor("#ef4444"));
    
    // Generate realistic OHLC data for 30 days
    auto now = QDateTime::currentDateTime();
    double basePrice = 2000 + QRandomGenerator::global()->bounded(2000);
    
    for (int i = 30; i >= 0; --i) {
        auto timestamp = now.addDays(-i).toMSecsSinceEpoch();
        
        // Generate OHLC values
        double open = basePrice;
        double volatility = basePrice * 0.03; // 3% volatility
        double high = open + QRandomGenerator::global()->bounded(static_cast<int>(volatility));
        double low = open - QRandomGenerator::global()->bounded(static_cast<int>(volatility));
        double close = low + QRandomGenerator::global()->bounded(static_cast<int>(high - low));
        
        auto candlestickSet = new QCandlestickSet(timestamp);
        candlestickSet->setOpen(open);
        candlestickSet->setHigh(high);
        candlestickSet->setLow(low);
        candlestickSet->setClose(close);
        
        candlestickSeries->append(candlestickSet);
        basePrice = close + (QRandomGenerator::global()->bounded(200) - 100) / 10.0;
    }
    
    chart->addSeries(candlestickSeries);
    
    // Configure axes
    auto axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("MMM dd");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    candlestickSeries->attachAxis(axisX);
    
    auto axisY = new QValueAxis;
    axisY->setLabelFormat("₹%.0f");
    axisY->setTitleText("Price (₹)");
    chart->addAxis(axisY, Qt::AlignLeft);
    candlestickSeries->attachAxis(axisY);
    
    auto chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("border: 1px solid #e2e8f0; border-radius: 8px;");
    
    return chartView;
}

QWidget* MainWindow::setupTechnicalIndicators()
{
    auto indicatorsWidget = new QWidget;
    auto layout = new QHBoxLayout(indicatorsWidget);
    layout->setContentsMargins(0, 8, 0, 0);
    layout->setSpacing(16);
    
    // Technical Indicators Checkboxes
    QStringList indicators = {
        "SMA(20)", "EMA(50)", "SMA(200)", "RSI(14)", 
        "MACD", "Bollinger Bands", "Stochastic", "Volume Profile"
    };
    
    auto indicatorsLabel = new QLabel("Technical Indicators:");
    indicatorsLabel->setStyleSheet("font-weight: bold; color: #374151;");
    layout->addWidget(indicatorsLabel);
    
    for (const QString& indicator : indicators) {
        auto checkbox = new QCheckBox(indicator);
        checkbox->setStyleSheet(
            "QCheckBox { "
            "  font-size: 12px; "
            "  color: #4b5563; "
            "}"
            "QCheckBox::indicator:checked { "
            "  background-color: #3b82f6; "
            "  border: 2px solid #3b82f6; "
            "}"
        );
        layout->addWidget(checkbox);
    }
    
    layout->addStretch();
    
    // Drawing Tools
    auto drawingLabel = new QLabel("Drawing Tools:");
    drawingLabel->setStyleSheet("font-weight: bold; color: #374151;");
    layout->addWidget(drawingLabel);
    
    QStringList tools = {"✏️ Trend Line", "📐 Rectangle", "🔄 Fibonacci", "📏 Measure"};
    for (const QString& tool : tools) {
        auto toolBtn = new QPushButton(tool);
        toolBtn->setStyleSheet(
            "QPushButton { "
            "  padding: 4px 8px; "
            "  border: 1px solid #d1d5db; "
            "  border-radius: 4px; "
            "  background: white; "
            "  font-size: 11px; "
            "}"
            "QPushButton:hover { background: #f3f4f6; }"
        );
        layout->addWidget(toolBtn);
    }
    
    return indicatorsWidget;
}

QWidget* MainWindow::createDashboardCard(const QString& title, const QString& accentColor)
{
    auto card = new QWidget;
    card->setStyleSheet(
        "QWidget {"
        "  background: white;"
        "  border: 1px solid #e5e7eb;"
        "  border-radius: 12px;"
        "  border-top: 4px solid " + accentColor + ";"
        "}"
    );
    
    auto layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 16, 20, 20);
    layout->setSpacing(12);
    
    // Title with icon
    auto titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "font-size: 16px;"
        "font-weight: bold;"
        "color: #1f2937;"
        "border: none;"
        "padding: 0;"
        "margin-bottom: 8px;"
    );
    
    layout->addWidget(titleLabel);
    return card;
}

// ============== ADDITIONAL DASHBOARD COMPONENTS ==============

QWidget* MainWindow::setupQuickStatsPanel()
{
    auto statsCard = createDashboardCard("⚡ Quick Market Stats", "#7c3aed");
    auto statsLayout = qobject_cast<QVBoxLayout*>(statsCard->layout());
    
    // Market indices grid
    auto indicesGrid = new QWidget;
    auto gridLayout = new QGridLayout(indicesGrid);
    gridLayout->setSpacing(12);
    
    QStringList indices = {"NIFTY 50", "SENSEX", "NIFTY BANK", "NIFTY IT", "NIFTY AUTO", "NIFTY PHARMA"};
    QList<double> values = {19456.75, 65498.25, 43287.90, 30123.45, 15678.30, 13456.80};
    QList<double> changes = {127.35, 289.65, -156.20, 78.90, -45.60, 234.15};
    
    for (int i = 0; i < indices.size(); ++i) {
        auto indexWidget = new QWidget;
        auto indexLayout = new QVBoxLayout(indexWidget);
        indexLayout->setContentsMargins(12, 8, 12, 8);
        indexLayout->setSpacing(4);
        
        auto nameLabel = new QLabel(indices[i]);
        nameLabel->setStyleSheet("font-weight: bold; font-size: 12px; color: #374151;");
        
        auto valueLabel = new QLabel(QString("%1").arg(values[i], 0, 'f', 2));
        valueLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #1f2937;");
        
        double changePercent = (changes[i] / values[i]) * 100;
        QString changeText = QString("%1%2 (%3%4%)")
                                .arg(changes[i] >= 0 ? "+" : "")
                                .arg(changes[i], 0, 'f', 2)
                                .arg(changePercent >= 0 ? "+" : "")
                                .arg(changePercent, 0, 'f', 2);
        
        auto changeLabel = new QLabel(changeText);
        QString color = changes[i] >= 0 ? "#059669" : "#dc2626";
        changeLabel->setStyleSheet(QString("font-size: 11px; color: %1; font-weight: bold;").arg(color));
        
        indexLayout->addWidget(nameLabel);
        indexLayout->addWidget(valueLabel);
        indexLayout->addWidget(changeLabel);
        
        indexWidget->setStyleSheet(
            "QWidget { "
            "  background: #f8fafc; "
            "  border: 1px solid #e2e8f0; "
            "  border-radius: 8px; "
            "}"
            "QWidget:hover { background: #f1f5f9; }"
        );
        
        gridLayout->addWidget(indexWidget, i / 2, i % 2);
    }
    
    statsLayout->addWidget(indicesGrid);
    return statsCard;
}

QWidget* MainWindow::setupMarketOverview()
{
    auto overviewCard = createDashboardCard("🌐 Market Overview", "#0ea5e9");
    auto overviewLayout = qobject_cast<QVBoxLayout*>(overviewCard->layout());
    
    // Market status
    auto statusWidget = new QWidget;
    auto statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    
    auto statusIcon = new QLabel("🟢");
    statusIcon->setStyleSheet("font-size: 16px;");
    
    auto statusText = new QLabel("Market Open");
    statusText->setStyleSheet("font-weight: bold; color: #059669; font-size: 14px;");
    
    auto timeLabel = new QLabel("9:15 AM - 3:30 PM IST");
    timeLabel->setStyleSheet("color: #6b7280; font-size: 12px;");
    
    statusLayout->addWidget(statusIcon);
    statusLayout->addWidget(statusText);
    statusLayout->addStretch();
    statusLayout->addWidget(timeLabel);
    
    // Market stats
    auto statsWidget = new QWidget;
    auto statsLayout = new QVBoxLayout(statsWidget);
    statsLayout->setSpacing(8);
    
    QStringList statLabels = {"Advances", "Declines", "Unchanged", "Volume (Cr)"};
    QStringList statValues = {"1,247", "892", "156", "₹45,670"};
    QStringList statColors = {"#059669", "#dc2626", "#6b7280", "#3b82f6"};
    
    for (int i = 0; i < statLabels.size(); ++i) {
        auto statRow = new QWidget;
        auto rowLayout = new QHBoxLayout(statRow);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        
        auto label = new QLabel(statLabels[i] + ":");
        label->setStyleSheet("font-size: 13px; color: #4b5563;");
        
        auto value = new QLabel(statValues[i]);
        value->setStyleSheet(QString("font-size: 13px; font-weight: bold; color: %1;").arg(statColors[i]));
        
        rowLayout->addWidget(label);
        rowLayout->addStretch();
        rowLayout->addWidget(value);
        
        statsLayout->addWidget(statRow);
    }
    
    overviewLayout->addWidget(statusWidget);
    overviewLayout->addWidget(statsWidget);
    
    return overviewCard;
}

QWidget* MainWindow::setupSentimentAnalysis()
{
    auto sentimentCard = createDashboardCard("💭 Market Sentiment", "#f59e0b");
    auto sentimentLayout = qobject_cast<QVBoxLayout*>(sentimentCard->layout());
    
    // Overall sentiment gauge
    auto sentimentGauge = new QWidget;
    auto gaugeLayout = new QVBoxLayout(sentimentGauge);
    gaugeLayout->setAlignment(Qt::AlignCenter);
    
    auto sentimentValue = new QLabel("78%");
    sentimentValue->setStyleSheet(
        "font-size: 36px; "
        "font-weight: bold; "
        "color: #059669; "
        "text-align: center;"
    );
    sentimentValue->setAlignment(Qt::AlignCenter);
    
    auto sentimentLabel = new QLabel("BULLISH");
    sentimentLabel->setStyleSheet(
        "font-size: 14px; "
        "font-weight: bold; "
        "color: #059669; "
        "text-align: center;"
    );
    sentimentLabel->setAlignment(Qt::AlignCenter);
    
    // Sentiment breakdown
    auto breakdownWidget = new QWidget;
    auto breakdownLayout = new QVBoxLayout(breakdownWidget);
    breakdownLayout->setSpacing(6);
    
    QStringList sentiments = {"Bullish", "Neutral", "Bearish"};
    QList<int> percentages = {78, 15, 7};
    QStringList colors = {"#059669", "#6b7280", "#dc2626"};
    
    for (int i = 0; i < sentiments.size(); ++i) {
        auto sentimentRow = new QWidget;
        auto rowLayout = new QHBoxLayout(sentimentRow);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        
        auto label = new QLabel(sentiments[i]);
        label->setStyleSheet("font-size: 12px; color: #4b5563;");
        
        auto progressBar = new QProgressBar;
        progressBar->setRange(0, 100);
        progressBar->setValue(percentages[i]);
        progressBar->setTextVisible(false);
        progressBar->setFixedHeight(6);
        progressBar->setStyleSheet(QString(
            "QProgressBar {"
            "  border: none;"
            "  border-radius: 3px;"
            "  background-color: #f3f4f6;"
            "}"
            "QProgressBar::chunk {"
            "  background-color: %1;"
            "  border-radius: 3px;"
            "}"
        ).arg(colors[i]));
        
        auto percentage = new QLabel(QString("%1%").arg(percentages[i]));
        percentage->setStyleSheet("font-size: 12px; font-weight: bold; color: #1f2937;");
        
        rowLayout->addWidget(label);
        rowLayout->addWidget(progressBar, 1);
        rowLayout->addWidget(percentage);
        
        breakdownLayout->addWidget(sentimentRow);
    }
    
    gaugeLayout->addWidget(sentimentValue);
    gaugeLayout->addWidget(sentimentLabel);
    
    sentimentLayout->addWidget(sentimentGauge);
    sentimentLayout->addWidget(breakdownWidget);
    
    return sentimentCard;
}

QWidget* MainWindow::setupGainersLosers()
{
    auto gainersCard = createDashboardCard("📊 Top Movers", "#8b5cf6");
    auto gainersLayout = qobject_cast<QVBoxLayout*>(gainersCard->layout());
    
    // Tab widget for Gainers/Losers
    auto tabWidget = new QTabWidget;
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #e5e7eb; border-radius: 8px; }"
        "QTabBar::tab { padding: 8px 16px; margin-right: 2px; }"
        "QTabBar::tab:selected { background: #3b82f6; color: white; border-radius: 6px; }"
        "QTabBar::tab:!selected { background: #f3f4f6; color: #6b7280; border-radius: 6px; }"
    );
    
    // Top Gainers
    auto gainersWidget = new QWidget;
    auto gainersListLayout = new QVBoxLayout(gainersWidget);
    gainersListLayout->setSpacing(6);
    
    QStringList topGainers = {"ADANIPORTS", "CIPLA", "DRREDDY", "EICHERMOT", "HEROMOTOCO"};
    for (const QString& stock : topGainers) {
        auto stockRow = createMoverRow(stock, true);
        gainersListLayout->addWidget(stockRow);
    }
    
    // Top Losers
    auto losersWidget = new QWidget;
    auto losersListLayout = new QVBoxLayout(losersWidget);
    losersListLayout->setSpacing(6);
    
    QStringList topLosers = {"BAJFINANCE", "BAJAJFINSV", "DIVISLAB", "HDFC", "HDFCLIFE"};
    for (const QString& stock : topLosers) {
        auto stockRow = createMoverRow(stock, false);
        losersListLayout->addWidget(stockRow);
    }
    
    tabWidget->addTab(gainersWidget, "🟢 Gainers");
    tabWidget->addTab(losersWidget, "🔴 Losers");
    
    gainersLayout->addWidget(tabWidget);
    return gainersCard;
}

QWidget* MainWindow::createMoverRow(const QString& symbol, bool isGainer)
{
    auto widget = new QWidget;
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(8);
    
    auto symbolLabel = new QLabel(symbol);
    symbolLabel->setStyleSheet("font-weight: bold; font-size: 11px; color: #1f2937;");
    symbolLabel->setFixedWidth(80);
    
    double change = isGainer ? 
        (50 + QRandomGenerator::global()->bounded(100)) / 10.0 :
        -((50 + QRandomGenerator::global()->bounded(100)) / 10.0);
    
    QString changeText = QString("%1%2%")
                            .arg(change >= 0 ? "+" : "")
                            .arg(change, 0, 'f', 1);
    
    auto changeLabel = new QLabel(changeText);
    QString color = isGainer ? "#059669" : "#dc2626";
    changeLabel->setStyleSheet(QString("font-size: 11px; font-weight: bold; color: %1;").arg(color));
    
    layout->addWidget(symbolLabel);
    layout->addStretch();
    layout->addWidget(changeLabel);
    
    widget->setStyleSheet(
        "QWidget { "
        "  background: #f9fafb; "
        "  border-radius: 4px; "
        "  border: 1px solid #f3f4f6; "
        "}"
        "QWidget:hover { background: #f3f4f6; }"
    );
    
    return widget;
}

// ============== REMAINING DASHBOARD COMPONENTS ==============

QWidget* MainWindow::setupNewsPanel()
{
    auto newsCard = createDashboardCard("📰 Market News & Analysis", "#ef4444");
    auto newsLayout = qobject_cast<QVBoxLayout*>(newsCard->layout());
    
    // News categories filter
    auto categoriesWidget = new QWidget;
    auto categoriesLayout = new QHBoxLayout(categoriesWidget);
    categoriesLayout->setSpacing(8);
    
    QStringList categories = {"All", "Market", "Economy", "Company", "Global"};
    auto categoryGroup = new QButtonGroup(this);
    
    for (int i = 0; i < categories.size(); ++i) {
        auto button = new QPushButton(categories[i]);
        button->setCheckable(true);
        button->setStyleSheet(
            "QPushButton { "
            "  padding: 6px 12px; "
            "  border: 1px solid #d1d5db; "
            "  border-radius: 6px; "
            "  background: white; "
            "  font-size: 11px; "
            "}"
            "QPushButton:checked { "
            "  background: #3b82f6; "
            "  color: white; "
            "  border-color: #3b82f6; "
            "}"
        );
        categoryGroup->addButton(button, i);
        categoriesLayout->addWidget(button);
        
        if (i == 0) button->setChecked(true); // Default to "All"
    }
    categoriesLayout->addStretch();
    
    newsLayout->addWidget(categoriesWidget);
    
    // News items list
    auto newsListWidget = new QWidget;
    auto newsListLayout = new QVBoxLayout(newsListWidget);
    newsListLayout->setSpacing(8);
    
    QStringList headlines = {
        "Indian markets hit record high on strong GDP data",
        "RBI maintains repo rate at 6.5% amid inflation concerns",
        "Tech stocks rally as global sentiment improves",
        "Banking sector shows strong quarterly performance",
        "FII inflows support market momentum this week"
    };
    
    QStringList sources = {"Economic Times", "Business Standard", "Mint", "Financial Express", "Money Control"};
    QStringList times = {"2h ago", "4h ago", "6h ago", "8h ago", "1d ago"};
    
    for (int i = 0; i < qMin(headlines.size(), 5); ++i) {
        auto newsItem = new QWidget;
        auto itemLayout = new QVBoxLayout(newsItem);
        itemLayout->setContentsMargins(12, 8, 12, 8);
        itemLayout->setSpacing(4);
        
        auto headlineLabel = new QLabel(headlines[i]);
        headlineLabel->setStyleSheet(
            "font-size: 13px; "
            "font-weight: bold; "
            "color: #1f2937; "
            "line-height: 1.4;"
        );
        headlineLabel->setWordWrap(true);
        
        auto metaWidget = new QWidget;
        auto metaLayout = new QHBoxLayout(metaWidget);
        metaLayout->setContentsMargins(0, 0, 0, 0);
        
        auto sourceLabel = new QLabel(sources[i]);
        sourceLabel->setStyleSheet("font-size: 11px; color: #3b82f6; font-weight: 500;");
        
        auto timeLabel = new QLabel(times[i]);
        timeLabel->setStyleSheet("font-size: 11px; color: #6b7280;");
        
        auto sentimentLabel = new QLabel(i % 2 == 0 ? "📈 Bullish" : "📉 Bearish");
        sentimentLabel->setStyleSheet(QString(
            "font-size: 10px; "
            "color: %1; "
            "font-weight: bold;"
        ).arg(i % 2 == 0 ? "#059669" : "#dc2626"));
        
        metaLayout->addWidget(sourceLabel);
        metaLayout->addWidget(sentimentLabel);
        metaLayout->addStretch();
        metaLayout->addWidget(timeLabel);
        
        itemLayout->addWidget(headlineLabel);
        itemLayout->addWidget(metaWidget);
        
        newsItem->setStyleSheet(
            "QWidget { "
            "  background: #f8fafc; "
            "  border: 1px solid #e2e8f0; "
            "  border-radius: 8px; "
            "}"
            "QWidget:hover { "
            "  background: #f1f5f9; "
            "  border-color: #3b82f6; "
            "}"
        );
        newsItem->setCursor(Qt::PointingHandCursor);
        
        newsListLayout->addWidget(newsItem);
    }
    
    newsLayout->addWidget(newsListWidget);
    
    // View more button
    auto viewMoreBtn = new QPushButton("📄 View All News →");
    viewMoreBtn->setStyleSheet(
        "QPushButton { "
        "  background: transparent; "
        "  color: #3b82f6; "
        "  border: 1px solid #3b82f6; "
        "  padding: 8px 16px; "
        "  border-radius: 6px; "
        "  font-size: 12px; "
        "  font-weight: bold; "
        "}"
        "QPushButton:hover { background: #eff6ff; }"
    );
    newsLayout->addWidget(viewMoreBtn);
    
    return newsCard;
}

QWidget* MainWindow::setupDataStructuresAnalyzer()
{
    auto analyzerCard = createDashboardCard("🔍 Algorithm Performance", "#8b5cf6");
    auto analyzerLayout = qobject_cast<QVBoxLayout*>(analyzerCard->layout());
    
    // Algorithm metrics
    auto metricsGrid = new QWidget;
    auto gridLayout = new QGridLayout(metricsGrid);
    gridLayout->setSpacing(12);
    
    // Time Complexity
    auto timeWidget = new QWidget;
    auto timeLayout = new QVBoxLayout(timeWidget);
    timeLayout->setContentsMargins(12, 8, 12, 8);
    
    auto timeLabel = new QLabel("Time Complexity");
    timeLabel->setStyleSheet("font-size: 11px; color: #6b7280; font-weight: 500;");
    
    auto timeValue = new QLabel("O(n log n)");
    timeValue->setStyleSheet("font-size: 16px; font-weight: bold; color: #3b82f6; font-family: 'Courier New';");
    
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeValue);
    
    // Space Complexity
    auto spaceWidget = new QWidget;
    auto spaceLayout = new QVBoxLayout(spaceWidget);
    spaceLayout->setContentsMargins(12, 8, 12, 8);
    
    auto spaceLabel = new QLabel("Space Complexity");
    spaceLabel->setStyleSheet("font-size: 11px; color: #6b7280; font-weight: 500;");
    
    auto spaceValue = new QLabel("O(n)");
    spaceValue->setStyleSheet("font-size: 16px; font-weight: bold; color: #059669; font-family: 'Courier New';");
    
    spaceLayout->addWidget(spaceLabel);
    spaceLayout->addWidget(spaceValue);
    
    timeWidget->setStyleSheet("background: #f8fafc; border-radius: 8px; border: 1px solid #e2e8f0;");
    spaceWidget->setStyleSheet("background: #f8fafc; border-radius: 8px; border: 1px solid #e2e8f0;");
    
    gridLayout->addWidget(timeWidget, 0, 0);
    gridLayout->addWidget(spaceWidget, 0, 1);
    
    // Data Structure Info
    auto structureWidget = new QWidget;
    auto structureLayout = new QVBoxLayout(structureWidget);
    structureLayout->setContentsMargins(12, 8, 12, 8);
    
    auto structureLabel = new QLabel("Primary Structure:");
    structureLabel->setStyleSheet("font-size: 12px; color: #4b5563; font-weight: 500;");
    
    auto structureValue = new QLabel("Binary Search Tree + HashMap");
    structureValue->setStyleSheet("font-size: 12px; font-weight: bold; color: #8b5cf6;");
    
    structureLayout->addWidget(structureLabel);
    structureLayout->addWidget(structureValue);
    
    // Performance Metrics
    auto performanceLabel = new QLabel("Algorithm Efficiency:");
    performanceLabel->setStyleSheet("font-size: 12px; color: #4b5563; font-weight: 500; margin-top: 8px;");
    
    auto performanceBar = new QProgressBar;
    performanceBar->setRange(0, 100);
    performanceBar->setValue(89);
    performanceBar->setTextVisible(true);
    performanceBar->setFormat("89% Optimal");
    performanceBar->setStyleSheet(
        "QProgressBar { "
        "  border: 1px solid #d1d5db; "
        "  border-radius: 6px; "
        "  text-align: center; "
        "  font-size: 11px; "
        "  height: 24px; "
        "  background: #f3f4f6; "
        "}"
        "QProgressBar::chunk { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "             stop:0 #8b5cf6, stop:1 #6d28d9); "
        "  border-radius: 5px; "
        "}"
    );
    
    analyzerLayout->addWidget(metricsGrid);
    analyzerLayout->addWidget(structureWidget);
    analyzerLayout->addWidget(performanceLabel);
    analyzerLayout->addWidget(performanceBar);
    
    return analyzerCard;
}

QWidget* MainWindow::setupAlgorithmMonitor()
{
    auto monitorCard = createDashboardCard("⚡ Real-time Algorithm Monitor", "#10b981");
    auto monitorLayout = qobject_cast<QVBoxLayout*>(monitorCard->layout());
    
    // Algorithm status indicators
    auto statusGrid = new QWidget;
    auto statusLayout = new QGridLayout(statusGrid);
    statusLayout->setSpacing(16);
    
    struct AlgoStatus {
        QString name;
        QString status;
        QString color;
        int performance;
    };
    
    QList<AlgoStatus> algorithms = {
        {"Prediction Engine", "Active", "#10b981", 94},
        {"Risk Assessment", "Running", "#f59e0b", 87},
        {"Portfolio Optimizer", "Active", "#10b981", 92},
        {"Sentiment Analyzer", "Running", "#3b82f6", 78}
    };
    
    for (int i = 0; i < algorithms.size(); ++i) {
        auto algoWidget = new QWidget;
        auto algoLayout = new QVBoxLayout(algoWidget);
        algoLayout->setContentsMargins(12, 10, 12, 10);
        algoLayout->setSpacing(6);
        
        auto nameLabel = new QLabel(algorithms[i].name);
        nameLabel->setStyleSheet("font-size: 12px; font-weight: bold; color: #1f2937;");
        
        auto statusWidget = new QWidget;
        auto statusWidgetLayout = new QHBoxLayout(statusWidget);
        statusWidgetLayout->setContentsMargins(0, 0, 0, 0);
        
        auto statusDot = new QLabel("●");
        statusDot->setStyleSheet(QString("color: %1; font-size: 12px;").arg(algorithms[i].color));
        
        auto statusLabel = new QLabel(algorithms[i].status);
        statusLabel->setStyleSheet("font-size: 11px; color: #4b5563;");
        
        statusWidgetLayout->addWidget(statusDot);
        statusWidgetLayout->addWidget(statusLabel);
        statusWidgetLayout->addStretch();
        
        auto performanceLabel = new QLabel(QString("%1%").arg(algorithms[i].performance));
        performanceLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #1f2937;");
        
        algoLayout->addWidget(nameLabel);
        algoLayout->addWidget(statusWidget);
        algoLayout->addWidget(performanceLabel);
        
        algoWidget->setStyleSheet(
            "QWidget { "
            "  background: #f8fafc; "
            "  border: 1px solid #e2e8f0; "
            "  border-radius: 8px; "
            "  border-left: 4px solid " + algorithms[i].color + "; "
            "}"
        );
        
        statusLayout->addWidget(algoWidget, i / 2, i % 2);
    }
    
    monitorLayout->addWidget(statusGrid);
    
    // System metrics
    auto metricsWidget = new QWidget;
    auto metricsLayout = new QHBoxLayout(metricsWidget);
    metricsLayout->setSpacing(20);
    
    QStringList metricLabels = {"CPU Usage", "Memory", "Network", "Latency"};
    QStringList metricValues = {"45%", "2.1GB", "↕ 125KB/s", "12ms"};
    QStringList metricColors = {"#f59e0b", "#3b82f6", "#10b981", "#8b5cf6"};
    
    for (int i = 0; i < metricLabels.size(); ++i) {
        auto metricItem = new QWidget;
        auto itemLayout = new QVBoxLayout(metricItem);
        itemLayout->setAlignment(Qt::AlignCenter);
        
        auto valueLabel = new QLabel(metricValues[i]);
        valueLabel->setStyleSheet(QString(
            "font-size: 16px; "
            "font-weight: bold; "
            "color: %1; "
            "text-align: center;"
        ).arg(metricColors[i]));
        valueLabel->setAlignment(Qt::AlignCenter);
        
        auto labelLabel = new QLabel(metricLabels[i]);
        labelLabel->setStyleSheet("font-size: 11px; color: #6b7280; text-align: center;");
        labelLabel->setAlignment(Qt::AlignCenter);
        
        itemLayout->addWidget(valueLabel);
        itemLayout->addWidget(labelLabel);
        
        metricsLayout->addWidget(metricItem);
    }
    
    monitorLayout->addWidget(metricsWidget);
    
    return monitorCard;
}

QWidget* MainWindow::setupPerformanceMetrics()
{
    auto performanceCard = createDashboardCard("📊 Performance Analytics", "#06b6d4");
    auto performanceLayout = qobject_cast<QVBoxLayout*>(performanceCard->layout());
    
    // Portfolio performance summary
    auto summaryWidget = new QWidget;
    auto summaryLayout = new QVBoxLayout(summaryWidget);
    summaryLayout->setSpacing(8);
    
    auto totalReturnWidget = new QWidget;
    auto returnLayout = new QHBoxLayout(totalReturnWidget);
    returnLayout->setContentsMargins(0, 0, 0, 0);
    
    auto returnLabel = new QLabel("Total Return:");
    returnLabel->setStyleSheet("font-size: 13px; color: #4b5563;");
    
    auto returnValue = new QLabel("+₹47,830 (12.4%)");
    returnValue->setStyleSheet("font-size: 15px; font-weight: bold; color: #059669;");
    
    returnLayout->addWidget(returnLabel);
    returnLayout->addStretch();
    returnLayout->addWidget(returnValue);
    
    // Daily P&L
    auto dailyPnLWidget = new QWidget;
    auto dailyLayout = new QHBoxLayout(dailyPnLWidget);
    dailyLayout->setContentsMargins(0, 0, 0, 0);
    
    auto dailyLabel = new QLabel("Today's P&L:");
    dailyLabel->setStyleSheet("font-size: 13px; color: #4b5563;");
    
    auto dailyValue = new QLabel("+₹2,150 (0.8%)");
    dailyValue->setStyleSheet("font-size: 15px; font-weight: bold; color: #059669;");
    
    dailyLayout->addWidget(dailyLabel);
    dailyLayout->addStretch();
    dailyLayout->addWidget(dailyValue);
    
    summaryLayout->addWidget(totalReturnWidget);
    summaryLayout->addWidget(dailyPnLWidget);
    
    // Performance chart (mini)
    auto chartWidget = new QWidget;
    chartWidget->setFixedHeight(100);
    chartWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "           stop:0 #f0fdf4, stop:0.5 #dcfce7, stop:1 #f0fdf4); "
        "border: 1px solid #bbf7d0; "
        "border-radius: 8px;"
    );
    
    auto chartLabel = new QLabel("📈 7-Day Performance Trend");
    chartLabel->setStyleSheet("color: #059669; font-size: 11px; font-weight: 500; text-align: center;");
    chartLabel->setAlignment(Qt::AlignCenter);
    
    auto chartLayout = new QVBoxLayout(chartWidget);
    chartLayout->addWidget(chartLabel);
    
    // Risk metrics
    auto riskWidget = new QWidget;
    auto riskLayout = new QVBoxLayout(riskWidget);
    riskLayout->setSpacing(4);
    
    QStringList riskLabels = {"Sharpe Ratio", "Beta", "Max Drawdown", "VaR (95%)"};
    QStringList riskValues = {"1.42", "0.89", "-3.2%", "-₹8,450"};
    
    for (int i = 0; i < riskLabels.size(); ++i) {
        auto riskRow = new QWidget;
        auto rowLayout = new QHBoxLayout(riskRow);
        rowLayout->setContentsMargins(0, 2, 0, 2);
        
        auto label = new QLabel(riskLabels[i] + ":");
        label->setStyleSheet("font-size: 11px; color: #6b7280;");
        
        auto value = new QLabel(riskValues[i]);
        value->setStyleSheet("font-size: 11px; font-weight: bold; color: #1f2937;");
        
        rowLayout->addWidget(label);
        rowLayout->addStretch();
        rowLayout->addWidget(value);
        
        riskLayout->addWidget(riskRow);
    }
    
    performanceLayout->addWidget(summaryWidget);
    performanceLayout->addWidget(chartWidget);
    performanceLayout->addWidget(riskWidget);
    
    return performanceCard;
}

// ============== ADDITIONAL PAGES IMPLEMENTATION ==============

void MainWindow::setupWatchlistPage()
{
    auto watchlistPage = new QWidget;
    auto layout = new QVBoxLayout(watchlistPage);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);
    
    auto titleLabel = new QLabel("⭐ My Watchlist");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1f2937;");
    layout->addWidget(titleLabel);
    
    auto watchlistCard = createDashboardCard("Tracked Stocks", "#f59e0b");
    auto cardLayout = qobject_cast<QVBoxLayout*>(watchlistCard->layout());
    
    auto placeholderLabel = new QLabel("🚧 Advanced Watchlist Management\nReal-time Prices, Alerts, Custom Columns, Sorting\n(Implementation matching React Watchlist component)");
    placeholderLabel->setStyleSheet("color: #6b7280; text-align: center; padding: 60px;");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(placeholderLabel);
    
    layout->addWidget(watchlistCard);
    m_stackedWidget->addWidget(watchlistPage);
}

void MainWindow::setupPortfolioPage()
{
    auto portfolioPage = new QWidget;
    auto layout = new QVBoxLayout(portfolioPage);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);
    
    auto titleLabel = new QLabel("💼 Portfolio Management");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1f2937;");
    layout->addWidget(titleLabel);
    
    auto portfolioCard = createDashboardCard("Holdings & Performance", "#10b981");
    auto cardLayout = qobject_cast<QVBoxLayout*>(portfolioCard->layout());
    
    auto placeholderLabel = new QLabel("🚧 Complete Portfolio Analytics\nHoldings, P&L, Asset Allocation, Performance Charts\n(Implementation matching React Portfolio component)");
    placeholderLabel->setStyleSheet("color: #6b7280; text-align: center; padding: 60px;");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(placeholderLabel);
    
    layout->addWidget(portfolioCard);
    m_stackedWidget->addWidget(portfolioPage);
}

void MainWindow::setupPredictionsPage()
{
    auto predictionsPage = new QWidget;
    auto layout = new QVBoxLayout(predictionsPage);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);
    
    auto titleLabel = new QLabel("🔮 AI Predictions & Forecasts");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1f2937;");
    layout->addWidget(titleLabel);
    
    auto predictionsCard = createDashboardCard("Machine Learning Models", "#8b5cf6");
    auto cardLayout = qobject_cast<QVBoxLayout*>(predictionsCard->layout());
    
    auto placeholderLabel = new QLabel("🚧 AI-Powered Stock Predictions\nML Models, Confidence Scores, Price Targets\n(Implementation matching React Predictions component)");
    placeholderLabel->setStyleSheet("color: #6b7280; text-align: center; padding: 60px;");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(placeholderLabel);
    
    layout->addWidget(predictionsCard);
    m_stackedWidget->addWidget(predictionsPage);
}

void MainWindow::setupNewsPage()
{
    auto newsPage = new QWidget;
    auto layout = new QVBoxLayout(newsPage);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);
    
    auto titleLabel = new QLabel("📰 News & Market Sentiment");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1f2937;");
    layout->addWidget(titleLabel);
    
    auto newsCard = createDashboardCard("Market Intelligence", "#ef4444");
    auto cardLayout = qobject_cast<QVBoxLayout*>(newsCard->layout());
    
    auto placeholderLabel = new QLabel("🚧 Comprehensive News Analysis\nReal-time News, Sentiment Scores, Market Impact\n(Implementation matching React News component)");
    placeholderLabel->setStyleSheet("color: #6b7280; text-align: center; padding: 60px;");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(placeholderLabel);
    
    layout->addWidget(newsCard);
    m_stackedWidget->addWidget(newsPage);
}

void MainWindow::setupScannerPage()
{
    auto scannerPage = new QWidget;
    auto layout = new QVBoxLayout(scannerPage);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);
    
    auto titleLabel = new QLabel("🔍 Stock Scanner & Screener");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1f2937;");
    layout->addWidget(titleLabel);
    
    auto scannerCard = createDashboardCard("Market Scanner", "#06b6d4");
    auto cardLayout = qobject_cast<QVBoxLayout*>(scannerCard->layout());
    
    auto placeholderLabel = new QLabel("🚧 Advanced Stock Screening\nCustom Filters, Technical Scans, Sector Analysis\n(Implementation matching React Scanner component)");
    placeholderLabel->setStyleSheet("color: #6b7280; text-align: center; padding: 60px;");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(placeholderLabel);
    
    layout->addWidget(scannerCard);
    m_stackedWidget->addWidget(scannerPage);
}

void MainWindow::setupSettingsPage()
{
    auto settingsPage = new QWidget;
    auto layout = new QVBoxLayout(settingsPage);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);
    
    auto titleLabel = new QLabel("⚙️ Application Settings");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1f2937;");
    layout->addWidget(titleLabel);
    
    auto settingsCard = createDashboardCard("Preferences & Configuration", "#6b7280");
    auto cardLayout = qobject_cast<QVBoxLayout*>(settingsCard->layout());
    
    auto placeholderLabel = new QLabel("🚧 Comprehensive Settings Panel\nThemes, API Keys, Notifications, Data Sources\n(Implementation matching React Settings component)");
    placeholderLabel->setStyleSheet("color: #6b7280; text-align: center; padding: 60px;");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(placeholderLabel);
    
    layout->addWidget(settingsCard);
    m_stackedWidget->addWidget(settingsPage);
}

// ============== UTILITY METHODS ==============

void MainWindow::toggleTheme()
{
    m_darkTheme = !m_darkTheme;
    applyTheme();
}

void MainWindow::updateTicker()
{
    // Update ticker data in real-time
    // This would typically fetch live market data
}

void MainWindow::applyTheme()
{
    QString backgroundColor = m_darkTheme ? "#0f172a" : "#f8fafc";
    QString cardBackground = m_darkTheme ? "#1e293b" : "#ffffff";
    QString textColor = m_darkTheme ? "#f1f5f9" : "#1f2937";
    QString borderColor = m_darkTheme ? "#334155" : "#e2e8f0";
    
    setStyleSheet(QString(
        "QMainWindow { background-color: %1; }"
        "QWidget { background-color: %2; color: %3; }"
        "QStackedWidget { background-color: %1; border: none; }"
        "QScrollArea { background-color: %1; border: none; }"
        "QLabel { color: %3; border: none; background: transparent; }"
        "QLineEdit { "
        "  background: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 6px; "
        "  padding: 8px 12px; "
        "  color: %3; "
        "}"
        "QComboBox { "
        "  background: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 6px; "
        "  padding: 6px 12px; "
        "  color: %3; "
        "}"
        "QSpinBox { "
        "  background: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 6px; "
        "  padding: 6px; "
        "  color: %3; "
        "}"
    ).arg(backgroundColor, cardBackground, textColor, borderColor));
}
void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}
