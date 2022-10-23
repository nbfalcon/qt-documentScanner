#include "qvariantlistmodel.h"

namespace qtx {

QVariantListModel::QVariantListModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

int QVariantListModel::rowCount(const QModelIndex &parent) const
{
    return m_data.size();
}

QVariant QVariantListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (role == Item && row >= 0 && row < (int)m_data.size()) {
        return m_data[row];
    } else {
        return QVariant();
    }
}

void QVariantListModel::insert(int index, QVariant datum)
{
    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(m_data.begin() + index, std::move(datum));
    endInsertRows();
}

void QVariantListModel::remove(int index)
{
    m_data.erase(m_data.begin() + index);
}

// FIXME: this is untested
void QVariantListModel::move(int source, int target)
{
    if (source < target) {
        // Moving forward
        auto srcElem = std::move(m_data[source]);
        // Move everything between [source+1 (which we just moved);target) one backward, creating a hole at [target]
        std::move(m_data.begin() + source + 1, m_data.begin() + target, m_data.begin() + source);
        // Fill the hole
        m_data[target] = std::move(srcElem);
    } else if (source > target) {
        // Moving backward
        auto srcElem = std::move(m_data[source]);
        // Move everything between [target;source) one forward, creating a hole at [target] and overwriting [source]
        std::move(m_data.begin() + target, m_data.begin() + source, m_data.begin() + target + 1);
        m_data[target] = std::move(srcElem);
    }
    // Not actually moving
}

QHash<int, QByteArray> QVariantListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Item] = "item";
    return roles;
}

} // namespace qtx