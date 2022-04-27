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

#include "pch.hpp"

#include <QObject>
#include <QString>

#include "SampleCategory.h"
#include "SampleListModel.h"

SampleCategory::SampleCategory(const QString& name, const QString& displayName,
                               const QString& path, const QString& thumbnailUrl,
                               const QString& backgroundThumbnailUrl,
                               SampleListModel* samples, QObject* parent /*= nullptr*/) :
  QObject(parent),
  m_name(name),
  m_displayName(displayName),
  m_path(path),
  m_thumbnail(thumbnailUrl),
  m_backgroundThumbnail(backgroundThumbnailUrl),
  m_samples(samples)
{
}
