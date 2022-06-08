#!/bin/sh

# display help
_display_help() {
    echo
    echo "usage: ./create_sample_app_ipa.sh -l ~/applications -v cpp -q ~/Qt5.6.0/5.6/ios/bin"
    echo
    echo " -l LOCATION the location that contains your"
    echo "          output, 3rd party, Qt, etc folders"
    echo "          Required"
    echo " -v APIVERSION the API to use (qml or cpp)"
    echo "          Required"
    echo " -q QTLOCATION the bin folder for Qt"
    echo "          Required"
    echo " -b BINFOLDER the path to the release bin folder"
    echo "          Required"
    echo " -i INTERMEDIATEFOLDER the path to the intermediate output samples folder"
    echo "          Required"
    echo " -p PLIST the path to the plist file"
    echo "          Required"
    echo " -s SAMPLESPATHDIR the path to the samples folder"
    echo "          Required"
    echo
    echo " Note: Additional xcodebuild options can be added by setting XCODEBUILD_FLAGS in the build"
    echo "       environment to match Qt's behavior in xcodebuild.mk"
    echo
    exit 1
}

# variables
LOCATION=
APIVERSION=
QTLOCATION=
BINFOLDER=
INTERMEDIATEFOLDER=
PLIST=
SAMPLESPATHDIR=

# parse options
while getopts :l:v:q:b:i:p:s:h opt; do
    case $opt in
    l)
        #echo "$OPTARG"
        LOCATION=$OPTARG
        ;;
    v)
        #echo "$OPTARG"
        APIVERSION=$OPTARG
        ;;
    q)
        #echo "$OPTARG"
        QTLOCATION=$OPTARG
        ;;
    b)
        #echo "$OPTARG"
        BINFOLDER=$OPTARG
        ;;
    i)
        #echo "$OPTARG"
        INTERMEDIATEFOLDER=$OPTARG
        ;;
    p)
        #echo "$OPTARG"
        PLIST=$OPTARG
        ;;
    s)
        #echo "$OPTARG"
        SAMPLESPATHDIR=$OPTARG
        ;;
    h)
        _display_help
        ;;
    \?)
        echo "Invalid option: -$OPTARG"
        _display_help
        ;;
    esac
done

# Check if the provided location exists
if [ ! -d ${LOCATION}/output ]; then
    echo
    echo "The LOCATION specified does not exist,"
    echo "or is not a valid folder"
    _display_help
fi

# Check if the provided kit contains the androiddeployqt exe
if [ ! -f ${QTLOCATION}/qmake ]; then
    echo
    echo "The QTLOCATION path specified does not exist,"
    echo "or is not valid. Make sure this is the full path"
    echo "to the kit's bin folder (do not include the exe)"
    _display_help
fi

if [ "$APIVERSION" = "qml" ]
then
    PROJECTFOLDERNAME=ArcGISRuntimeSDKQt_QMLSamples
    PROJECTNAME=ArcGISQt_QMLSamples
else
    PROJECTFOLDERNAME=ArcGISRuntimeSDKQt_CppSamples
    PROJECTNAME=ArcGISQt_CppSamples
fi

# create variables
INTERMEDIATE_FILES=${LOCATION}/${INTERMEDIATEFOLDER}/${PROJECTNAME}
EXPORT_PLIST=${LOCATION}/${SAMPLESPATHDIR}/${PROJECTFOLDERNAME}/${PLIST}

# create itermediate directory
mkdir -p ${INTERMEDIATE_FILES}
cd ${INTERMEDIATE_FILES}

# create xcodeproj via qmake
${QTLOCATION}/qmake ${LOCATION}/${SAMPLESPATHDIR}/${PROJECTFOLDERNAME}/${PROJECTFOLDERNAME}.pro -r -spec macx-ios-clang CONFIG+=release CONFIG+=iphoneos CONFIG+=daily

# create bin folder if it does not exist
if [ ! -L ${LOCATION}/${BINFOLDER} ]; then
    mkdir ${LOCATION}/${BINFOLDER}
fi

# Archive the application
xcodebuild -scheme ${PROJECTNAME} -sdk iphoneos -archivePath "${INTERMEDIATE_FILES}/Release-iphoneos/${PROJECTNAME}.xcarchive" -configuration Release archive ENABLE_BITCODE=NO -allowProvisioningUpdates ${XCODEBUILD_FLAGS}

if [ $? != 0 ]; then
    echo "xcodebuild .xcarchive generation failed"
    exit 1
fi

# Export the archive to an ipa
xcodebuild -exportArchive -archivePath "${INTERMEDIATE_FILES}/Release-iphoneos/${PROJECTNAME}.xcarchive" -exportOptionsPlist "${EXPORT_PLIST}" -exportPath "${LOCATION}/${BINFOLDER}" ENABLE_BITCODE=NO -allowProvisioningUpdates ${XCODEBUILD_FLAGS}

if [ $? != 0 ]; then
    echo "xcodebuild .xcarchive export to ipa failed"
    exit 1
fi

echo "complete"
