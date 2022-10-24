#include "qmlqimageview.h"
#include <QSGSimpleTextureNode>

QmlQImageView::QmlQImageView(QQuickItem *parent) : QQuickItem(parent) {
    setFlag(QQuickItem::ItemHasContents, true);
    setSize(QSize(800, 600));
}

QSGNode *QmlQImageView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (_image.isNull()) return nullptr;

    if (!oldNode) {
        oldNode = new QSGSimpleTextureNode();
    }
    QSGSimpleTextureNode *textureNode = static_cast<QSGSimpleTextureNode *>(oldNode);
    textureNode->setRect(0, 0, width(), height());

    QSGTexture *texture = window()->createTextureFromImage(_image);
    textureNode->setTexture(texture);
    textureNode->setOwnsTexture(true);

    return textureNode;
}
