#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Connect button click to function
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);

    // Menu and About action are now defined in Designer (mainwindow.ui)
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onButtonClicked()
{
    ui->pushButton->setText("Button was clicked!");
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg(this);
    dlg.exec();
}
