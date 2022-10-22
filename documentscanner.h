#ifndef DOCUMENTSCANNER_H
#define DOCUMENTSCANNER_H

#include "qscannerinterface.h"
#include <QObject>
#include <QtQml>
#include <QStringList>
#include <iostream>

class DocumentScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qtx::QScannerInterface* scannerService READ getScannerService CONSTANT)
    Q_PROPERTY(bool scannersLoaded READ scannersLoaded NOTIFY scannersRefreshed)
    Q_PROPERTY(bool scannedAPage READ scannedAPage NOTIFY pageScanned)
    QML_ELEMENT

public:
    explicit DocumentScanner(QObject *parent = nullptr);

    qtx::QScannerInterface *getScannerService() { return &scannerService; }

    bool scannersLoaded() { return _scannersLoaded; }
    bool scannedAPage() { return _scannedAPage; }

public slots:
    void scan(const QString &);

signals:
    void scannersChanged();
    void scannersRefreshed();
    void pageScanned();
    void scanFinished(QImage image);

private slots:
    void handleScannersRediscovered();

private:
    qtx::QScannerInterface scannerService {};
    bool _scannersLoaded = false;
    bool _scannedAPage = false;
};

#endif // DOCUMENTSCANNER_H
