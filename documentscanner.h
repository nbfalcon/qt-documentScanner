#ifndef DOCUMENTSCANNER_H
#define DOCUMENTSCANNER_H

#include "qscannerinterface.h"
#include "qobjectlistmodel.h"
#include <QObject>
#include <QtQml>
#include <QStringList>
#include <QAbstractListModel>

class ScannedPage : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY notifyImageReady)
    Q_PROPERTY(QImage scannedPage READ scannedPage NOTIFY notifyImageReady)
    QML_ELEMENT

public:
    using QObject::QObject;
    ~ScannedPage() {
        qDebug() << "delete ScannedPage";
    }

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

    Q_PROPERTY(qtx::QObjectListModel* scannedPages READ getScannedPages CONSTANT)
    QML_ELEMENT

public:
    explicit DocumentScanner(QObject *parent = nullptr);

    qtx::QScannerInterface *getScannerService() { return &scannerService; }

    bool scannersLoaded() const { return _scannersLoaded; }
    bool scannedAPage() const { return _scannedAPage; }
    qtx::QObjectListModel *getScannedPages() { return &_scannedPages; }

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
    qtx::QObjectListModel _scannedPages;
    bool _scannersLoaded = false;
    bool _scannedAPage = false;
};

#endif // DOCUMENTSCANNER_H
