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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include <QString>

#include "RelatedFeature.h"

RelatedFeature::RelatedFeature(const QString& displayFieldName, const QString& displayFieldValue,
                               const QString& serviceLayerName) :
  m_displayFieldName(displayFieldName),
  m_displayFieldValue(displayFieldValue),
  m_serviceLayerName(serviceLayerName)
{
}
