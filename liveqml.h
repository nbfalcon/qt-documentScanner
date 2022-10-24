#ifndef LIVEQML_H
#define LIVEQML_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QFileSystemWatcher>
#include <QString>
#include <QVariantMap>
#include <unordered_set>
#include <unordered_map>

namespace qtx {

class LiveQML : public QObject
{
    Q_OBJECT
public:
    explicit LiveQML(QQmlEngine *engine, QObject *parent = nullptr);

    void setInitialProperties(const QVariantMap &initialProperties) {
        this->initialProperties = initialProperties;
    }
    void load(const QString &pathRelativeToAppSrc);

signals:

private slots:
    void handleFileChanged(const QString &path);

private:
    void handleObjectCreated(QObject *object, const QUrl &url);
//    void handleObjectCreationFailed(const QUrl &url);

private:
    QQmlEngine *engine;
    QFileSystemWatcher *fileWatcher;
    std::unordered_set<QString> urlsWeAreWatching{};
    std::unordered_map<QString, QObject *> url2Object{};
    QVariantMap initialProperties{};

    void doLoad(const QUrl &theUrl);
};

}

#endif // LIVEQML_H
