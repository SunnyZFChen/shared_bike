import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.0


ApplicationWindow {
    property bool isphone : Screen.width < Screen.height ? 1 : 0

    //除以得到系数
    property real multiplierH : window.height / 480
    property real multiplierW : window.width / 320
    property string mapWidonwUrl: ""
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
    //平均值函数
    function dpX(numbers)
    {
        return (dpW(numbers) + dpH(numbers)) / 2;
    }
    id : window;
    color : "white";
    visible : true
    width : (Qt.platform.os === "ios" || Qt.platform.os === "android") ? Screen.width : 320
    height : (Qt.platform.os === "ios" || Qt.platform.os === "android") ? Screen.height : 480

    MsgDialog{
        id:msgWindow
    }

    Component{
        id:loginPage
        LoginPage{}
    }
    Component{
        id:mapPage
        MapWindow{}
    }
    Component{
        id:registerPage
        RegisterPage{}
    }
    StackView {
        id: myStackview
        anchors.fill: parent
        initialItem: loginPage
    }

    function enterToMainPage()
    {
         mapWidonwUrl = "file:///android_asset/map.html"
         myStackview.push(mapPage)// 切换显示主页面
    }

    function enterToBikeManagePage()
    {
         mapWidonwUrl = "file:///android_asset/bikeManage.html"
         myStackview.push(mapPage)// 切换显示主页面
    }

    function returnToLoginPage()
    {
        myStackview.pop();
    }

    function enterToRegPage()
    {
        myStackview.push(registerPage)// 切换显示注册页面
    }
    function refreshLoginPage()
    {      
        myStackview.pop();
        myStackview.push(loginPage)// 切换显示主页面
    }

    function openMsgWindow(msgText)
    {
        msgWindow.open(msgText)
    }
}
