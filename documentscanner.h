#ifndef DOCUMENTSCANNER_H
#define DOCUMENTSCANNER_H

#include "qscannerinterface.h"
#include "qvariantlistmodel.h"
#include <QObject>
#include <QtQml>
#include <QStringList>
#include <QAbstractListModel>
#include <iostream>

class ScannedPage : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY notifyImageReady)
    Q_PROPERTY(QImage scannedPage READ scannedPage NOTIFY notifyImageReady)
    QML_ELEMENT

public:
    using QObject::QObject;

    bool ready() {
        return !scannedPageOrNull.isNull();
    }

    const QImage &scannedPage() {
        return scannedPageOrNull;
    }

    void setImageReady(const QImage &theImage) {
        scannedPageOrNull = theImage;
        notifyImageReady();
    }

private:
    QImage scannedPageOrNull;

signals:
    void notifyImageReady();
};

class DocumentScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qtx::QScannerInterface* scannerService READ getScannerService CONSTANT)

    Q_PROPERTY(bool scannersLoaded READ scannersLoaded NOTIFY scannersRefreshed)
    Q_PROPERTY(bool scannedAPage READ scannedAPage NOTIFY pageScanned)

    Q_PROPERTY(qtx::QVariantListModel* scannedPages READ getScannedPages CONSTANT)
    QML_ELEMENT

public:
    explicit DocumentScanner(QObject *parent = nullptr);

    qtx::QScannerInterface *getScannerService() { return &scannerService; }

    bool scannersLoaded() const { return _scannersLoaded; }
    bool scannedAPage() const { return _scannedAPage; }
    qtx::QVariantListModel *getScannedPages() { return &_scannedPages; }

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
    qtx::QVariantListModel _scannedPages {};
    bool _scannersLoaded = false;
    bool _scannedAPage = false;
};

#endif // DOCUMENTSCANNER_H
