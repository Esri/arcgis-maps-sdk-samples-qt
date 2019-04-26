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

#ifndef RELATEDFEATURE_H
#define RELATEDFEATURE_H

// Qt headers
#include <QString>

class RelatedFeature
{

public:
  explicit RelatedFeature(const QString& displayFieldName, const QString& displayFieldValue,
                          const QString& serviceLayerName);
  ~RelatedFeature() = default;

public:
  QString displayFieldName() const { return m_displayFieldName; }
  QString displayFieldValue() const { return m_displayFieldValue; }
  QString serviceLayerName() const { return m_serviceLayerName; }

private:
  QString m_displayFieldName;
  QString m_displayFieldValue;
  QString m_serviceLayerName;
};

#endif // RELATEDFEATURE_H
