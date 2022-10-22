#ifndef SCANNEDPAGE_H
#define SCANNEDPAGE_H

#include <QQuickItem>
#include <QImage>

class ScannedPage : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage)
    QML_ELEMENT

public:
    explicit ScannedPage(QQuickItem *parent = nullptr);

    QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

    const QImage &image() {
        return _image;
    }

    void setImage(const QImage &image) {
        _image = image;
        update();
    }

signals:

private:
    QImage _image;
};

#endif // SCANNEDPAGE_H
