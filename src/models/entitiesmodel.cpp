#include "entitiesmodel.h"

EntitiesModel::EntitiesModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

EntitiesModel::~EntitiesModel()
{
    qDeleteAll(m_entities.begin(), m_entities.end());
}

Entity *EntitiesModel::entityById(const QString &entityId)
{
    for (Entity *entity : m_entities) {
        if (entity->entityId() == entityId)
            return entity;
    }

    return nullptr;
}

Entity *EntitiesModel::entityAt(int index)
{
    if (index < 0 || index >= rowCount(QModelIndex()))
        return nullptr;

    return m_entities.at(index);
}

Entity *EntitiesModel::entityAt(const QModelIndex &index)
{
    if (!index.isValid())
        return nullptr;

    return m_entities.at(index.row());
}

QList<Entity *> EntitiesModel::entities() const
{
    return m_entities;
}

bool EntitiesModel::isEmpty() const
{
    return m_entities.isEmpty();
}

void EntitiesModel::addEntity(Entity *entity)
{
    if (!entity)
        return;

    beginInsertRows(QModelIndex(), m_entities.count(), m_entities.count());
    m_entities.append(entity);
    endInsertRows();

    emit changed();
}

void EntitiesModel::setEntities(const QList<Entity *> &entities)
{
    beginResetModel();
    qDeleteAll(m_entities.begin(), m_entities.end());
    m_entities.clear();
    m_entities = entities;
    endResetModel();

    emit changed();
}

void EntitiesModel::updateEntity(Entity *entity)
{
    if (!entity)
        return;

    const int idx = m_entities.indexOf(entity);

    if (idx < 0) {
        addEntity(entity);
    } else {
        const QModelIndex index = QAbstractListModel::createIndex(idx, 0, entity);
        emit dataChanged(index, index);
    }

    emit changed();
}

void EntitiesModel::reset()
{
    beginResetModel();
    qDeleteAll(m_entities.begin(), m_entities.end());
    m_entities.clear();
    endResetModel();
}

int EntitiesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_entities.count();
}

QVariant EntitiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Entity *entity = m_entities.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return entity->name();

    case NameRole:
        return entity->name();

    case AttributesRole:
        return entity->attributes();

    case ContextRole:
        return entity->context();

    case EntityIdRole:
        return entity->entityId();

    case SupportedFeaturesRole:
        return entity->supportedFeatures();

    case StateRole:
        return entity->state().toString();

    case TypeRole:
        return int(entity->type());

    default:
        return QVariant();
    }
}

QHash<int, QByteArray> EntitiesModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[AttributesRole]           = "attributes";
    roles[ContextRole]              = "context";
    roles[EntityIdRole]             = "entity_id";
    roles[SupportedFeaturesRole]    = "supported_features";
    roles[NameRole]                 = "name";
    roles[StateRole]                = "entity_state";
    roles[TypeRole]                 = "type";

    return roles;
}