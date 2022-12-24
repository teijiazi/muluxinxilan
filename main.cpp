#include "../baikePub/shouci.h"
#include "chaxun.h"
#include <winver.h>
#include <windows.h>
#include "../baikePub/BaikeTypeData.h"
using namespace std;

#include <QApplication>

///
/// 写点说明，时间长了就忘了。
/// 全自动，必须F2启动：在任务列表页面，鼠标放在词条上，按F2启动
/// 已经打开的，F1启动，填完以后，还得返回到F2启动，才能全自动。
/// 原理：
/// F2启动，记录鼠标的位置，以便自动移动到下一个词条上。
///

//void shifang()
//{
//    for(mvk& mv:modvk)
//    {
//        bool jg=UnregisterHotKey(NULL, mv.at);
//        if(!jg){
//            snd<<"unreg err="<<GetLastError()<<",id="<<mv.at<<",key="<<mv.idstr;
//        }
//    }
//}
BOOL WINAPI ConsoleCtrlhandler(DWORD dwCtrlType)
{
    //控制台消息循环抓不住wm_close wm_quit wm_destroy
    //需要使用SetConsoleCtrlHandler(ConsoleCtrlhandler, TRUE);在退出前释放注册的hotkey
    //经使用发现hotkey在程序结束时自动失效，不必释放。
    //如果提供更改hotkey的功能则可以使用释放
    if(dwCtrlType == CTRL_CLOSE_EVENT)
    {
        HWND hwndGuanbi = FindWindowW(NULL, L"关闭");
        if(hwndGuanbi!=NULL){
            DWORD tid=GetWindowThreadProcessId(hwndGuanbi,NULL);
            PostThreadMessageW(tid,WM_USER+msgCloseGbid,0,0);
        }
        //shifang();
    }
    return TRUE;
}

int main(int argc, char *argv[])
{
    UINT sjc[][2][1]={
        //半屏幕
        {
            //放弃y
            {7},
            {7}
        },
        //满屏幕
        {
            //---------------高100%
            //放弃y
            {793},
            //---------------低90%
            {713}

        }
    };

    QApplication a(argc, argv);

    Q_UNUSED(argc);
    QThreadPool::globalInstance()->setMaxThreadCount(2);

    Baseop bop;

    int vkTijiaoKaiguan=bop.vkAdd("vkTijiaoKaiguan",VK_F5);

    bool isRebootself=bop.init(L"目录信息栏--F6强制关闭",pzManping,true,
                               argv[0],
            QString("\r\n1 Edge浏览器，页面100%显示"
                    "\r\n\r\n2 F1 提交"
                    "\r\n\r\n3 F2连续"
                    "\r\n\r\n3 F3开始填表"
                    "\r\n\r\n4 F5提交开关"),
            QString("\r\n1 Edge浏览器，页面90%显示"
                    "\r\n\r\n3 F1提交"
                    "\r\n\r\n3 F2连续"
                    "\r\n\r\n3 F3开始填表"
                    "\r\n\r\n3 F5提交开关")
            );
    bop.renwuZuobiao(sjc[bop.banping][bop.pingmu][0]);


    Shouci sc(&bop);
    Chaxun dydata;
    dydata.setBop(&bop);
    //
    SetConsoleCtrlHandler(ConsoleCtrlhandler, TRUE);
    MSG msg;
    PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        switch(msg.message)
        {
        //case WM_DESTROYCLIPBOARD://控制台窗口收不到这个消息
        //    sow<<"delclip";
        //    break;
        case WM_USER+msgHotkey:
            bop.zhucerejian();
            if(isRebootself){
                isRebootself=false;
                bool jg=bop.xiayige();
                if(jg){
                    sc.runscript();
                }else{
                    sow<<"chognqi error";
                }
            }
            break;
        case WM_HOTKEY:
        {
            UINT ctrlalt=LOWORD(msg.lParam);
            UINT anjian=HIWORD(msg.lParam);

            if(ajjc(vkiBtnleft)){
                bop.LeftClick();
            }else if(ajjc(vkiTijiao)){
                bop.tijiao(tfGengzhengCuowu,true);
            }else if(ajjc(vkiLianxu)){
                sc.runscript();//记录鼠标位置并打开词条
            }else if(ajjc(vkiFangqi)){
                bop.fangqi();//自动找到下一个词条，打开。
            }else if(ajjc(vkiTianbiao)){
                dydata.xiugaiMuluXinxilan();
                //-----------------------------------------------------------------
            }else if(ajjc(vkTijiaoKaiguan)){
                dydata.tijiaoQiehuan();
            }
        }
            break;
        default:
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return a.exec();
}
