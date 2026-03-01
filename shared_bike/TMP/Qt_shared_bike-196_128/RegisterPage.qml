import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import UserLogin.module 1.0

Rectangle {

    property bool isphone : Screen.width < Screen.height ? 1 : 0

    //除以得到系数
    property real multiplierH : loginWindow.height / 480
    property real multiplierW : loginWindow.width / 320
    property int count: 10
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

    MsgDialog{
        id: msgwindow
    }
    UserLogin{
        id:userLogin
        //UserLogin类的实例,有实例在，不能执行myStackview.pop()，执行后，相当于销毁掉UserLogin类，执行其析构函数，所以不能在析构函数里delete静态成员
        /*
            UserLogin::~UserLogin()
            {
                if(m_tcpClient) delete m_tcpClient;
                if(m_instance)  delete m_instance;
                if(m_instance)  delete m_instance;
                if(m_qmlObject) delete m_qmlObject;
            }*/

    }

    function openMsgWindow(msg)
    {
        msgwindow.open(msg)
    }

    id : loginWindow
    color : "white"
    visible : false
    width : (Qt.platform.os === "ios" || Qt.platform.os === "android") ? Screen.width : 320
    height : (Qt.platform.os === "ios" || Qt.platform.os === "android") ? Screen.height : 480

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
        height : dpH(200) //运用
        x : 0
        y : dpH(120)
        //账号文本框
        TextField
        {
            id : username
            x : 0
            y : 0
            z : 1
            anchors.left : parent.left
            anchors.right : parent.right
            anchors.leftMargin : dpW(20)
            anchors.rightMargin : dpW(20)
            width : dpW(parent.width)
            height : dpH(30)
            placeholderText : "QQ号/手机号/邮箱"
            maximumLength: 18
            validator: RegExpValidator{
                regExp: /^[0-9a-zA-Z_.@]+$/
            }
            style : TextFieldStyle
            {
                //字体大小
                font.pixelSize: 45
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
            id : password
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
            placeholderText : "输入密码"
            maximumLength: 16
            //使用正则表达式限制密码输入的字符
            validator: RegExpValidator{
                regExp: /^[0-9a-zA-Z_.@]+$/
            }
            style : TextFieldStyle
            {
                //字体大小
                font.pixelSize: 45
                background : Rectangle
                {

                    color : "transparent"
                    border.color: "grey"
                    radius: dpX(6)
                }

            }
        }
        //确认密码文本框
        TextField
        {
            id : confirm_password
            x : 0
            y : dpH(70)
            z : 1
            anchors.left : parent.left
            anchors.right : parent.right
            anchors.leftMargin : dpW(20)
            anchors.rightMargin : dpW(20)
            width : dpW(parent.width)
            height : dpH(30)
            //密码输入时显示*
            echoMode: TextField.Password
            placeholderText : "在此输入密码"
            maximumLength: 16
            //使用正则表达式限制密码输入的字符
            validator: RegExpValidator{
                regExp: /^[0-9a-zA-Z_.@]+$/
            }
            style : TextFieldStyle
            {
                //字体大小
                font.pixelSize: 45
                background : Rectangle
                {

                    color : "transparent"
                    border.color: "grey"
                    radius: dpX(6)
                }

            }
        }
        //验证码文本框
        TextField
        {
            id : ver_code
            x : 0
            y : dpH(105)
            z : 1
            anchors.left : parent.left
            anchors.right : parent.right
            anchors.leftMargin : dpW(20)
            anchors.rightMargin : dpW(120)
            width : dpW(parent.width)
            height : dpH(30)
            placeholderText : "再次输入验证码"
            maximumLength: 6
            validator: RegExpValidator{
                regExp: /^[0-9a-zA-Z_.@]+$/
            }
            style : TextFieldStyle
            {
                //字体大小
                font.pixelSize: 45
                background : Rectangle
                {
                    color : "transparent"
                    border.color: "grey"
                    radius: dpX(6)
                }
            }

        }
        //获取验证码按钮
        Rectangle
        {
            id: getVerCode_button
            anchors.left : parent.left
            anchors.leftMargin : dpW(205)
            anchors.bottom : parent.bottom
            anchors.bottomMargin : dpH(66)

            color : "#1eb9f2"
            width : dpW(70)
            height : dpH(30)
            radius : dpX(6)
            border.color : "#1eb9f2"
            border.width : dpX(1)

            Text
            {
                id : getVerCode_button_text
                anchors.centerIn : parent
                color : "white"
                font.family : "微软雅黑"
                text : "获取验证码"
            }
            MouseArea{
                id: btnClickArea
                anchors.fill: parent // Edit1
                onClicked: { // this Method is not getting called
                    if(username.text == "")
                    {
                        openMsgWindow("请输入用户名！");
                    }
                    else{
                        timer.start();
                        btnClickArea.enabled = false
                        userLogin.getVerificationCode_sig(username.text)
                    }

                }
            }

        }
        //注册按钮
        Rectangle
        {
            id: reg_button
            anchors.left : parent.left
            anchors.leftMargin : dpW(55)
            anchors.bottom : parent.bottom
            anchors.bottomMargin : dpH(12)

            color : "#1eb9f2"
            width : dpW(200)
            height : dpH(30)
            radius : dpX(6)
            border.color : "#1eb9f2"
            border.width : dpX(1)

            Text
            {
                id : reg_button_text
                anchors.centerIn : parent
                color : "white";
                font.family : "微软雅黑"
                text : "注    册"
            }
            MouseArea {
                anchors.fill: parent // Edit1
                onClicked: { // this Method is not getting called
                    console.log("reg_button onClicked......")
                    if(username.text == "")
                    {
                        openMsgWindow("用户名不能为空！")
                    }
                    else if(password.text == "" || confirm_password.text == "")
                    {
                        openMsgWindow("密码不能为空！")
                    }
                    else if(ver_code.text == "")
                    {
                        openMsgWindow("请输入验证码！")
                    }
                    else if(password.text != confirm_password.text)
                    {
                        openMsgWindow("两次输入的密码不一致！")
                    }
                    else
                    {
                        userLogin.registerUser_sig(username.text, password.text, ver_code.text)
                    }
                }
            }

        }
        Text {
            id: register_text
            text: qsTr("返 回")
            anchors.right : parent.right
            anchors.rightMargin : dpW(15)
            anchors.bottom : parent.bottom
            //anchors.bottomMargin : dpW(1)
            color: "#1768e1"
            MouseArea{
                anchors.fill: parent // Edit1
                onClicked: { //this Method is not getting called
                    //console.log("register_text")
                    //refreshLoginPage()
                    console.log("myStackview.depth: "+ myStackview.depth)
                    if(myStackview.depth > 1){
                        myStackview.pop()// 压栈显示登陆页面
                    }
                    else{
                        myStackview.push(loginPage)// 压栈显示登陆页面
                    }
                }
            }
        }
    }

    //定时器的调用方式，restart()、start()、stop()
    Timer {
        id: timer
        interval: 1000 //设置定时器定时时间，默认为1000ms
        repeat: true  //是否重复定时,默认为false
        running: false //是否开启定时，默认是false，当为true的时候，进入此界面就开始定时
        onTriggered: {
            if(count==10) {
                getVerCode_button.color = "#808080"
                getVerCode_button.border.color = "#808080"
                getVerCode_button_text.font.pointSize = dpW(7)
                //getVerCode_button.enabled = false
            }
            getVerCode_button_text.text = Number(count)
            count--
            if(count==0) {
                count=10
                timer.stop()
                getVerCode_button.color = "#1eb9f2"
                getVerCode_button.border.color = "#1eb9f2"
                getVerCode_button_text.font.pointSize = dpW(4)
                getVerCode_button_text.text = "获取验证码"
                btnClickArea.enabled = true
                //getVerCode_button.enabled = true
            }
        }
    }


}
