#!/bin/bash

# display help
_display_help() {
    echo
    echo "usage: ./create_sample_app_apk.sh -l ~/applications -b release "
    echo "       -a ~/Qt5.15.2/5.15.2/android/bin "
    echo "       -m ~/applications/qt/sdk/samples/build-ArcGISRuntimeSDKQt_QMLSamples-Release "
    echo "       -v cpp"
    echo
    echo " -b BUILDTYPE The buildtype of the project."
    echo "          Valid buildtypes: debug, release"
    echo "          Required."
    echo " -l LOCATION the location that contains your"
    echo "          output, 3rd party, Qt, etc folders"
    echo "          Required"
    echo " -a ANDROIDDEPLOYQT the full path to the kit's"
    echo "          bin folder, where the deploy tool is"
    echo "          Required"
    echo " -d DIRECTORY the full path to the folder where to qmake and build"
    echo "          the project"
    echo "          Required"
    echo " -v APIVERSION the API to use (qml or cpp)"
    echo "          Required"
    echo " -i INTERMEDIATEFOLDER the path to the intermediate output samples folder"
    echo "          Required"
    echo " -k KEYSTORE path to the remote android release keystore file"
    echo "          Required"
    echo " -o OUTPUTFOLDER the path to the android build output folder"
    echo "          Required"
    echo " -p PASSKEY the android store passkey"
    echo "          Required"
    echo
    exit 1
}

# variables
BUILDTYPE=
LOCATION=
ANDROIDDEPLOYQT=
DIRECTORY=
APIVERSION=
INTERMEDIATEFOLDER=
KEYSTORE=
OUTPUTFOLDER=
PASSKEY=

# parse options
while getopts :l:b:a:d:v:i:k:o:p:h opt; do
    case $opt in
    l)
        #echo "$OPTARG"
        LOCATION=$OPTARG
        ;;
    b)
        #echo "$OPTARG"
        BUILDTYPE=$OPTARG
        ;;
    a)
        #echo "$OPTARG"
        ANDROIDDEPLOYQT=$OPTARG
        ;;
    d)
        #echo "$OPTARG"
        DIRECTORY=$OPTARG
        ;;
    v)
        #echo "$OPTARG"
        APIVERSION=$OPTARG
        ;;
    i)
        #echo "$OPTARG"
        INTERMEDIATEFOLDER=$OPTARG
        ;;
    k)
        #echo "$OPTARG"
        KEYSTORE=$OPTARG
        ;;
    o)
        #echo "$OPTARG"
        OUTPUTFOLDER=$OPTARG
        ;;
    p)
        #echo "$OPTARG"
        PASSKEY=$OPTARG
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



# check for valid build type
shopt -s nocasematch
case "$BUILDTYPE" in
    'debug')
      BUILDTYPE="debug"
      ;;
    'release')
      BUILDTYPE="release"
      ;;
    *)
      echo "Unknown buildtype: $BUILDTYPE"
      _display_help
esac
shopt -u nocasematch

if [ "$ANDROID_SDK_ROOT" == "" ]; then
    echo
    echo "The ANDROID_SDK_ROOT env var is empty,"
    echo "This is required"
    _display_help
fi

# Check if the provided location exists
if [ ! -d ${LOCATION}/output ]; then
    echo
    echo "The LOCATION specified does not exist,"
    echo "or is not a valid folder"
    _display_help
fi

# Check if the provided kit contains the androiddeployqt exe
if [ ! -f ${ANDROIDDEPLOYQT}/androiddeployqt ]; then
    echo
    echo "The ANDROIDDEPLOYQT path specified does not exist,"
    echo "or is not valid. Make sure this is the full path"
    echo "to the kit's bin folder (do not include the exe)"
    _display_help
fi

# Check if the provided DIRECTORY location exists
if [ ! -d ${DIRECTORY} ]; then
    mkdir -p ${DIRECTORY}
        if [ $? != 0 ]; then
        echo
        echo "The DIRECTORY path specified does not exist,"
        echo "and could not be created"
        _display_help
    fi
fi

PRO_FILE=

if [ "$APIVERSION" = "qml" ]
then
    PROJECTNAME=ArcGISQt_QMLSamples
    PRO_FILE=${LOCATION}/qt/sdk/samples/ArcGISRuntimeSDKQt_QMLSamples/ArcGISRuntimeSDKQt_QMLSamples.pro
    echo $PROJECTNAME
else
    PROJECTNAME=ArcGISQt_CppSamples
    PRO_FILE=${LOCATION}/qt/sdk/samples/ArcGISRuntimeSDKQt_CppSamples/ArcGISRuntimeSDKQt_CppSamples.pro
    echo $PROJECTNAME
fi

APPNAME=${PROJECTNAME}
INTERMEDIATE=${LOCATION}/${INTERMEDIATEFOLDER}/${APPNAME}
BUILDOUTPUT=${LOCATION}/${OUTPUTFOLDER}${BUILDTYPE}/bin
DEPLOYTOOL=${ANDROIDDEPLOYQT}/androiddeployqt
jdk=${JAVA_HOME}
INPUT=${DIRECTORY}/android-${APPNAME}-deployment-settings.json
PLATFORM=android-23
DEPLOYMENT=bundled
APKSIGNER=`find ${ANDROID_SDK_ROOT} -name apksigner -print -quit`
KEYSTORE=${KEYSTORE}

OUTPUT=${DIRECTORY}/${APPNAME}/android-build

NUMCORES=$NUMCORES
if [ "$NUMCORES" == "" ]; then
    NUMCORES=16
fi

# qmake the project
cd ${DIRECTORY}
qmake ${PRO_FILE} -spec android-clang CONFIG+=qtquickcompiler

# 'make apk' runs the androiddeployqt tool automatically, but missing many of our options so
# it will build with a different android platform
make install INSTALL_ROOT=${OUTPUT} -j $NUMCORES
${DEPLOYTOOL} --input ${INPUT} --output ${OUTPUT} --deployment ${DEPLOYMENT} --android-platform ${PLATFORM} --verbose

# locate the apk - it dumps it in a build folder
APKOUTPUT=`find ${OUTPUT} -name "*.apk"`

# cp the output APK to the output
cp ${APKOUTPUT} ${BUILDOUTPUT}/${APPNAME}.apk

# manually code sign the apk, which would normally be done via the DEPLOYTOOL step
${APKSIGNER} sign --ks ${KEYSTORE} --ks-key-alias esriqt --ks-pass ${PASSKEY} ${BUILDOUTPUT}/${APPNAME}.apk

if [ $? != 0 ]; then
    echo "apksigner failed to sign the apk - exiting"
    exit 1
fi

${APKSIGNER} verify --verbose ${BUILDOUTPUT}/${APPNAME}.apk

if [ $? != 0 ]; then
    echo "apk signature failed to verify - exiting"
    exit 1
fi

echo "--- Cleanup ---"
rm -r ${OUTPUT}
echo "complete"
