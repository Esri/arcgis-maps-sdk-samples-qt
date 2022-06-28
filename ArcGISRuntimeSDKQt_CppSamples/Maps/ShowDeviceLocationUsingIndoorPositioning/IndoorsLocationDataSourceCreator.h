#ifndef INDOORLOCATIONDATASOURCECREATOR_H
#define INDOORLOCATIONDATASOURCECREATOR_H

#include <QUuid>
#include <QObject>

namespace Esri
{
namespace ArcGISRuntime
{
class ArcGISFeatureTable;
class FeatureTable;
class IndoorsLocationDataSource;
class Map;
}
}


class IndoorLocationDataSourceCreator : public QObject
{
  Q_OBJECT

public:
  IndoorLocationDataSourceCreator(QObject* parent = nullptr);
  ~IndoorLocationDataSourceCreator();

public slots:
  void createIndoorLocationDataSource(Esri::ArcGISRuntime::Map* map, const QString &positioningTableName, const QString &pathwaysTableName, const QStringList &globalIdSortNames);

signals:
  void createIndoorLocationDataSourceCompleted(Esri::ArcGISRuntime::IndoorsLocationDataSource* indoorLocationDataSource);

private:
  void returnIndoorLocationDataSource();
  void findPositioningTable();
  void findPathwaysTable();
  void findGlobalId();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::FeatureTable* m_positioningTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeatureTable* m_pathwaysTable = nullptr;
  QUuid m_globalId;
  QString m_positioningTableName;
  QString m_pathwaysTableName;
  QStringList m_globalIdSortNames;
};

#endif // INDOORLOCATIONDATASOURCECREATOR_H
