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

    ScannedPage *page = new ScannedPage{};
    _scannedPages.insert(_scannedPages.size(), page);

    QObject::connect(watcher, &QFutureWatcherBase::finished, [this, future, page](){
        auto image = future.result();
        qDebug() << "Scan finished";
        emit this->scanFinished(image);

        page->setImageReady(image);
    });

    _scannedAPage = true;
    emit pageScanned();
}

void DocumentScanner::save(const QString &fileName, SaveFormat format)
{
    std::vector<QImage> images;
    for (const auto *pageO : _scannedPages.asList()) {
        const auto *page = static_cast<const ScannedPage *>(pageO);
        images.push_back(page->scannedPage());
    }

    switch (format) {
    case DocumentScanner::PNG: {
            QString ext;
            QString baseFileName = fileName;
            if (fileName.endsWith(".png", Qt::CaseInsensitive)) {
                ext = fileName.last(4);
                baseFileName = fileName.first(fileName.size() - 4);
            }
            int i = 0;
            for (const auto &im : images) {
                auto urlS = QStringLiteral("%1.%2%3").arg(baseFileName, QString::number(i), ext);
                QUrl url{urlS};
                qDebug() << "Saving to:" << url.toLocalFile();
                im.save(url.toLocalFile(), "PNG");
                i++;
            }
        }
        break;
    case DocumentScanner::TIFF:
        break;
    case DocumentScanner::JPEG:
        break;
    case DocumentScanner::PDF:
        break;
    }
}

void DocumentScanner::handleScannersRediscovered()
{
    _scannersLoaded = true;
    scannersRefreshed();
}
