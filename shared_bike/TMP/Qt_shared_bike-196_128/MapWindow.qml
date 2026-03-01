import QtQuick 2.2
import QtQuick.Controls 1.1
import QtWebView 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.0

//ApplicationWindow{
Rectangle {
    //状态栏的加载
    /*property bool showProgress: webView.loading
                                && Qt.platform.os !== "ios"
                                && Qt.platform.os !== "winrt"   
    statusBar: StatusBar {
        id: statusBar
        visible: showProgress
        RowLayout {
            anchors.fill: parent
            Label { text: webView.loadProgress == 100 ? qsTr("Done") : qsTr("Loading: ") + webView.loadProgress + "%" }
        }
    }*/
    visible: true

    WebView {
        id: webView
        anchors.fill: parent
        url: window.mapWidonwUrl
        onLoadingChanged: {

            if (loadRequest.errorString)
                console.error(loadRequest.errorString);
        }
    }
}
