#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include "qscannerinterface.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    qtx::QScannerInterface *scannerService;

    QToolBar *uiToolBar;
    QComboBox *uiScannerChooser;
    QHBoxLayout *uiScannedPages;

    QLabel *statusLabel;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void triggerScan();

private slots:
    void onScannersUpdated();
};
#endif // MAINWINDOW_H
