// [WriteFile Name=ListRelatedFeatures, Category=Features]
// [Legal]
// Copyright 2017 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef RELATEDFEATURELISTMODEL_H
#define RELATEDFEATURELISTMODEL_H

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <RelatedFeature.h>

class RelatedFeatureListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum RelatedRoles
  {
    DisplayFieldNameRole = Qt::UserRole + 1,
    DisplayFieldValueRole,
    ServiceLayerName
  };

  explicit RelatedFeatureListModel(QObject* parent = nullptr);
  ~RelatedFeatureListModel() override = default;

  void addRelatedFeature(RelatedFeature relatedFeature);
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  void clear();
  int size() const { return m_relatedFeatures.size(); }

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QList<RelatedFeature> m_relatedFeatures;
};

#endif // RELATEDFEATURELISTMODEL_H
