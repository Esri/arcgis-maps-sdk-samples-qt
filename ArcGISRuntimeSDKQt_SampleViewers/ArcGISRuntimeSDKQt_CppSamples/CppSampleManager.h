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

#ifndef CPPSAMPLEMANAGER_H
#define CPPSAMPLEMANAGER_H

#include "DownloadSampleManager.h"

#include <QObject>

Q_MOC_INCLUDE("AuthenticationManager.h")

namespace Esri
{
namespace ArcGISRuntime
{
class AuthenticationManager;
class Portal;
class PortalItem;
}
}

class CppSampleManager : public DownloadSampleManager
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authenticationManager READ authenticationManager CONSTANT)

public:
  explicit CppSampleManager(QObject* parent = nullptr);
  ~CppSampleManager() override;

  void init() override;
  Q_INVOKABLE void clearCredentialCache() override;

protected:
  QString api() const override;
  void buildCategoriesList() override;
  void loadPortal() override;
  bool isPortalLoaded() const override;
  void createPortalItem(const QString& itemId) override;
  void fetchData(const QString& outputPath) override;

private:
  Esri::ArcGISRuntime::AuthenticationManager* authenticationManager() const;

private:
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_portalItem = nullptr;
};

#endif // CPPSAMPLEMANAGER_H
