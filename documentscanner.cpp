#include "documentscanner.h"

DocumentScanner::DocumentScanner(QObject *parent) : QObject(parent)
{
    QObject::connect(&scannerService, &qtx::QScannerInterface::scannersRediscovered, this, [this](){
        emit scannersChanged();
    });
    QObject::connect(&scannerService, &qtx::QScannerInterface::scannersRediscovered, this, &DocumentScanner::handleScannersRediscovered);
    scannerService.updateScanners();
}

void DocumentScanner::scan(const QString &name)
{
    auto future = scannerService.scan(name.toUtf8().constData());
    auto *watcher = new QFutureWatcher<QImage>(this);
    watcher->setFuture(future);

    ScannedPage *page = new ScannedPage(&_scannedPages);
    _scannedPages.insert(_scannedPages.size(), QVariant::fromValue(page));

    QObject::connect(watcher, &QFutureWatcherBase::finished, [this, future, page](){
        auto image = future.result();
        qDebug() << "Scan finished";
        emit this->scanFinished(image);

        page->setImageReady(image);
    });

    _scannedAPage = true;
    pageScanned();
}

void DocumentScanner::handleScannersRediscovered()
{
    _scannersLoaded = true;
    scannersRefreshed();
}
