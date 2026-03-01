import QtQuick 2.12
import QtQuick.Window 2.0
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import UserLogin.module 1.0

Rectangle {

    id : loginWindow
    color : "white"
    visible : true
    width : (Qt.platform.os === "ios" || Qt.platform.os === "android") ? Screen.width : 320
    height : (Qt.platform.os === "ios" || Qt.platform.os === "android") ? Screen.height : 480

    property bool isphone : Screen.width < Screen.height ? 1 : 0

    //除以得到系数
    property real multiplierH : loginWindow.height / 480
    property real multiplierW : loginWindow.width / 320
    //计算高的函数
    function dpH(numbers)
    {
        return numbers * multiplierH
    }
    //计算宽的函数
    function dpW(numbers)
    {

        return numbers * multiplierW
    }
    //平均值函数
    function dpX(numbers)
    {
        return (dpW(numbers) + dpH(numbers)) / 2
    }

    UserLogin{
        id:userLogin //UserLogin类的实例
    }
    //--------------------------------------------

    Rectangle
    {
        id : bg_rect
        anchors.fill : parent
        color : "#ecedf1"
        z : 0
    }
    /*加载动态背景图
    currentFrame，指示当前正在播放的帧序号
    frameCount，指示图片的总帧数
    paused，表示是否暂停，设置它也可以暂停或继续播放
    playing，指示动画是否在播放，默认为 true ，意思是 AnimatedImage 对象创建后立即开始播放
    source，类型为 url ，指定要播放的图片地址，可以使本地文件、 qrc 里的资源、网络文件
    */
    AnimatedImage {
    //Image {
        anchors.top: bg_rect.top
        id: animated
        source: "qrc:/images/bike.gif"
        width: parent.width
        height: parent.height
    }

    Column
    {
        width : parent.width
        height : dpH(150)  //运用
        x : 0
        y : dpH(120)
        //账号文本框
        TextField
        {
            id : username_TF
            x : 0
            y : 0
            z : 1
            anchors.left : parent.left
            anchors.right : parent.right
            anchors.leftMargin : dpW(20)
            anchors.rightMargin : dpW(20)
            width : dpW(parent.width)
            height : dpH(30)
            maximumLength: 18 //限制输入长度
            //使用正则表达式限制账号输入的字符
            validator: RegExpValidator{
                regExp: /^[0-9a-zA-Z_.@]+$/
            }
            placeholderText : "QQ号/手机号/邮箱"
            style : TextFieldStyle
            {
                //字体大小
                font.pixelSize: 60
                background : Rectangle
                {
                    color : "transparent"
                    border.color: "grey"
                    radius: dpX(6)
                }
            }

        }
        //密码文本框
        TextField
        {
            id : password_TF
            x : 0
            y : dpH(35)
            z : 1
            anchors.left : parent.left
            anchors.right : parent.right
            anchors.leftMargin : dpW(20)
            anchors.rightMargin : dpW(20)
            width : dpW(parent.width)
            height : dpH(30)
            //密码输入时显示*
            echoMode: TextField.Password
            placeholderText : "密码"
            maximumLength: 16
            //使用正则表达式限制密码输入的字符
            validator: RegExpValidator{
                regExp: /^[0-9a-zA-Z_.@]+$/
            }
            style : TextFieldStyle
            {
                //字体大小
                font.pixelSize: 55
                background : Rectangle
                {

                    color : "transparent"
                    border.color: "grey"
                    radius: dpX(6)
                }

            }
        }
        //登陆按钮
        Rectangle
        {

            id: login_button
            anchors.left : parent.left
            anchors.leftMargin : dpW(55)
            anchors.bottom : parent.bottom
            anchors.bottomMargin : dpH(45)

            color : "#1eb9f2"
            width : dpW(200)
            height : dpH(30)
            radius : dpX(6)
            border.color : "#1eb9f2"
            border.width : dpX(1)

            Text
            {
                id : login_button_text
                anchors.centerIn : parent
                color : "white"
                font.family : "微软雅黑"
                text : "登  录"
            }
            MouseArea{
                anchors.fill: parent // Edit1
                onClicked: { // this Method is not getting called
                    //console.log("用户名：" + username_TF.text)
                    //console.log("密码" + password_TF.text)
                    if(username_TF.text == "")
                    {
                        openMsgWindow("用户名不能为空！");
                    }
                    if(password_TF.text == "")
                    {
                        openMsgWindow("密码不能为空！");
                    }
                    else
                    {
                        login_button_text.text = "正在登陆..."
                        userLogin.loginToBikeClient_sig(username_TF.text, password_TF.text)
                    }
                }
            }
        }
        Text {
            id: register_text
            text: qsTr("注册账号")
            anchors.right : parent.right
            anchors.rightMargin : dpW(15)
            anchors.bottom : parent.bottom
            //anchors.bottomMargin : dpW(1)
            color: "#1768e1"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    enterToRegPage()
                }
            }
        }
    }
}
