// [Legal]
// Copyright 2022 Esri.

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

#ifndef SAMPLESEARCHFILTERMODEL_H
#define SAMPLESEARCHFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <SearchFilterCriteria.h>

class SearchFilterCriteria;

/*!
 * \brief The SampleSearchFilterModel class is a simple proxy model that goes
          around anther model (assumed to be a list model.) Where a given
          \c{SearchFilterCriteria} object is used to filter the model.
 */
class SampleSearchFilterModel : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
  Q_PROPERTY(int blurbRole READ blurbRole NOTIFY roleIdsChanged)
  Q_PROPERTY(int scoreRole READ scoreRole NOTIFY roleIdsChanged)

public:
  SampleSearchFilterModel(QObject* parent = nullptr);

  SampleSearchFilterModel(SearchFilterCriteria* searchCriteria, QObject* parent = nullptr);

  ~SampleSearchFilterModel() override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  /*!
   * \brief filterString
   * \return returns the string that is currently used to filter the samples.
   */
  QString filterString() const;

  /*!
   * \brief setFilterString
   * \param ilterString sets the filter string
   */
  void setFilterString(QString filterString);

  SearchFilterCriteria* searchFilterCriteria() const;

  void setSearchFilterCriteria(SearchFilterCriteria* searchCriteria);

  /*!
   * \brief scoreRole. The dynamically chosen role to append a score to the
   *        source model. This value can change if the source model is changed.
   * \return current id for role.
   */
  int scoreRole() const;

  /*!
   * \brief blurbRole The dynamically chosen role to append a score to the
   *        source model. This value can change if the source model is changed.
   * \return current id for role.
   */
  int blurbRole() const;

protected:
  bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

  QHash<int, QByteArray> roleNames() const override;

signals:
  void filterStringChanged();
  void roleIdsChanged();

private slots:
  void discoverUniqueRoleIdForRoles();

private:
  QList<int> unusedRoleIds(int count) const;
  void updateFilterResults();

private:
  QString m_filterString;
  SearchFilterCriteria* m_searchCriteria = nullptr;

  QMap<int, SearchResult > m_filterResults;

  int m_blurbRole { Qt::UserRole };
  int m_scoreRole { Qt::UserRole + 1};
};

#endif // SAMPLESEARCHFILTERMODEL_H
