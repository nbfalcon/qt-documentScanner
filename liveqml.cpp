#include "liveqml.h"
#include <QFileInfo>
#include <QDir>
#include <QWindow>
#include <QQmlComponent>
#include <QThread>

namespace qtx {

LiveQML::LiveQML(QQmlEngine *engine, QObject *parent)
    : QObject{parent}, engine(engine), fileWatcher(new QFileSystemWatcher(this))
{
    QObject::connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &LiveQML::handleFileChanged);
}

void LiveQML::load(const QString &path)
{
    QDir base = QFileInfo(__FILE__).dir();
    auto qmlFile = base.absoluteFilePath(path);
    auto url = "file://" + qmlFile;

    qDebug() << "(LIVEQML) Loading (first time):" << url;
    urlsWeAreWatching.insert(url);
    fileWatcher->addPath(qmlFile);
    doLoad(url);
}

void LiveQML::handleFileChanged(const QString &qmlFile)
{
    // Editors first delete the file, then write it (QtCreator)
    // https://stackoverflow.com/a/30076119
    if (!fileWatcher->files().contains(qmlFile)) {
        QThread::msleep(1);
        fileWatcher->addPath(qmlFile);
    }

    auto url = "file://" + qmlFile;
    if (urlsWeAreWatching.find(url) != urlsWeAreWatching.end()) {
        qDebug() << "(LIVEQML) Reloading (file changed):" << url;
        doLoad(url);
    }
}

void LiveQML::handleObjectCreated(QObject *object, const QUrl &url)
{
    auto urlAsStr = url.toString();
    if (urlsWeAreWatching.find(urlAsStr) != urlsWeAreWatching.end()) {
        if (object != nullptr) {
            auto alreadyExists = url2Object.find(urlAsStr);
            if (alreadyExists != url2Object.end()) {
                auto *oldObjectIsAWindow = qobject_cast<QWindow*>(alreadyExists->second);
                if (oldObjectIsAWindow) {
                    auto *newObjectIsAlsoAWindow = qobject_cast<QWindow*>(object);
                    if (newObjectIsAlsoAWindow) {
                        newObjectIsAlsoAWindow->setScreen(oldObjectIsAWindow->screen());
                        newObjectIsAlsoAWindow->setGeometry(oldObjectIsAWindow->geometry());
                    }

                    qDebug() << "(LIVEQML) Shutting down:" << urlAsStr;
                    oldObjectIsAWindow->close();
                    oldObjectIsAWindow->deleteLater();
                }
                qDebug() << "(LIVEQML) Successfully reloaded:" << urlAsStr;
            } else {
                qDebug() << "(LIVEQML) Successfully loaded (first time):" << urlAsStr;
            }
            url2Object[urlAsStr] = object;
        } else {
            qDebug() << "(LIVEQML) Failed to reload:" << urlAsStr;
        }
        qDebug().noquote().nospace() << "\n";
    }
    // Else something went wrong
}

void LiveQML::doLoad(const QUrl &theUrl)
{
    assert(theUrl.isLocalFile());
    QFile asFile(theUrl.toLocalFile());
    asFile.open(QFile::ReadOnly);
    auto bytes = asFile.readAll();
    // FIXME: errors?

    QQmlComponent componentToBeLoaded(engine);
    componentToBeLoaded.setData(bytes, theUrl);
    QObject *component = nullptr;
    if (!componentToBeLoaded.isError()) {
        component = componentToBeLoaded.createWithInitialProperties(initialProperties);
    }
    handleObjectCreated(component, theUrl);
}

//void LiveQML::handleObjectCreationFailed(const QUrl &url)
//{
//    auto urlAsStr = url.toString();
//    if (urlsWeAreWatching.find(urlAsStr) != urlsWeAreWatching.end()) {
//        qDebug() << "(LIVEQML) Failed to load:" << url;
//    }
//}

}
