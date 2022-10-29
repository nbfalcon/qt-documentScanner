#include "qscannerinterface.h"
#include <string.h>
#include <sane/sane.h>
#include <sane/saneopts.h>
#include <algorithm>
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

struct FoundOption {
    int i = -1;
    const SANE_Option_Descriptor *desc = nullptr;
};

const FoundOption find_descriptor(SANE_Handle handle, SANE_String_Const option_name) {
    for (int i = 0; true; i++) {
        const auto *descriptor = sane_get_option_descriptor(handle, i);
        if (descriptor == nullptr) break;
        if (strcmp(descriptor->name, option_name) == 0) {
            return FoundOption{.i = i, .desc = descriptor};
        }
    }
    return FoundOption();
}

template<class T>
bool sanex_setopt(SANE_Handle handle, SANE_String_Const option_name, T value);

template<>
bool sanex_setopt(SANE_Handle handle, SANE_String_Const option_name, int value) {
    auto desc_s = find_descriptor(handle, option_name);
    int option_i = desc_s.i;
    const auto *desc = desc_s.desc;
    if (!desc) return false;
    assert(desc->type == SANE_TYPE_INT);

    switch (desc->constraint_type) {
    case SANE_CONSTRAINT_NONE:
        break;
    case SANE_CONSTRAINT_RANGE:
        value = std::clamp(value, desc->constraint.range->min, desc->constraint.range->max);
        break;
    case SANE_CONSTRAINT_WORD_LIST: {
        int count = desc->constraint.word_list[0];
        int distance = INT_MAX;
        for (int i = 0; i < count; i++) {
            int word = desc->constraint.word_list[i + 1];
            int d = std::abs(value - word);
            if (d < distance) {
                distance = d;
                value = word;
            }
        }
        }
        break;
    case SANE_CONSTRAINT_STRING_LIST:
        // Not actually possible
        break;
    }

    if (sane_control_option(handle, option_i, SANE_ACTION_SET_VALUE, &value, nullptr) != SANE_STATUS_GOOD) return false;
    return true;
}

void SaneWorker::scan(const QString &deviceName, QPromise<QImage> *result)
{
    // this image is mocked
//    QImage mock = QImage{800, 600, QImage::Format_RGB888};
//    int r = std::rand() % 256, g = std::rand() % 256, b = std::rand() % 256;
//    mock.fill(QColor::fromRgb(r, g, b));
//    result->addResult(std::move(mock));
//    result->finish();
//    delete result;
//    return;

    SANE_Handle handle;
    if (sane_open(deviceName.toUtf8().constData(), &handle) != SANE_STATUS_GOOD) {
        qDebug().nospace() << "sane_open('" << deviceName << "') failed" << Qt::endl;
        return;
    }
    assert(handle != nullptr);

    if (sanex_setopt(handle, SANE_NAME_SCAN_X_RESOLUTION, 300)) {
        sanex_setopt(handle, SANE_NAME_SCAN_Y_RESOLUTION, 300);
    } else {
        sanex_setopt(handle, SANE_NAME_SCAN_RESOLUTION, 300);
    }

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

    QImage scannedImage(buf, params.pixels_per_line, params.lines, params.bytes_per_line, QImage::Format_RGB888, [](void *arg){
        delete[] (uchar *)arg;
    }, buf);
    result->addResult(scannedImage);
    result->finish();

    sane_close(handle);
    delete result;
}

} // namespace qtx
