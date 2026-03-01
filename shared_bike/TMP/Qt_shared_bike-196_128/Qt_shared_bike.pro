TEMPLATE = app
TARGET = minibrowser
QT += gui webchannel widgets websockets
QT += androidextras
QT += core network
QT += quick
QT += qml webview

SOURCES += main.cpp \
    UserLogin.cpp \
    bike.pb.cc \
    networkInterface.cpp \
    websocketclientwrapper.cpp \
    websockettransport.cpp

RESOURCES += qml.qrc

EXAMPLE_FILES += doc

macos:QMAKE_INFO_PLIST = macos/Info.plist
ios:QMAKE_INFO_PLIST = ios/Info.plist

target.path = $$[QT_INSTALL_EXAMPLES]/webview/minibrowser
INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/assets/bikeManage.html \
    android/assets/bike_32.ico \
    android/assets/bike_48.ico \
    android/assets/end.ico \
    android/assets/images/bike_32.ico \
    android/assets/images/bike_48.ico \
    android/assets/images/end.ico \
    android/assets/images/left-32.png \
    android/assets/images/refresh-32.png \
    android/assets/images/right-32.png \
    android/assets/images/start.ico \
    android/assets/images/stop-32.png \
    android/assets/map.html \
    android/assets/map_3.html \
    android/assets/qwebchannel.js \
    android/assets/start.ico \
    android/assets/test.html \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/values/libs.xml \
    android/src/an/qt/qtWithJava/ExtendsQtWithJava.java \
    android/src/an/qt/qtWithJava/MyJavaClass.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


android {
    #assets.files = images/map.html
    #assets.path = /assets/
    #INSTALLS += assets
    QT += webview
}

HEADERS += \
    UserLogin.h \
    bike.pb.h \
    global_def.h \
    networkInterface.h \
    websocketclientwrapper.h \
    websockettransport.h


ANDROID_EXTRA_LIBS = E:/QtProjects/protobuf_arm_3.6.0_clang/lib/libprotobuf.so

INCLUDEPATH += E:/QtProjects/protobuf_arm_3.6.0_clang/include
DEPENDPATH += E:/QtProjects/protobuf_arm_3.6.0_clang/include

unix:!macx: LIBS += -LE:/QtProjects/protobuf_arm_3.6.0_clang/lib/ -lprotobuf

#unix:!macx: LIBS += -L$$PWD/../protobuf_arm_3.6.0_clang/lib/ -lprotobuf-lite

#unix:!macx: LIBS += -L$$PWD/../protobuf_arm_3.6.0_clang/lib/ -lprotoc

FORMS +=

