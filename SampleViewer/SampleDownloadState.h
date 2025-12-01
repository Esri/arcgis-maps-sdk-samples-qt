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
