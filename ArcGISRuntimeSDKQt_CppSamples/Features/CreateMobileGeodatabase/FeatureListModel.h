#ifndef FEATURELISTMODEL_H
#define FEATURELISTMODEL_H

#include <QAbstractListModel>

namespace Esri::ArcGISRuntime
{
class Feature;
}

class FeatureListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum SampleFeatureRoles
  {
    FeatureOidRole,
    FeatureTimestampRole
  };

  explicit FeatureListModel(QObject* parent = nullptr);
  ~FeatureListModel() override = default;

  void addFeature(Esri::ArcGISRuntime::Feature* feature);
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  void clear();
  int size() const { return m_features.size(); }

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QList<Esri::ArcGISRuntime::Feature*> m_features;
};

#endif // FEATURELISTMODEL_H
