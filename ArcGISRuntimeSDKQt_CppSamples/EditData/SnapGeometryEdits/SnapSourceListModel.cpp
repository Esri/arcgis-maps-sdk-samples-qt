// Copyright 2024 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QList>

#include "GraphicsOverlay.h"
#include "GeometryTypes.h"
#include "FeatureTable.h"
#include "FeatureLayer.h"
#include "SnapSourceSettings.h"

#include "SnapSourceListModel.h"

using namespace Esri::ArcGISRuntime;

SnapSourceListModel::SnapSourceListModel(QObject* parent) :
    QAbstractListModel(parent)
{
    setupRoles();
}

void SnapSourceListModel::setupRoles()
{
    m_roles[SectionRole] = "section";
    m_roles[NameRole] = "name";
    m_roles[IsEnabledRole] = "isEnabled";
}

void SnapSourceListModel::enableAllLayersInSection(const QString &section, bool enabled)
{
    beginResetModel();
    for (auto* source : m_snapSourceSettings)
    {
        if (determineSection(source) == section)
        {
            source->setEnabled(enabled);
        }
    }
    endResetModel();
}

int SnapSourceListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_snapSourceSettings.count();
}

QVariant SnapSourceListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_snapSourceSettings.count())
        return QVariant();

    SnapSourceSettings* snapSourceSetting = m_snapSourceSettings[index.row()];

    QVariant retVal;
    QString section = determineSection(snapSourceSetting);
    QString name = determineName(snapSourceSetting);

    switch (role)
    {
    case SectionRole:
        retVal = section.isEmpty() ? "Unknown" : section;
        break;
    case NameRole:
        retVal = name.isEmpty() ? "Unknown Name" : name;
        break;
    case IsEnabledRole:
        retVal = snapSourceSetting->isEnabled();
        break;
    default:
        break;
    }

    return retVal;
}

bool SnapSourceListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid())
    {
        SnapSourceSettings* snapSourceSettings = m_snapSourceSettings.at(index.row());
        if (snapSourceSettings)
        {
            switch (role)
            {
            case IsEnabledRole:
            {
                if (snapSourceSettings->isEnabled() != value.toBool())
                {
                    snapSourceSettings->setEnabled(value.toBool());
                    emit dataChanged(index, index, QVector<int>() << role);
                }
                return true;
            }
            default:
                break;
            }
        }
    }

    return false;
}

QHash<int, QByteArray> SnapSourceListModel::roleNames() const
{
    return m_roles;
}

QString SnapSourceListModel::determineSection(Esri::ArcGISRuntime::SnapSourceSettings *snapSourceSettings) const
{
    // determine if it is a feature layer or graphics overlay
    FeatureLayer* featureLayer = static_cast<FeatureLayer*>(snapSourceSettings->source()); // TODO - change do dynamic_cast

    //GraphicsOverlay* graphicsOverlay = static_cast<GraphicsOverlay*>(snapSourceSetting->source()); // TODO - change do dynamic_cast

    QString section;

    if (featureLayer)
    {
        GeometryType type = featureLayer->featureTable()->geometryType();

        switch(type)
        {
        case GeometryType::Point:
            section = "Point";
            break;
        case GeometryType::Polyline:
            section = "Polyline";
            break;
        case GeometryType::Polygon:
            section = "Polygon";
            break;
        default:
            section = "Unknown";
            break;
        }
    }
    // else if (graphicsOverlay)
    // {
    //     section = "Graphics Overlay"; // TODO Update
    // }

    return section;
}

QString SnapSourceListModel::determineName(Esri::ArcGISRuntime::SnapSourceSettings *snapSourceSettings) const
{
    // determine if it is a feature layer or graphics overlay
    FeatureLayer* featureLayer = static_cast<FeatureLayer*>(snapSourceSettings->source()); // TODO - change do dynamic_cast

    //GraphicsOverlay* graphicsOverlay = static_cast<GraphicsOverlay*>(snapSourceSetting->source()); // TODO - change do dynamic_cast

    if (featureLayer)
        return featureLayer->name();
    // else if (graphicsOverlay) // TODO Add Graphics Overlay
    //     return graphcisOverlay->overlayId();
    return "";  // TODO remove
}

void SnapSourceListModel::setSnapSourceSettings(QList<Esri::ArcGISRuntime::SnapSourceSettings*> snapSourceSettings)
{
    beginResetModel();
    m_snapSourceSettings.clear();
    m_snapSourceSettings = snapSourceSettings;
    endResetModel();
}
