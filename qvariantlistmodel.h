#ifndef QTX_QVARIANTLISTMODEL_H
#define QTX_QVARIANTLISTMODEL_H

#include <vector>
#include <QObject>
#include <QAbstractListModel>

namespace qtx {

class QVariantListModel : public QAbstractListModel
{
    Q_OBJECT

    enum Roles {
        Item = Qt::UserRole
    };

public:
    explicit QVariantListModel(QObject *parent = nullptr);

    // @override
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Roles::Item) const override;
    QHash<int, QByteArray> roleNames() const override;

    // User API stuff
public slots:
    int size() { return m_data.size(); }
    void insert(int index, QVariant datum);
    void remove(int index);
    void move(int source, int target);

private:
    std::vector<QVariant> m_data;
    bool checkIndex(int index);
};

} // namespace qtx

#endif // QTX_QVARIANTLISTMODEL_H
