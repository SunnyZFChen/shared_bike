import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Rectangle {
    id: msgWindow
    property bool isphone : Screen.width < Screen.height ? 1 : 0

    //除以得到系数
    property real multiplierH : window.height / 480;
    property real multiplierW : window.width / 320;
    //计算高的函数
    function dpH(numbers)
    {
        return numbers * multiplierH;
    }
    //计算宽的函数
    function dpW(numbers)
    {

        return numbers * multiplierW;
    }
    anchors.centerIn: parent
    //提示框内容
    property alias tipText: msg.text
    //提示框颜色
    property string backGroundColor: "white"
    property Item parentItem : Rectangle {}

    //Dialog header、contentItem、footer之间的间隔默认是12
    // 提示框的最小宽度是 100
    /*width: {
        if(msg.implicitWidth < 100 || msg.implicitWidth == 100)
            return 100;
        else
            return msg.implicitWidth > 300 ? 300 + 24 : (msg.implicitWidth + 24);
    }*/
    width: dpW(120)
    height: dpW(100)

    Dialog {
        id: dialog
        width: msgWindow.width
        height: msgWindow.height
        modal: true
        background: Rectangle {
            color: backGroundColor
            anchors.fill: parent
            radius: 5
        }
        header: Rectangle {
            width: dialog.width
            height: dpW(20)
            border.color: backGroundColor
            radius: 5
            Image {

                width: dpW(20)
                height: dpW(20)

                anchors.centerIn: parent
                source: "qrc:/images/tip.png"
            }
        }
        contentItem: Rectangle {
            border.color: backGroundColor
            color: backGroundColor
            Text {
                anchors.fill: parent
                anchors.centerIn: parent
                font.family: "Microsoft Yahei"
                color: "gray"
                text: tipText
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

            }
        }
        footer: Rectangle {
            width: dpW(40)
            height: dpW(20)
            border.color: backGroundColor
            color: backGroundColor
            radius: 5
            Button {
                anchors.centerIn: parent
                width: dpW(40)
                height: dpW(20)
                background: Rectangle {
                    anchors.centerIn: parent
                    width: 130
                    height: 60
                    radius: 5
                    border.color: "#0f748b"
                    border.width: 2
                    color: backGroundColor
                    Text {
                        anchors.centerIn: parent
                        font.family: "微软雅黑"
                        font.bold: true
                        color: "#0f748b"
                        text: "确定"
                    }
                }
                onClicked: {
                    dialog.close();
                }
            }
        }
    }

    //利用Text 的implicitWidth属性来调节提示框的大小
    //该Text的字体格式需要与contentItem中的字体一模一样
    Text {
        id: msg
        visible: false
        width: 300
        wrapMode: Text.WordWrap
        font.family: "Microsoft Yahei"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    function open(msgtext) {
        msgWindow.x = (parent.width - dialog.width) * 0.5
        msgWindow.y = (parent.height - dialog.height) * 0.5

        msg.text = msgtext
        dialog.open();
    }
}
