// [WriteFile Name=QueryDynamicEntities, Category=Search]
// [Legal]
// Copyright 2026 Esri.
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

#ifndef FLIGHTINFOLISTMODEL_H
#define FLIGHTINFOLISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>

struct FlightInfoEntry
{
  QString trackId;
  QString flightNumber;
  QString status;
  QString arrivalAirport;
  QString aircraft;
  QString altitudeFeet;
  QString speed;
  QString heading;
};

class FlightInfoListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum FlightInfoRoles
  {
    TrackIdRole = Qt::UserRole + 1,
    FlightNumberRole,
    StatusRole,
    ArrivalAirportRole,
    AircraftRole,
    AltitudeFeetRole,
    SpeedRole,
    HeadingRole
  };

  explicit FlightInfoListModel(QObject* parent = nullptr);
  ~FlightInfoListModel() override = default;

  void setupRoles();
  void clear();
  void setAttributesForTrack(const QString& trackId, const QVariantMap& attributes);

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

private:
  int indexOfTrack(const QString& trackId) const;
  static QString attrToString(const QVariantMap& attrs, const char* key);

private:
  QHash<int, QByteArray> m_roles;
  QList<FlightInfoEntry> m_entries;
};

#endif // FLIGHTINFOLISTMODEL_H
