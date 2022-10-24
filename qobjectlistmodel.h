#ifndef QTX_QVARIANTLISTMODEL_H
#define QTX_QVARIANTLISTMODEL_H

#include <vector>
#include <QObject>
#include <QAbstractListModel>
#include <QJSEngine>

namespace qtx {

class QObjectListModel : public QAbstractListModel
{
    Q_OBJECT

    enum Roles {
        Item = Qt::UserRole
    };

public:
    explicit QObjectListModel(QObject *parent = nullptr);
    ~QObjectListModel();

    // @override
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Roles::Item) const override;
    QHash<int, QByteArray> roleNames() const override;

    // User API (also for QML, so bounds-checked and safe)

public slots:
    int size() { return m_objects.size(); }
    void insert(int index, QObject *object);
    void detach(int index);
    void move(int source, int target);

private:
    std::vector<QObject *> m_objects;
    bool checkIndex(int index);
};

} // namespace qtx

#endif // QTX_QVARIANTLISTMODEL_H
