// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include <MyApplication.h>

#include <QDebug>
#include <QDesktopServices>
#include <QFileOpenEvent>

#ifdef Q_OS_ANDROID
#include <jni.h>
#endif

MyApplication::MyApplication(int &argc, char **argv, bool GUIenabled):
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    QtSingleApplication(argc, argv, GUIenabled)
{
#else
    QApplication(argc, argv)
{
  Q_UNUSED(GUIenabled)
#endif
}

MyApplication::MyApplication(const QString &id, int &argc, char **argv):
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    QtSingleApplication(id, argc, argv)
{
#else
    QApplication(argc, argv)
{
    Q_UNUSED(id)
#endif

}

MyApplication::~MyApplication()
{

}

bool MyApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen)
    {
        QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent *>(event);
        QDesktopServices::openUrl(fileEvent->url());
        return true;
    }

    return QApplication::event(event);
}

#ifdef Q_OS_ANDROID
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_esri_Launcher_openUri(JNIEnv *env,
                               jobject obj,
                               jstring uri)
{
    Q_UNUSED(obj)
    jboolean isCopy;
    isCopy = false;
    const char* utf = env->GetStringUTFChars(uri, &isCopy);
    QDesktopServices::openUrl(QUrl(QString(utf)));
    env->ReleaseStringUTFChars(uri, utf);
}

#ifdef __cplusplus
}
#endif
#endif // Q_OS_ANDROID
