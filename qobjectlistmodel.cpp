#include "qobjectlistmodel.h"
#include <algorithm>

namespace qtx {

QObjectListModel::QObjectListModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

QObjectListModel::~QObjectListModel()
{
    for (auto *obj : m_objects) {
        delete obj;
    }
}

int QObjectListModel::rowCount(const QModelIndex &) const
{
    return m_objects.size();
}

QVariant QObjectListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (role == Item && row >= 0 && row < (int)m_objects.size()) {
        auto *obj = m_objects[row];
        return QVariant::fromValue(obj);
    } else {
        return QVariant();
    }
}

#define BOUNDSCHECK if (!checkIndex(index)) return;

void QObjectListModel::insert(int index, QObject *object)
{
    if (index != (int)m_objects.size()) BOUNDSCHECK
            object->setParent(this);
    beginInsertRows(QModelIndex(), index, index);
    m_objects.insert(m_objects.begin() + index, object);
    endInsertRows();
}

void QObjectListModel::detach(int index)
{
    BOUNDSCHECK
            beginRemoveRows(QModelIndex(), index, index);
    m_objects[index]->deleteLater();
    m_objects.erase(m_objects.begin() + index);
    endRemoveRows();
}

// FIXME: this is untested
void QObjectListModel::move(int source, int target)
{
    if (!checkIndex(source)) return;
    if (target != (int)m_objects.size() && !checkIndex(target)) return;

    // Working, simple +- 1 implementation
//    if (source == target) return;
//    if (target == source + 1) {
//        beginMoveRows(QModelIndex(), source, source, QModelIndex(), target + 1);
//        std::iter_swap(m_objects.begin() + source, m_objects.begin() + target);
//        endMoveRows();
//    } else if (target == source - 1) {
//        beginMoveRows(QModelIndex(), source, source, QModelIndex(), target);
//        std::iter_swap(m_objects.begin() + source, m_objects.begin() + target);
//        endMoveRows();
//    }

        if (source < target) {
            beginMoveRows(QModelIndex(), source, source, QModelIndex(), target + 1);
            // Moving forward
            auto srcElem = std::move(m_objects[source]);
            // Move everything between [source+1 (which we just moved);target) one backward, creating a hole at [target]
            std::move(m_objects.begin() + source + 1, m_objects.begin() + target, m_objects.begin() + source);
            // Fill the hole
            m_objects[target] = std::move(srcElem);
            endMoveRows();
        } else if (source > target) {
            beginMoveRows(QModelIndex(), source, source, QModelIndex(), target);
            // Moving backward
            auto srcElem = std::move(m_objects[source]);
            // Move everything between [target;source) one forward, creating a hole at [target] and overwriting [source]
            std::move(m_objects.begin() + target, m_objects.begin() + source, m_objects.begin() + target + 1);
            m_objects[target] = std::move(srcElem);
            endMoveRows();
        }
        // else: we don't have to move anything
}

bool QObjectListModel::checkIndex(int index)
{
    if (index >= 0 && index < (int)m_objects.size()) return true;
    qCritical() << "qtx::QVariantListModel: index out of bounds:" << index;
    return false;
}

QHash<int, QByteArray> QObjectListModel::roleNames() const
{
    return QHash<int, QByteArray>{{Item, "item"}};
}

} // namespace qtx
