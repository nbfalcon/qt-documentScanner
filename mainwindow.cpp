#include "mainwindow.h"
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QScrollArea>
#include <QLabel>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , scannerService(new qtx::QScannerInterface)
{
    uiToolBar = new QToolBar(this);
    uiToolBar->setFloatable(false);
    uiToolBar->setMovable(false);
    uiToolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    addToolBar(uiToolBar);

    QAction *scanAction = new QAction(QIcon::fromTheme("document-scan"), tr("&Scan"), this);
    QObject::connect(scanAction, &QAction::triggered, this, &MainWindow::triggerScan);
    uiToolBar->addAction(scanAction);

    uiScannerChooser = new QComboBox(this);
    uiToolBar->addWidget(uiScannerChooser);

    statusLabel = new QLabel(this);
    statusLabel->setText(tr("Loading Scanners... Please wait"));
    setCentralWidget(statusLabel);

    QObject::connect(scannerService, &qtx::QScannerInterface::scannersRediscovered, this, &MainWindow::onScannersUpdated);

    resize(800, 600);
    scannerService->updateScanners();
}

MainWindow::~MainWindow()
{
    delete scannerService;
}

void MainWindow::triggerScan()
{
    int selectedCurrently = uiScannerChooser->currentIndex();
    if (selectedCurrently != -1) {
        const char *name = scannerService->scannerName(selectedCurrently);
        qDebug() << "Scanning from:" << name;
        auto resp = scannerService->scan(name);
    }
}

void MainWindow::onScannersUpdated()
{
    auto scanners = scannerService->getScannerNames();
    uiScannerChooser->clear();
    uiScannerChooser->addItems(scanners);

    uiScannerChooser->setEnabled(scanners.length() != 0);

    if (scanners.length() > 0) {
        statusLabel->setText(tr("Ready to Scan"));
    } else {
        statusLabel->setText(tr("No Scanners Available"));
    }
}

