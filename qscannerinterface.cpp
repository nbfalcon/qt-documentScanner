#include "qscannerinterface.h"
#include <iostream>

namespace qtx {

QScannerInterface::QScannerInterface(QObject *parent)
    : QObject(parent)
    , saneWorker(new SaneWorker)
{
    saneWorker->moveToThread(saneWorker);

    QObject::connect(saneWorker, &SaneWorker::scannersRediscovered, this, &QScannerInterface::handleScannersRediscovered);

    saneWorker->start();
}

QScannerInterface::~QScannerInterface()
{
    saneWorker->quit();
}

QStringList QScannerInterface::getScannerNames()
{
    QStringList result{};
    for (const auto &device : discoveredScanners) {
        result << device.name;
    }
    return result;
}

QFuture<QImage> QScannerInterface::scan(const QString &_deviceName)
{
    auto deviceName = _deviceName;
    auto worker = saneWorker;

    QPromise<QImage> *result = new QPromise<QImage>();
    QMetaObject::invokeMethod(worker, [worker, result, deviceName](){
        worker->scan(deviceName, result);
    });

    qDebug() << "Scan queued";
    return result->future();
}

void QScannerInterface::updateScanners()
{
    QMetaObject::invokeMethod(saneWorker, &SaneWorker::updateScanners, Qt::QueuedConnection);
}

void QScannerInterface::handleScannersRediscovered(std::vector<SANE_Device> newDevices)
{
    discoveredScanners = newDevices;
    emit scannersRediscovered();
}

void SaneWorker::run()
{
    SANE_Int sane_version;
    sane_init(&sane_version, nullptr);
    qDebug().nospace()
            << "libsane v"
            << SANE_VERSION_MAJOR(sane_version) << "." << SANE_VERSION_MINOR(sane_version) << SANE_VERSION_BUILD(sane_version)
            << Qt::endl;

    QThread::exec();
    sane_exit();
}

void SaneWorker::updateScanners()
{
    std::vector<SANE_Device> newDevices{};
    const SANE_Device **devices;
    sane_get_devices(&devices, false);
    for (const SANE_Device **i = devices; *i != nullptr; i++) {
        newDevices.push_back(**i);
    }
    emit scannersRediscovered(std::move(newDevices));
}

void SaneWorker::scan(const QString &deviceName, QPromise<QImage> *result)
{
    SANE_Handle handle;
    if (sane_open(deviceName.toUtf8().constData(), &handle) != SANE_STATUS_GOOD) {
        qDebug().nospace() << "sane_open('" << deviceName << "') failed" << Qt::endl;
        return;
    }
    assert(handle != nullptr);

    SANE_Parameters params;
    sane_get_parameters(handle, &params);

    assert(sane_start(handle) == SANE_STATUS_GOOD);

    result->setProgressRange(0, params.bytes_per_line * params.lines);

    size_t totalSize = params.bytes_per_line * params.lines;
    uchar *buf = new uchar[totalSize];

    size_t readBytes = 0;
    while (readBytes < totalSize) {
        SANE_Int lastReadLength;
        SANE_Status status = sane_read(handle, buf + readBytes, totalSize - readBytes, &lastReadLength);
        if (status == SANE_STATUS_EOF) {
            break;
        }
        if (status != SANE_STATUS_GOOD) {
            qDebug().nospace() << "sane_read() failed" << Qt::endl;
            sane_close(handle);
            delete[] buf;
            return;
        }
        readBytes += lastReadLength;
        result->setProgressValue(readBytes);
        std::cout << "Progress: " << readBytes << "/" << totalSize << "\r" << std::flush;
    }

    sane_close(handle);

    QImage scannedImage(buf, params.pixels_per_line, params.lines, params.bytes_per_line, QImage::Format_RGB888, [](void *arg){
        delete[] (uchar *)arg;
    }, buf);
    result->addResult(scannedImage);

    result->finish();
    delete result;
}

} // namespace qtx
