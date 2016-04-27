# Overview
ArcGIS Runtime SDK 100.0 for Qt (Quartz) samples for Qt Creator.  The repo contains [Qt Creator](http://qt.io) projects for each sample that can be run from within the Qt Creator IDE.

# Prerequisites
* Qt SDK 5.6 or higher (Open source oe commercial)
* Qt Creator
* For Windows: Microsoft Visual C++ Compiler 14.0 
* For more information, please visit the [System Requirements](https://developers.arcgis.com/qt/qml/guide/arcgis-runtime-sdk-for-qt-system-requirements.htm) page.

## Fork the repo
If you haven't already, fork [the repo](https://github.com/Esri/arcgis-runtime-samples-qt/fork).

## Clone the repo

### Command line Git
[Clone the ArcGIS Qt SDK Samples](https://help.github.com/articles/fork-a-repo#step-2-clone-your-fork)

Open your terminal, navigate to your working directory, use ```git clone``` to get a copy of the repo.

```
# Clones your fork of the repository into the current directory in terminal
$ git clone https://github.com/YOUR-USERNAME/arcgis-runtime-samples-qt.git
```

## Configure remote upstream for your fork
To sync changes you make in a fork with this repository, you must configure a remote that points to the upstream repository in Git.

- Open a terminal (Mac users) or command prompt (Windows & Linux users)
- List the current configured remote repository for your fork

```
$ git remote -v
origin	https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (fetch)
origin	https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (push)
```

- Specify a new remote upstream repository

```
$ git remote add upstream https://github.com/Esri/arcgis-runtime-samples-qt.git
```

- Verify the new upstream repository

```
$ git remote -v

origin	https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (fetch)
origin	https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (push)
upstream https://github.com/Esri/arcgis-runtime-samples-qt.git (fetch)
upstream https://github.com/Esri/arcgis-runtime-samples-qt.git (push)
```

### Sync your fork
Once you have set up a remote upstream you can keep your fork up to date with our samples repository by syncing your fork.

- Open a terminal (Mac users) or command prompt (Windows & Linux users)
- Change to the current working directory of your local repository
- Fetch the branches and commits from the upstream repository.  Commits to ```master``` will be stored in a local branch, ```upstream/master```.

```
$ git fetch upstream
```

- Check out your forks local ```master``` branch

```
$ git checkout master
```

- Merge changes from ```upstream/master``` into  your local ```master``` branch which syncs your forks ```master``` branch with our samples repository.

```
$ git merge upstream/master
```
## Open a sample project file in Qt Creator
Start Qt Creator. When the IDE opens to the Welcome screen, click on the **Open Project** button and browse to a project file (.pro) within your forked repo location.
Configure the project and run the sample.

