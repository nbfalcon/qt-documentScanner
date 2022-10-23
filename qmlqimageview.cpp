#include "qmlqimageview.h"
#include <QSGSimpleTextureNode>
#include <iostream>

QmlQImageView::QmlQImageView(QQuickItem *parent) : QQuickItem(parent) {
    setFlag(QQuickItem::ItemHasContents, true);
    setSize(QSize(800, 600));
}

QSGNode *QmlQImageView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (!oldNode) {
        oldNode = new QSGSimpleTextureNode();
    }
    std::cout << "updatePaintNode(" << width() << "; " << height() << ")" << std::endl;
    QSGSimpleTextureNode *textureNode = static_cast<QSGSimpleTextureNode *>(oldNode);
    textureNode->setRect(0, 0, width(), height());

    QSGTexture *texture = window()->createTextureFromImage(_image);
    textureNode->setTexture(texture);
    textureNode->setOwnsTexture(true);

    return textureNode;
}
