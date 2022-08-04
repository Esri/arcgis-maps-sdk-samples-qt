#include "FeatureListModel.h"

#include "Feature.h"

using namespace Esri::ArcGISRuntime;

FeatureListModel::FeatureListModel(QObject* parent /* = nullptr */) :
  QAbstractListModel(parent)
{
}

void FeatureListModel::addFeature(Feature* feature)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_features << feature;
  endInsertRows();
}

int FeatureListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_features.count();
}

QVariant FeatureListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_features.count())
    return QVariant();

  const Feature* feature = m_features[index.row()];

  if (role == FeatureOidRole)
    return feature->attributes()->attributeValue("oid");
  else if (role == FeatureTimestampRole)
    return feature->attributes()->attributeValue("collection_timestamp");

  return QVariant();
}

QHash<int, QByteArray> FeatureListModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[FeatureOidRole] = "featureOidRole";
  roles[FeatureTimestampRole] = "featureTimestampRole";
  return roles;
}

void FeatureListModel::clear()
{
  beginResetModel();
  m_features.clear();
  endResetModel();
}
