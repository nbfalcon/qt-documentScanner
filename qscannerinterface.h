#ifndef QTX_QSCANNERINTERFACE_H
#define QTX_QSCANNERINTERFACE_H

#include <QObject>
#include <QStringList>
#include <QThread>
#include <QImage>
#include <QPromise>
#include <QtQml>
#include <sane/sane.h>

namespace qtx {

class SaneWorker : public QThread {
    Q_OBJECT

public:
    void run() override;

public slots:
    void updateScanners();
    void scan(const QString &deviceName, QPromise<QImage> *result);

signals:
    void scannersRediscovered(std::vector<SANE_Device> newDevices);
};

class QScannerInterface : public QObject
{
    // FIXME: there is a use-after-free race here, with update() and using discoveredScanners meanwhile;
    Q_OBJECT
    Q_PROPERTY(QStringList scannerNames READ getScannerNames NOTIFY scannersRediscovered)
    Q_PROPERTY(int scannerCount READ scannerCount NOTIFY scannersRediscovered)
    QML_ELEMENT

public:
    explicit QScannerInterface(QObject *parent = nullptr);
    ~QScannerInterface();

    QStringList getScannerNames();
    QFuture<QImage> scan(const QString &name);
    const char *scannerName(int index) {
        return discoveredScanners[(size_t)index].name;
    }
    int scannerCount() {
        return discoveredScanners.size();
    }

public slots:
    void updateScanners();

signals:
    void scannersRediscovered();

private:
    SaneWorker *saneWorker;
    std::vector<SANE_Device> discoveredScanners;

private slots:
    void handleScannersRediscovered(std::vector<SANE_Device> newDevices);
};

} // namespace qtx

#endif // QTX_QSCANNERINTERFACE_H
