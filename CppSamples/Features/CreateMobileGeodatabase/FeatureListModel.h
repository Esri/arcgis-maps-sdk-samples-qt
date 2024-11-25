// [Legal]
// Copyright 2024 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef FEATURELISTMODEL_H
#define FEATURELISTMODEL_H

// Qt headers
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
