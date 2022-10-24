#ifndef QMLQIMAGEVIEW_H
#define QMLQIMAGEVIEW_H

#include <QQuickItem>
#include <QImage>

class QmlQImageView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage RESET setNullImage)
    QML_ELEMENT

public:
    explicit QmlQImageView(QQuickItem *parent = nullptr);

    QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

    const QImage &image() { return _image; }
    void setImage(const QImage &image);
    void setNullImage();

private:
    QImage _image;
};

#endif // QMLQIMAGEVIEW_H
