package an.qt.qtWithJava;
import java.io.OutputStream;
import java.util.Iterator;
import java.io.File;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.NotificationChannel;
import android.graphics.Color;

import android.app.Activity;
import android.content.Intent;
import android.content.ContentValues;
import android.content.res.Configuration;
import android.hardware.Camera;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.KeyEvent;
import android.widget.LinearLayout;
import android.provider.MediaStore;
import android.net.Uri;
import android.os.Environment;
import android.content.pm.PackageManager;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Parcelable;
import android.text.TextUtils;
import android.view.Window;
import android.view.WindowManager;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.app.NotificationCompat;
//import androidx.annotation.RequiresApi;
//import androidx.core.app.ActivityCompat;
import com.yzq.zxinglibrary.android.CaptureActivity;
import com.yzq.zxinglibrary.common.Constant;
import com.yzq.zxinglibrary.bean.ZxingConfig;


public class MyJavaClass extends org.qtproject.qt5.android.bindings.QtActivity //implements ActivityCompat.OnRequestPermissionsResultCallback
{
    private static NotificationManager m_notificationManager;
    private static MyJavaClass m_instance;
    //java调用QT的方法
    public static native void SendDataToQt(String data);

    private static int num;

    private static final int REQUEST_CODE_SCAN = 1;
     private static int REQUEST_PERMISSION_CODE = 3;

    public static String[] permissions = new String[]{
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_COARSE_LOCATION
    };

    public MyJavaClass()
    {
        m_instance = this;
    }

    public static MyJavaClass getInstance()
    {
        return m_instance;
    }
    //请求权限
    public static void getPermission()
    {
        for (int i = 0 ; i < permissions.length ; i++)
        {
            if(ActivityCompat.checkSelfPermission(m_instance,permissions[i])!= PackageManager.PERMISSION_GRANTED)
            {
                ActivityCompat.requestPermissions(m_instance, permissions, REQUEST_PERMISSION_CODE);
            }

        }
    }
    public static void choosePhoto(){
        Intent intentToPickPic = new Intent(Intent.ACTION_PICK, null);
        // 如果限制上传到服务器的图片类型时可以直接写如："image/jpeg 、 image/png等的类型" 所有类型则写 "image/*"
        intentToPickPic.setDataAndType(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, "image/jpeg");
        m_instance.startActivityForResult(intentToPickPic, 1);
    }

    public static void openSetting(String action){
        //Intent intent = new Intent(action);
        //intent.setAction(android.settings.WIRELESS_SETTINGS);
        m_instance.startActivity(new Intent(action)) ;
    }

    public static void scanQRCode(String action) {

        Intent intent = new Intent(m_instance, CaptureActivity.class);
        /*ZxingConfig是配置类  可以设置是否显示底部布局，闪光灯，相册，是否播放提示音  震动等动能
         * 也可以不传这个参数
         * 不传的话  默认都为默认不震动  其他都为true
         * */
        getPermission();
        ZxingConfig config = new ZxingConfig();
        config.setShowbottomLayout(true);//底部布局（包括闪光灯和相册）
        config.setPlayBeep(true);//是否播放提示音
        config.setShake(true);//是否震动
        config.setShowAlbum(true);//是否显示相册
        config.setShowFlashLight(true);//是否显示闪光灯
        intent.putExtra(Constant.INTENT_ZXING_CONFIG, config);
        m_instance.startActivityForResult(intent, REQUEST_CODE_SCAN);
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        // 扫描二维码/条码回传
        if (requestCode == REQUEST_CODE_SCAN && resultCode == RESULT_OK) {
            if (data != null) {

                String content = data.getStringExtra(Constant.CODED_CONTENT);
                SendDataToQt(content);
                //result.setText("扫描结果为：" + content);
            }
        }
    }
    //手机在应用程序按下返回键时退出界面,但不销毁，程序仍在后台运行
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
       if(keyCode==KeyEvent.KEYCODE_BACK){
           // 仿返回键退出界面,但不销毁，程序仍在后台运行
           moveTaskToBack(false); // 关键的一行代码
           return true;

       }
       return super.onKeyDown(keyCode, event);
   }

   //通知
    public static void notify(String msg){


        if (m_notificationManager == null) {
               m_notificationManager = (NotificationManager)m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
        }

        NotificationChannel channel = new NotificationChannel("1", "重要通知", NotificationManager.IMPORTANCE_HIGH);
        //channel.setDescription("这是重要通知，建议开启");
        // 设置通知出现时的闪灯（如果 android 设备支持的话）
        //channel.enableLights(true);
        //channel.setLightColor(Color.RED);
        // 设置通知出现时的震动（如果 android 设备支持的话）
        //channel.enableVibration(true);
        //channel.setVibrationPattern(new long[]{100, 200, 300, 400, 500, 400, 300, 200, 400});
        m_notificationManager.createNotificationChannel(channel);
        //---------------------------
        NotificationCompat.Builder builder= new NotificationCompat.Builder(m_instance,"1");

        builder.setSmallIcon(R.drawable.icon)
               .setContentTitle("【奇牛科技】")
               .setAutoCancel(true)
               .setWhen(System.currentTimeMillis()); //发送时间
        //builder.setContentText(msg);
        builder.setStyle(new NotificationCompat.BigTextStyle().bigText(msg));

        m_notificationManager.notify(12, builder.build());
        //-----------------------------
        /*
        Notification.Builder builder1 = new Notification.Builder(m_instance);
        builder1.setSmallIcon(R.drawable.icon); //设置图标
        builder1.setTicker("显示第二个通知");
        builder1.setContentTitle("通知"); //设置标题
        builder1.setContentText(msg); //消息内容
        builder1.setWhen(System.currentTimeMillis()); //发送时间
        builder1.setDefaults(Notification.DEFAULT_ALL); //设置默认的提示音，振动方式，灯光
        builder1.setAutoCancel(true);//打开程序后图标消失

        Intent intent =new Intent (m_instance,MyJavaClass.class);
        PendingIntent pendingIntent =PendingIntent.getActivity(m_instance, 0, intent, 0);

        builder1.setContentIntent(pendingIntent);
        //builder1.setChannelId(channelID);


        Notification notification1 = builder1.build();

        m_notificationManager.notify(124, notification1); // 通过通知管理器发送通知*/
   }

}
