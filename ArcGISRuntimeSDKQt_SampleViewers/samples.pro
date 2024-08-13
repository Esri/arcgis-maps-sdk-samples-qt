mac {
    cache()
}

TEMPLATE = subdirs

ios {
    QMAKE_IOS_DEPLOYMENT_TARGET = 16.0
}

SUBDIRS += \
    $$PWD/ArcGISRuntimeSDKQt_QMLSamples \
    $$PWD/ArcGISRuntimeSDKQt_CppSamples
