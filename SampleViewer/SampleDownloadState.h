// [Legal]
// Copyright 2025 Esri.
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

#ifndef SAMPLEDOWNLOADSTATE_H
#define SAMPLEDOWNLOADSTATE_H

struct SampleDownloadState
{
  int totalItems = 0;
  int downloadedItems = 0;
  bool downloaded = false;
  bool isDownloading = false;
  double progress = 0.0;
};

#endif // SAMPLEDOWNLOADSTATE_H
