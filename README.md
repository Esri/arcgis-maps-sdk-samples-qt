# ArcGIS Maps SDK for Qt Samples

## Overview

The repo contains [Qt](http://qt.io) projects for each sample that can be run from within the Qt Creator IDE. It also contians source code to build and run the [C++](https://www.arcgis.com/home/search.html?t=content&q=tags%3A%22CppSampleViewer%22) and [QML](https://www.arcgis.com/home/search.html?t=content&q=tags%3A%22QmlSampleViewer%22) sample viewer apps locally.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Set up the ArcGIS Maps SDK for Qt samples repo locally](#set-up-the-arcgis-maps-sdk-for-qt-samples-repo-locally)
3. [Open a sample project file in Qt Creator](#open-a-sample-project-file-in-qt-creator)
4. [Build and run the ArcGIS Maps SDK for Qt Sample Viewers locally](#build-and-run-the-qt-maps-sdk-samples-viewer-locally)
5. [Use a previous version](#use-a-previous-version)
6. [Use offline data in the samples](#use-offline-data-in-the-samples)
7. [Resources](#resources)
8. [Issues](#issues)
9. [Contributing](#contributing)
10. [Licensing](#licensing)

## Prerequisites

* Qt SDK 5.15.2 or higher (Open source or commercial)
* Qt Creator
* An [ArcGIS Developer API key](https://developers.arcgis.com/documentation/mapping-apis-and-services/security/api-keys/)
* [ArcGIS Maps SDK for Qt Toolkit](https://github.com/Esri/arcgis-runtime-toolkit-qt) is required as some samples utilize toolkit components.
* For more information, please visit the [System Requirements](https://developers.arcgis.com/qt/reference/system-requirements/) page.

## Set up the ArcGIS Maps SDK for Qt samples repo locally

### Fork the repo

If you haven't already, fork [the repo](https://github.com/Esri/arcgis-runtime-samples-qt/fork).

### Clone the repo

[How to clone a forked repository](https://docs.github.com/en/get-started/quickstart/fork-a-repo#cloning-your-forked-repository)

#### Clone the sample code repo

Open your terminal, navigate to your working directory, use ```git clone``` to get a copy of the repo.

```bash
# Clone your fork of the repository into the current directory in terminal
git clone https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git
```

#### Clone the toolkit repo

Change directory into your locally cloned samples repo and then use `git clone` to get a copy of the [ArcGIS Maps SDK for Qt Toolkit](https://github.com/Esri/arcgis-runtime-toolkit-qt.git).

```bash
# Change directory to the clone of the samples repository
# Clone the toolkit repository into the current directory in terminal
cd /arcgis-runtime-samples-qt
git clone https://github.com/Esri/arcgis-runtime-toolkit-qt.git
```

Cloning the toolkit in this location will allow for the samples to automatically pick it up. If you wish to place the toolkit in another location, you will need to update the samples project file accordingly to locate the necessary .pri file.

### Configure remote upstream for your fork

To sync changes you make in a fork with this repository, you must configure a remote that points to the upstream repository in Git.

* Open a terminal (Mac users) or command prompt (Windows & Linux users)
* List the current configured remote repository for your fork

```bash
$ git remote -v
origin https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (fetch)
origin https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (push)
```

* Specify a new remote upstream repository

```bash
git remote add upstream https://github.com/Esri/arcgis-runtime-samples-qt.git
```

* Verify the new upstream repository

```bash
$ git remote -v

origin https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (fetch)
origin https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (push)
upstream https://github.com/Esri/arcgis-runtime-samples-qt.git (fetch)
upstream https://github.com/Esri/arcgis-runtime-samples-qt.git (push)
```

### Sync your fork

Once you have set up a remote upstream you can keep your fork up to date with our samples repository by syncing your fork.

* Open a terminal (Mac users) or command prompt (Windows & Linux users)
* Change to the current working directory of your local repository
* Fetch the branches and commits from the upstream repository. Commits to `main` will be stored in a local branch, `upstream/main`.

```bash
git fetch upstream
```

* Check out your forks local `main` branch

```bash
git checkout main
```

* Merge changes from `upstream/main` into your local `main` branch which syncs your forks `main` branch with our samples repository.

```bash
git merge upstream/main
```

## Open a sample project file in Qt Creator

Start Qt Creator. When the IDE opens to the Welcome screen, click on the **Open Project** button and browse to a project file (.pro) within your forked repo location. Configure the project, [set your ArcGIS Developer API key](https://developers.arcgis.com/documentation/mapping-apis-and-services/security/tutorials/create-and-manage-an-api-key/#set-your-api-key) in `main.cpp`, and run the sample.

## Build and run the ArcGIS Maps SDK for Qt Sample Viewers locally

Start Qt Creator. When the IDE opens to the Welcome screen, click on the **Open Project** button and browse to either sample viewer's project file (.pro) within your forked repo location. The sample viewer project files are located at `arcgis-runtime-samples-qt\ArcGISRuntimeSDKQt_SampleViewers\`, in either `ArcGISRuntimeSDKQt_CppSamples` or `ArcGISRuntimeSDKQt_QMLSamples`. Configure the project, [set your ArcGIS Developer API key](https://developers.arcgis.com/documentation/mapping-apis-and-services/security/tutorials/create-and-manage-an-api-key/#set-your-api-key) in `ArcGISRuntimeSDKQt_Samples\SampleManager.cpp`, and run the sample.

NOTE: the sample viewer project files search for the toolkit.pri at the [default location specified above](#clone-the-toolkit-repo). If you cloned the toolkit repo to a different location, you will need to update the path in the respective sample viewer's project file.

## Use a previous version

The `main` branch contains samples for the current, most up-to-date version of ArcGIS Maps SDK. To access previous versions, you can checkout by a tag or commit, or alternativley you can download the source zip:

* [100.0](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/1529)
* [100.1](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/1744)
* [100.2.1](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.2.1)
* [100.3](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/2102)
* [100.4](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.4.0.2207)
* [100.5](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.5.0.final)
* [100.6](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.6.0.final)
* [100.7](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.7.0.final)
* [100.8](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.8.0.final)
* [100.9](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.9.0.final)
* [100.10](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.10.0.final)
* [100.11](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.11.0.final)
* [100.12](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.12.0.3244.final)

## Use offline data in the samples

Some of the samples consume offline data, including TPKs, VTPKs, mobile geodatabases, and mobile map packages, among other data formats. If the sample does consume offline data, the sample's associated readme.md will note where you can download the data (from ArcGIS Online), and where the data can be placed once downloaded. For example, it may say something like:

Link | Local Location
---------|-------|
|[Los Angeles Vector Tile Package](https://www.arcgis.com/home/item.html?id=d9f8ce6f6ac84b90a665a861d71a5d0a)| `<userhome>`/ArcGIS/Runtime/Data/vtpk/LosAngeles.vtpk |

In this case, you would download the VTPK from the ArcGIS Online Location, and place it in a specified location. This location is the path returned by `QStandardPaths::writableLocation(QStandardPaths::HomeLocation)` from the Qt [QStandardPaths](https://doc.qt.io/qt-6.2/qstandardpaths.html) class. `<userhome>` is used throughout to denote a generic location on the different platforms. The below table specifies where this is on all of the supported platforms:

Platform | Userhome Location  | Example Location
|---------|--------------------|------------------|
Windows  | `%userprofile%`    | `C:\Users\Bob\ArcGIS\Runtime\Data`      |
Linux    | `~`                | `/users/bob/ArcGIS/Runtime/Data`        |
Mac OS X | `~`                | `/Users/Bob/ArcGIS/Runtime/Data`        |
Android  | `<APPROOT>/files`  | `<APPROOT>/files/ArcGIS/Runtime/Data`   |
iOS      | `<your app bundle>`| `<appbundle>/ArcGIS/Runtime/Data`       |

## Resources

* [ArcGIS Maps SDK for Qt Resource Center](https://developers.arcgis.com/qt/latest/)
* [Qt and QML](http://www.qt.io/)
* [ArcGIS Blog](http://blogs.esri.com/esri/arcgis/)
* [twitter@esri](http://twitter.com/esri)

## Issues

Find a bug or want to request a new feature? Please let us know by submitting an issue.

## Contributing

Esri welcomes contributions from anyone and everyone. Please see our [guidelines for contributing](https://github.com/esri/contributing).

## Licensing

Copyright 2020 Esri

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   <http://www.apache.org/licenses/LICENSE-2.0>

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

A copy of the license is available in the repository's [license.txt]( https://github.com/Esri/arcgis-runtime-samples-qt/blob/main/license.txt) file
