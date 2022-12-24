#include "chaxun.h"
#include <iostream>
#include <QFile>
#include <urlmon.h>
#include <QImage>
#include <QThreadPool>
#include <QPainter>
#include "../rizhi/sslhttp.h"

void Chaxun::setBop(Baseop *vbop)
{
    bop=vbop;
}

void Chaxun::reset(){
    xuyaoShuminghao=false;
    duoyuShuminghao=false;
    xuyaoJuhao=false;
    pingtaiShiBaidu=false;
    youGaishutu=false;
}

void Chaxun::xiugaiLianzaiZhuangtai(LianzaiZhuangtai lz){
    //修改连载状态-小说作品
    UINT sjc[][2][7]={
        //半屏
        {
            //连载状态01  菜单空白2，连载中3，暂停更新4，完结5
            {318,728,    753,     781,   803,     832},
            {638,340,    920,     86,    243,     111}
        },
        //满屏
        {
            //---------------高屏幕100%---拖动页面后的坐标
            //连载状态01  菜单空白2，连载中3，暂停更新4，完结5 下拉菜单右下边x6
            //                                          判断菜单拉出来(6,5)，空点位置(6,3)
            {879,635,    677,     711,   747,     781,  916},//滚动条在顶部时向下滚2次，再确定坐标
            //---------------低屏幕90%---拖动页面后的坐标
            {609,453,    490,     523,   553,     577,  643}//滚动条在顶部时箭头向下5次，再确定坐标
        }
    };
    //点击连载状态
    dian(0,1);
    //等待“菜单”出来
    while(!yanse(6,5,0xD5D5D5)){
        Sleep(100);
    }
    Sleep(200);
    //点击选项
    switch(lz){
    case lzLianzaizhong:dian(0,3);break;
    case lzZanting:dian(0,4);break;
    case lzYiwanjie:dian(0,5);break;
    default:;
    }
    //等待“菜单”消失
    while(!yanse(6,5,0xFFFFFF)){
        Sleep(100);
    }
    //空点
    dian(6,3);
    Sleep(200);
}

bool Chaxun::gaishu(){
    UINT sjc[][2][20]={
        {},
        //------------全屏
        {
            //----------------高屏幕
            //搜索      替换2,3        查找输入框45     替换输入框x6  关搜索x7
            {679,132,  611,177,       225,179,       471,        678,
             //  概述空白 89   作者10 11     平台12 13
             293,439,     322,655,     636,587},
            //---------------低屏幕
            {638,340,  920,86,        243,178,   323,451,       619,356,       70,147,
             41,327,        938,85,    35,231}
        }
    };
    if(xuyaoShuminghao){
        dian(0,1);//搜索
        //等待“替换”出来
        while(!yanse(2,3,0xF5F5F5)){
            Sleep(100);
        }
        Sleep(100);
        //点击输入查找的内容：书名
        dian(4,5);
        Sleep(100);
        bop->SetCLB(citiaoMing);
        bop->CtrlJia('V');
        //点击输入替换的内容：《书名》
        dian(6,5);
        Sleep(100);
        bop->SetCLB(QString("《%1》").arg(citiaoMing));
        bop->CtrlJia('V');
        //点击替换
        dian(2,3);
        Sleep(100);
        //点击关闭
        dian(7,3);
        Sleep(50);
    }
    if(duoyuShuminghao){
        dian(0,1);//搜索
        //等待“替换”出来
        while(!yanse(2,3,0xF5F5F5)){
            Sleep(100);
        }
        Sleep(100);
        //点击输入查找的内容：《》
        dian(4,5);
        Sleep(100);
        bop->SetCLB("《》");
        bop->CtrlJia('V');
        //点击替换--删除
        dian(2,3);
        Sleep(100);
        //点击关闭
        dian(7,3);
        Sleep(50);
    }
    if(xuyaoJuhao){
        //点击概述部分底部空白
        dian(8,9);
        Sleep(50);
        bop->KeyPress(VK_END);
        bop->SetCLB("。");
        bop->CtrlJia('V');
    }
    return true;
}

bool Chaxun::xiaoshuoXinxilan(const QString& qw, bool&nengShibie){
    //修改小说作品信息栏
    //从参考资料获取内容，填写小说信息栏
    bool gaidong=false;
    UINT sjc[][2][10]={
        //半屏幕
        {
            //
            {679,132,  611,177,       225,179,       471,        678,
            },
            {638,340,  920,86,        243,178,   323,451,       }
        },
        //占满
        {
            //-------------高屏幕--------不滚动页面的坐标-----------
            //作者01   中文名y2   类型y3  平台x4     出版是56    出版否x7  空点89[高屏幕不需要]
            {883,543, 455,      590,   1290,      1057,546, 1112,    0,0},
            //-------------低屏幕--------滚动以后的坐标-----------
            {622,366, 286,      410,    977,      770,371,  820,     5,198}
        }
    };

    //低屏幕，修改信息栏先动页面，↓按键5次
    //高屏幕，在顶部时向下滚2次
    if(bop->pingmu==psDiPingmu){
        dian(8,9);
        for(int i=0;i<5;i++){
            bop->KeyPress(VK_DOWN);
            Sleep(100);
        }
    }else if(bop->pingmu==psGaoPingmu){
        bop->MouseWheel(-2);
        Sleep(200);
    }

    //检查词条信息栏是否有内容
    //
    QString zuozhe,leixing,pingtai,zhongwenMing;
    QString zuozheN,leixingN,pingtaiN;
    LianzaiZhuangtai zhuangtai=lzWeizhi,zhuangtaiN=lzWeizhi;
    ChubanZhuangtai chuban=czWeizhi,chubanN=czWeizhi;
    QRegularExpression regD;
    //中文名
    regD.setPattern("id=\"___1\" contenteditable=\"true\">(.*?)<");
    auto match = regD.match(qw);
    if (match.hasMatch()) {
        zhongwenMing=match.captured(1).trimmed();
    }
    //作者
    regD.setPattern("root_2_data__field.*?root_2_data_0__field.*?old-text\">(.*?)<");
    match = regD.match(qw);
    if (match.hasMatch()) {
        zuozhe=match.captured(1).trimmed();
    }
    //类型
    regD.setPattern("id=\"___3\" contenteditable=\"true\">(.*?)<");
    match = regD.match(qw);
    if (match.hasMatch()) {
        leixing=match.captured(1).trimmed();
    }
    //连载状态
    regD.setPattern("请选择连载状态.*?<span.*?>(.*?)<");
    match = regD.match(qw);
    if (match.hasMatch()) {
        QString str=match.captured(1).trimmed();
        if(str=="连载中")zhuangtai=lzLianzaizhong;
        else if(str=="已完结")zhuangtai=lzYiwanjie;
        else if(str=="暂停更新")zhuangtai=lzZanting;
        else zhuangtai=lzWeizhi;
    }
    //是否出版
    regD.setPattern("<span class=\"radio ?(.*?)\"><input type=\"checkbox\" class=\"radio-input\" value=\"是\">.*?"
                    "<span class=\"radio ?([^是]*?)\"><input type=\"checkbox\" class=\"radio-input\" value=\"否\">");
    match = regD.match(qw);
    if (match.hasMatch()) {
        QString shi=match.captured(1).trimmed();
        QString fou=match.captured(2).trimmed();
        if(shi=="radio-checked")chuban=czYichuban;
        else if(fou=="radio-checked")chuban=czWeichuban;
        else chuban=czWeizhi;
    }
    //连载平台
    regD.setPattern("id=\"___4\" contenteditable=\"true\">(.*?)<");
    match = regD.match(qw);
    if (match.hasMatch()) {
        pingtai=match.captured(1).trimmed();
    }

    //-------------------找到第一个参考资料----------------------
    //"J-ref-gn07qz90r9" class="reference-item type1"><span class="reference-index">[1]</span>
    //<a target="_blank" href="http://read.qidian.com/BookReader/1439410.aspx">至尊狙击</a>.起点网.<span class="reference-item__ref-data">[引用日期2014-08-16]
    regD.setPattern("id=\"J\\-ref\\-.*?href=\"(https?\\://.*?)\".*?</a>\\.(.*?)\\..*?>\\[引用日期");
    match = regD.match(qw);
    zlurl="";
    if (match.hasMatch()) {
        zlurl = match.captured(1);
        if(pingtai==""){
            pingtai=match.captured(2).trimmed();
        }
    }
    //从参考资料获取内容
    nengShibie=true;
    if(zlurl!=""){
        nengShibie=getXiaoshuoFromWeb(zlurl,zuozheN,leixingN,pingtaiN,zhuangtaiN,chubanN);
    }

    if(zuozheN!=""&&(zuozhe==""||zuozhe!=zuozheN)){
        gaidong=true;
        //点击信息栏 作者
        dian(0,1);
        Sleep(50);
        bop->CtrlJia('A');
        bop->SetCLB(zuozheN);
        bop->CtrlJia('V');
    }
    if(zhongwenMing==""|| zhongwenMing!=citiaoMing){
        gaidong=true;
        dian(0,2);
        Sleep(50);
        bop->CtrlJia('A');
        bop->SetCLB(citiaoMing);
        bop->CtrlJia('V');
    }
    //造类型
    if(leixingN==""&&leixing==""){
        leixingN="网络小说";
    }
    if(leixingN!=""&&(leixing==""||leixing!=leixingN)){
        gaidong=true;
        dian(0,3);
        Sleep(100);
        bop->CtrlJia('A');
        bop->SetCLB(leixingN);
        Sleep(50);
        bop->CtrlJia('V');
    }
    if(pingtaiN!=""&&(pingtai==""||pingtai!=pingtaiN)){
        gaidong=true;
        dian(4,2);
        Sleep(100);
        bop->CtrlJia('A');
        bop->SetCLB(pingtaiN);
        bop->CtrlJia('V');
    }
    if(chubanN!=czWeizhi&&(chuban==czWeizhi||chuban!=chubanN)){
        gaidong=true;
        if(chubanN==czYichuban){
            dian(5,6);
        }else if(chubanN==czWeichuban){
            dian(7,6);
        }
        Sleep(50);
    }

    //信息栏中最后修改
    if(zhuangtaiN!=lzWeizhi&&(zhuangtai==lzWeizhi||zhuangtai!=zhuangtaiN)){
        gaidong=true;
        xiugaiLianzaiZhuangtai(zhuangtaiN);
    }

    return gaidong;
}

bool Chaxun::getShujiFromQW(const QString& qw, QString zhengchangZifuN[]){
    QRegularExpression regD;

    QString regPat[xsCNT]={
        "sm",//shuming
        "<h2.*?>(?:出版|图书)信息.*?别名[:：] ?(.*?)<",//bieming
        "<h2.*?>(?:出版|图书)信息.*?作.?者(?:<.*?>)*[:：] ?(.*?)(?:主编|等著|等|著|编著|编)?</p",//zuozhe,
        "<h2.*?>(?:出版|图书)信息.*?(?:类别|开卷分类|图书分类)[:：] ?(.*?)<",//leibie
        "<h2.*?>(?:出版|图书)信息.*?原作品[:：] ?(.*?)<",//yuanzuopin
        "<h2.*?>(?:出版|图书)信息.*?译.?者[:：] ?(.*?)(?:等)?</p",//yizhe\r
        "<h2.*?>(?:出版|图书)信息.*?出.?版.?社[:：] ?(.*?)</p",//chubanshe
        "<h2.*?>(?:出版|图书)信息.*?出版(?:年|时间|日期)[:：] ?(\\d+)(?:[-\\.]\\d+)?(?:[-\\.]\\d+)?",
        "<h2.*?>(?:出版|图书)信息.*?出版(?:年|时间|日期)[:：] ?\\d+(?:[-\\.](\\d+))?(?:[-\\.]\\d+)?",
        "<h2.*?>(?:出版|图书)信息.*?出版(?:年|时间|日期)[:：] ?\\d+(?:[-\\.]\\d+)?(?:[-\\.](\\d+))?",
        "<h2.*?>(?:出版|图书)信息.*?页.?[码数].*?[:：] ?(\\d+)",//yeshu
        "<h2.*?>(?:出版|图书)信息.*?(?:定.?价|市场价|价.?格)[:：] ?￥?([\\d\\.]*)",//dingjia
        "<h2.*?>(?:出版|图书)信息.*?开.?本.*?[:：](.*?)</p",//kaiben
        "<h2.*?>(?:出版|图书)信息.*?(?:包.?装|装.?[帧订定])[:：](.*?)</p",//zhuangzhen装订：平装
        "<h2.*?>(?:出版|图书)信息.*?(?:I S B N|ISBN|isbn|书.?号).*?[:：] ?(?:ISBN)?([0-9xX\\-, ]+)",//isbn;
        "<h2.*?>(?:出版|图书)信息.*?丛书名?[:：] ?(.*?)</p",//丛书;
        "<h2.*?>(?:出版|图书)信息.*?ASIN[:：] ?(.*?)<",//asin
        "<h2.*?>(?:出版|图书)信息.*?字.?数[:：] ?(\\d+)<",//
        "<h2.*?>(?:出版|图书)信息.*?纸.?张[:：] ?(.*?)<",//
        "<h2.*?>(?:出版|图书)信息.*?副标题[:：] ?(.*?)<",//
    };
    for(int i=1;i<xsCNT;i++){
        regD.setPattern(regPat[i]);
        auto match = regD.match(qw);
        if (match.hasMatch()) {
            zhengchangZifuN[i]=match.captured(1).remove(QRegularExpression("<.*?>")).trimmed();
        }
    }
    //开本多种格式
    regD.setPattern("(大?\\d+)开");//开本： 16开
    auto match = regD.match(zhengchangZifuN[xsKaiben]);
    if (match.hasMatch()) {
        zhengchangZifuN[xsKaiben]=match.captured(1).trimmed();
    }
    //出版社带出版年 出版社: 中国档案出版社; 第1版 (2010年1月1日)
    sow<<zhengchangZifuN[xsChubanshe];
    regD.setPattern("(.*?)(?:</a>)?[;； ]*(?:第\\d+版) ?[（\\(](\\d+)年(\\d+)月(\\d+)日");
    match = regD.match(zhengchangZifuN[xsChubanshe]);
    if (match.hasMatch()) {
        zhengchangZifuN[xsChubanshe]=match.captured(1).trimmed();
        zhengchangZifuN[xsChubanSJn]=match.captured(2).trimmed();
        zhengchangZifuN[xsChubanSJy]=match.captured(3).trimmed();
        zhengchangZifuN[xsChubanSJr]=match.captured(4).trimmed();
    }
    //出版社删除分号
    zhengchangZifuN[xsChubanshe].remove(QRegularExpression("[;]"));
    //装帧页码混合
    regD.setPattern("<h2.*?>(?:出版|图书)信息.*?([精平]装)[:：] ?(\\d+)页<");//平装：36页
    match = regD.match(qw);
    if (match.hasMatch()) {
        zhengchangZifuN[xsZhuangzhen]=match.captured(1).trimmed();
        zhengchangZifuN[xsYeshu]=match.captured(2).trimmed();
    }
    //单独行：平装
    if(zhengchangZifuN[xsZhuangzhen]==""){
        regD.setPattern("<h2.*?>(?:出版|图书)信息.*?([精平]装):?</p");//平装
        match = regD.match(qw);
        if (match.hasMatch()) {
            zhengchangZifuN[xsZhuangzhen]=match.captured(1);
        }
    }
    //装帧去除多余
    if(zhengchangZifuN[xsZhuangzhen].contains("精装")){
        zhengchangZifuN[xsZhuangzhen]="精装";
    }else if(zhengchangZifuN[xsZhuangzhen].contains("平装")){
        zhengchangZifuN[xsZhuangzhen]="平装";
    }
    //ISBN删除横线
    zhengchangZifuN[xsIsbn].remove(QRegularExpression("[\\-]"));
    //13 10位isbn混合
    regD.setPattern("(\\d{13}), ?\\d+");
    match = regD.match(zhengchangZifuN[xsIsbn]);
    if (match.hasMatch()) {
        zhengchangZifuN[xsIsbn]=match.captured(1).trimmed();
    }
    regD.setPattern("[\\dxX]+, ?(\\d{13})");
    match = regD.match(zhengchangZifuN[xsIsbn]);
    if (match.hasMatch()) {
        zhengchangZifuN[xsIsbn]=match.captured(1).trimmed();
    }
    //nt转cny
    regD.setPattern("NT\\$(\\d+)");
    match = regD.match(zhengchangZifuN[xsDingjia]);
    if (match.hasMatch()) {
        zhengchangZifuN[xsDingjia]=QString("%1").arg(match.captured(1).trimmed().toDouble()*0.2281,0,'f',2);
    }
    //补齐作者——义项以后概述开始，图书信息以外的段落，概述，义项等。
    if(zhengchangZifuN[xsZuozhe]==""){
        regD.setPattern("J-lemma-abstract-context-word[\\s\\S]*?(?:作者是|作者简介(?:<.*?>)*)(.*?)[等（,\\.，。]");//
        match = regD.match(qw);
        if (match.hasMatch()) {
            zhengchangZifuN[xsZuozhe]=match.captured(1).remove(QRegularExpression("<.*?>")).trimmed();
        }
    }
    zhengchangZifuN[xsZuozhe]=zhengchangZifuN[xsZuozhe].remove(QRegularExpression("译者.*")).trimmed();
    return true;
}

bool Chaxun::shujiXinxilan(const QString& qw, bool&nengShibie){
    //修改“书籍”模板的信息栏
    bool gaidong=false;
    UINT sjc[][2][29]={
        //半屏幕
        {
            //
            {679,132,  611,177,       225,179,       471,        678,
            },
            {638,340,  920,86,        243,178,   323,451,       }
        },
        //占满
        {
            //-------------高屏幕--------不滚动页面的坐标-----------
            //书名01   别名y2   作者y3  类别y4   原作品y5  译者y6   出版社y7 出版时间n89 月x10  日x11
            {887,453, 500,     550,   591,    636,     682,    732,    1090,453, 1171,  1253,
             //页数1213  定价y14  开本y15  装帧y16 ISBNy17 增加信息按钮左侧边1819 左边新名称x20 右边新名称x21 左侧空点2223 书名编辑框左侧边2425 开本备注按钮2627 开本备注y28
             1201,498, 552,     592,    639,   684,    577,771,             591,        985,        236,866,    653,477,          1202,556,        626
            },
            //-------------低屏幕--------滚动以后的坐标-----------
            {622,366, 286,      410,    977,      770,371,  820,     5,198}
        }
    };

    //低屏幕，修改信息栏先动页面，↓按键5次
    //高屏幕，在顶部时向下滚2次
    if(bop->pingmu==psDiPingmu){
        dian(22,23);
        for(int i=0;i<5;i++){
            bop->KeyPress(VK_DOWN);
            Sleep(100);
        }
    }else if(bop->pingmu==psGaoPingmu){
        bop->MouseWheel(-2);
        Sleep(700);
    }

    //检测中文名是否太长--调整坐标
    if(yanse(24,25,0xe0e0e0)){//书名占了两行
        sow<<"zhang 2hang";
        uint zengjia=(bop->pingmu==psGaoPingmu)?22:0;
        for(int i=2;i<=7;i++){
            sjc[bop->banping][bop->pingmu][i]+=zengjia;
        }
    }

    //检查词条信息栏是否有内容
    //
    QString zhengchangZifu[xsCNT]={""};//信息栏现有内容
    QString zhengchangZifuN[xsCNT]={""};//正文获取到的内容
    UINT zuobiao[][2]={//固定区域的坐标
                       {0,  1},//shuming
                       {0,  2},//bieming
                       {0,  3},//zuozhe,
                       {0,  4},//leibie
                       {0,  5},//yuanzuopin
                       {0,  6},//yizhe
                       {0,  7},//chubanshe
                       {8,  9},//chubanSJ n
                       {10, 9},//chubanSJ y
                       {11, 9},//chubanSJ r
                       {12, 13},//yeshu
                       {12, 14},//dingjia
                       {12, 15},//kaiben
                       {12, 16},//zhuangzhen
                       {12, 17},//isbn;
                      };
    QString regPat[]={
        "id=\"___1\".*?>(.*?)<",//shuming
        "id=\"___2\".*?>(?:<.*?>)?(.*?)<",//bieming
        "id=\"root_2_data_0__field\".*?>(?:<.*?>\\s*)*(.*?)(?:<.*?>\\s*)<div id=\"root_3__label",//zuozhe,
        "id=\"___3\".*?>(?:<.*?>)?(.*?)<",//leibie
        "root_4_data_0__field.*?jss-field.*?lemmaSelectorEditor.*?container.*?<input .*?>.*?<p id=\"root_4_data_0__help",//yuanzuopin
        "id=\"root_5_data_0__field\".*?>(?:<.*?>)*(.*?)<",//yizhe\r
        "id=\"___4\".*?>(?:<.*?>)?(.*?)<",//chubanshe
        "",//chubanSJ n
        "",//chubanSJ y
        "",//chubanSJ r
        "",//yeshu
        "",//dingjia
        "",//kaiben
        "id=\"___5\".*?>(?:<.*?>)?(.*?)<",//zhuangzhen
        "id=\"___6\".*?>(?:<.*?>)?(.*?)<",//isbn;
    };
    QRegularExpression regD;
    //for(int i=0;i<xsCNT;i++){
    //    regD.setPattern(regPat[i]);
    //    auto match = regD.match(qw);
    //    if (match.hasMatch()) {
    //        zhengchangZifu[i]=match.captured(1).trimmed();
    //    }
    //}
    //丛书
    //出品方
    //副标题

    //-------------------从正文获取内容----------------------
    nengShibie=getShujiFromQW(qw,zhengchangZifuN);

    //填写固定项目
    for(int i=0;i<xsGudingCNT;i++){
        sow<<i;
        sow<<zhengchangZifuN[i];
        if(zhengchangZifuN[i]!=""){
            gaidong=true;
            dianD(zuobiao[i][0],zuobiao[i][1],(i==xsChubanSJn||i==xsChubanSJy||i==xsChubanSJr||i==xsIsbn)?2:1);
            Sleep(50);
            if(i==xsKaiben){
                //给开本加备注
                if(zhengchangZifuN[xsKaiben].contains("大")){
                    zhengchangZifuN[xsKaiben].remove("大");
                    dian(26,27);
                    Sleep(50);
                    dian(26,28);
                    Sleep(50);
                    bop->CtrlJia('A');
                    bop->SetCLB("大");
                    bop->CtrlJia('V');
                    dian(22,23);
                    Sleep(50);
                    dian(zuobiao[i][0],zuobiao[i][1]);
                    Sleep(50);
                }
            }
            if(i==xsChubanSJn||i==xsChubanSJy||i==xsChubanSJr){
                //年月日，不用ctrlA全选。使用双击全选
            }else{
                //isbn:双击+CtrlA全选
                bop->CtrlJia('A');
            }
            //return true;
            bop->SetCLB(zhengchangZifuN[i]);
            bop->CtrlJia('V');
        }
    }


    /*
    //填写动态项目
    //找到增加信息项按钮
    uint btnx=sjc[bop->banping][bop->pingmu][18];//按钮左侧边x
    uint btny=bop->FindBiaojiXiangxia(btnx,
                                      sjc[bop->banping][bop->pingmu][19],0xd5d5d5);//按钮左侧边顶部D5D5D5 y
    QString ns[]={
        "丛书","ASIN","字数","纸张","副标题"
    };
    for(int i=xsGudingCNT;i<xsCNT;i++){
        if(zhengchangZifuN[i]!=""){
            //增加一项
            bop->LeftClick(btnx+10,btny+5);
            Sleep(100);
            uint mx=0,my=0,vx=0;
            //找到新增的编辑框位置
            mx=sjc[bop->banping][bop->pingmu][20];
            my=btny-5;
            vx=sjc[bop->banping][bop->pingmu][0];
            if(bop->Color(vx,my,0xF8F8F8)){
                btny=bop->FindBiaojiXiangxia(btnx,
                                                  btny,0xd5d5d5);//按钮左侧边顶部D5D5D5 y
            }else{
                mx=sjc[bop->banping][bop->pingmu][21];
                my=btny-40;
                vx=sjc[bop->banping][bop->pingmu][12];;
            }
            //填写名称
            bop->LeftClick(mx,my);
            Sleep(50);
            bop->SetCLB(ns[i-xsGudingCNT]);
            bop->CtrlJia('V');
            //填写值
            bop->LeftClick(vx,my,2);
            Sleep(50);
            bop->SetCLB(zhengchangZifuN[i]);
            bop->CtrlJia('V');
        }
    }
    */

    return gaidong;
}
bool Chaxun::getHanziFromQW(const QString& qw, QString zhengchangZifuN[]){
    QRegularExpression regD;

    QString regPat[xhCNT]={
        "<h2.*?>(?:基本)信息.*?拼.?音(?:<.*?>)*[:：] ?(.*?)(?:主编|等著|等|著|编著|编)?</p",
        "<h2.*?>(?:基本)信息.*?(?:繁体字).*?[:：](.*?)<",
        "<h2.*?>(?:基本)信息.*?部首[:：](.*?)(?:部外笔画|</p)",
        "<h2.*?>(?:基本)信息.*?(?:统一码|UniCode)[:：] ?(?:基本区|CJK 统一汉字)?(?:[ Uu]*\\+)?(.*?)(?:</p|，|,)",//字码 unicode
        "<h2.*?>(?:基本)信息.*?五笔(?:86&|86/)?98.*?[:：](.*?)(?:仓颉|<|，|,)",
        "<h2.*?>(?:基本)信息.*?仓颉.*?[:：](.*?)(?:</p|，|,)",
        "<h2.*?>(?:基本)信息.*?郑码.*?[:：](.*?)(?:</p|，|,)",
        "<h2.*?>(?:基本)信息.*?笔顺(?:编号|读写)?[:：](.*?)</p",
        "<h2.*?>(?:基本)信息.*?注音.*?[:：](.*?)<",
        "<h2.*?>(?:基本)信息.*?总笔画[:：] ?(\\d+)</p",
        "<h2.*?>(?:基本)信息.*?四角(?:号码)?.*?[:：] ?(\\d+)",
        "<h2.*?>(?:基本)信息.*?GBK(?:编码)?[:：](.*?)</p",
        "<h2.*?>(?:基本)信息.*?(?:汉字)?结构.*?[:：](.*?)(?:</p|造字法|<img)",
        "<h2.*?>(?:基本)信息.*?造字法.*?[:：](.*?)</",
    };
    for(int i=0;i<xhCNT;i++){
        regD.setPattern(regPat[i]);
        auto match = regD.match(qw);
        if (match.hasMatch()) {
            zhengchangZifuN[i]=match.captured(1).remove(QRegularExpression("<.*?>")).remove(QRegularExpression("[，,]")).trimmed();
        }
    }
    if(zhengchangZifuN[xhWubi]==""){
        regD.setPattern("<h2.*?>(?:基本)信息.*?五笔(?:86)?[:：](.*?)<");
        auto match = regD.match(qw);
        zhengchangZifuN[xhWubi]=match.captured(1).remove(QRegularExpression("<.*?>")).trimmed();
    }
    return true;
}
bool Chaxun::getXiaoshuoFromQW(const QString& qw, QString zhengchangZifuN[]){
    QRegularExpression regD;

    QString regPat[3]={
        "小说进度.*?contenteditable.*?>(.*?)<",
        ">连载网站<.*?contenteditable.*?>(.*?)</div",
        "<h2.*?>小说类型</h2><.*?>(.*?)<",
    };
    for(int i=0;i<3;i++){
        regD.setPattern(regPat[i]);
        auto match = regD.match(qw);
        if (match.hasMatch()) {
            zhengchangZifuN[i]=match.captured(1).remove(QRegularExpression("<.*?>")).remove(QRegularExpression("[，,]")).trimmed();
        }
    }
    return true;
}

bool Chaxun::xiaoshuoXinxilanQita(bool isQita,const QString& qw, bool&nengShibie){
    bool gaidong=false;
    UINT sjc[][2][28]={
        //半屏幕
        {
            //
            {679,132,  611,177,       225,179,       471,        678,
            },
            {638,340,  920,86,        243,178,   323,451,       }
        },
        //占满
        {
            //-------------高屏幕--------不滚动页面的坐标-----------
            //作者01   中文名y2   类型y3  平台x4     出版是56    出版否x7  空点89[高屏幕不需要]
            {883,543, 455,      590,   1290,      1057,546, 1112,    0,0,
             //改模板
             //修改1011 文化1213 小说1415  确定1617   撤销1819  回复2021
             320,248,  1074,358,635,490, 1034,827, 589,132, 621,131,
             //删信息栏小说进度 连载网站2223 正文第一段2425 一级目录2627
             564,693,                    814,887,   678,132
            },
            //-------------低屏幕--------滚动以后的坐标-----------
            {622,366, 286,      410,    977,      770,371,  820,     5,198}
        }
    };

    bop->CtrlJia(VK_HOME);

    //低屏幕，修改信息栏先动页面，↓按键5次
    //高屏幕，在顶部时向下滚2次
    if(bop->pingmu==psDiPingmu){
        dian(8,9);
        for(int i=0;i<5;i++){
            bop->KeyPress(VK_DOWN);
            Sleep(100);
        }
    }else if(bop->pingmu==psGaoPingmu){
        bop->MouseWheel(-2);
        Sleep(200);
    }

    LianzaiZhuangtai zhuangtaiN=lzWeizhi;
    QRegularExpression regD;

    QString zhengchangZifuN[3]={""};//正文获取到的内容
    nengShibie=getXiaoshuoFromQW(qw,zhengchangZifuN);

    Sleep(600);
    //修改模板---其他 改成 小说
    if(isQita){
        dian(10,11);//修改
        while(!yanse(16,17,0x333333)){
            Sleep(100);
        }
        dian(12,13);Sleep(100);//文化
        dian(14,15);Sleep(100);//小说
        dian(16,17);Sleep(200);//确定
        dian(18,19);Sleep(200);//撤销
        dian(20,21);Sleep(200);//恢复
    }
    //0 1 2 进度 平台 类型
    //类型
    if(zhengchangZifuN[2]!=""){
        gaidong=true;
        dian(0,3);
        Sleep(100);
        bop->CtrlJia('A');
        bop->SetCLB(zhengchangZifuN[2]);
        Sleep(50);
        bop->CtrlJia('V');
    }
    //平台
    if(zhengchangZifuN[1]!=""){
        gaidong=true;
        dian(4,2);
        Sleep(100);
        bop->CtrlJia('A');
        bop->SetCLB(zhengchangZifuN[1]);
        bop->CtrlJia('V');
    }

    //信息栏中最后修改
    if(zhengchangZifuN[0]!=""){
        if(zhengchangZifuN[0].contains(QRegularExpression("连载"))){
            zhuangtaiN=lzLianzaizhong;
        }else if(zhengchangZifuN[0].contains(QRegularExpression("已完结"))){
            zhuangtaiN=lzYiwanjie;
        }else if(zhengchangZifuN[0].contains(QRegularExpression("暂停"))){
            zhuangtaiN=lzZanting;
        }else{
            zhuangtaiN=lzWeizhi;
            nengShibie=false;
        }
        xiugaiLianzaiZhuangtai(zhuangtaiN);
    }

    //
    Sleep(100);
    //删除小说进度 连载网站 正文去掉类型 简介改为黑体
    dian(22,23);Sleep(100);
    dian(22,23);Sleep(100);
    //
    dian(24,25);Sleep(200);
    bop->CtrlShiftJia(VK_HOME);Sleep(300);
    bop->KeyPress(VK_BACK);Sleep(100);
    bop->KeyPress(VK_BACK);Sleep(100);
    dian(26,27);
    Sleep(50);
    bop->ShiftJia(VK_END);
    bop->CtrlJia('B');
    return gaidong;
}

bool Chaxun::hanziXinxilan(const QString& qw, bool&nengShibie){
    //修改“汉字”模板的信息栏
    bool gaidong=false;
    UINT sjc[][2][16]={
        //半屏幕
        {
            //
            {679,132,  611,177,       225,179,       471,        678,
            },
            {638,340,  920,86,        243,178,   323,451,       }
        },
        //占满
        {
            //-------------高屏幕--------不滚动页面的坐标-----------
            //拼音x0 y1   繁体y2   部首y3  字码y4   五笔y5  仓颉67
            //      郑码y 笔顺y
            {885,   499, 545,     591,   638,    684,   1288,452,
             //增加信息按钮左侧边89 左边新名称x10 右边新名称x11 左侧空点1213 第一个编辑框左侧边1415
             //x精确。y估计
             577,742,             591,        985,        236,866,    653,477,
            },
            //-------------低屏幕--------滚动以后的坐标-----------
            {622,366, 286,      410,    977,      770,371,  820,     5,198}
        }
    };

    //低屏幕，修改信息栏先动页面，↓按键5次
    //高屏幕，在顶部时向下滚2次
    if(bop->pingmu==psDiPingmu){
        dian(12,13);
        for(int i=0;i<5;i++){
            bop->KeyPress(VK_DOWN);
            Sleep(100);
        }
    }else if(bop->pingmu==psGaoPingmu){
        bop->MouseWheel(-2);
        Sleep(700);
    }

    //检测中文名是否太长--调整坐标
    if(yanse(14,15,0xe0e0e0)){//书名占了两行
        sow<<"zhang 2hang";
        uint zengjia=(bop->pingmu==psGaoPingmu)?22:0;
        for(int i=2;i<=7;i++){
            sjc[bop->banping][bop->pingmu][i]+=zengjia;
        }
    }

    //
    QString zhengchangZifuN[xhCNT]={""};//正文获取到的内容
    UINT zuobiao[][2]={//固定区域的坐标
                       {0,  1},//拼音
                       {0,  2},//繁体
                       {0,  3},//部首
                       {0,  4},//字码
                       {0,  5},//五笔
                       {6,  7},//仓颉
                       {6,  1},//郑码y
                       {6,  2},//笔顺y
                      };
    QRegularExpression regD;

    //-------------------从正文获取内容----------------------
    nengShibie=getHanziFromQW(qw,zhengchangZifuN);

    //填写固定项目
    for(int i=0;i<xhGudingCNT;i++){
        sow<<i;
        sow<<zhengchangZifuN[i];
        if(zhengchangZifuN[i]!=""){
            gaidong=true;
            dianD(zuobiao[i][0],zuobiao[i][1],1);
            Sleep(50);
            bop->CtrlJia('A');
            //return true;
            bop->SetCLB(zhengchangZifuN[i]);
            bop->CtrlJia('V');
        }
    }

    //填写动态项目
    //找到增加信息项按钮
    uint btnx=sjc[bop->banping][bop->pingmu][8];//按钮左侧边x
    uint btny=bop->FindBiaojiXiangxia(btnx,
                                      sjc[bop->banping][bop->pingmu][9],0xd5d5d5);//按钮左侧边顶部D5D5D5 y
    QString ns[]={
        "注音","总笔画","四角号码","GBK编码","结构","造字法"
    };
    for(int i=xhGudingCNT;i<xhCNT;i++){
        if(zhengchangZifuN[i]!=""){
            //增加一项
            bop->LeftClick(btnx+10,btny+5);
            Sleep(100);
            uint mx=0,my=0,vx=0;
            //找到新增的编辑框位置
            mx=sjc[bop->banping][bop->pingmu][10];//左边新名称x
            my=btny-5;
            vx=sjc[bop->banping][bop->pingmu][0];
            if(bop->Color(vx,my,0xF8F8F8)){
                btny=bop->FindBiaojiXiangxia(btnx,
                                             btny,0xd5d5d5);//按钮左侧边顶部D5D5D5 y
            }else{
                mx=sjc[bop->banping][bop->pingmu][11];//右边新名称x
                my=btny-40;
                vx=sjc[bop->banping][bop->pingmu][6];//第二列信息栏编辑框光标点击用来输入，vx=鼠标x
            }
            //填写名称
            bop->LeftClick(mx,my);
            Sleep(50);
            bop->SetCLB(ns[i-xhGudingCNT]);
            bop->CtrlJia('V');
            //填写值
            bop->LeftClick(vx,my,2);
            Sleep(50);
            bop->SetCLB(zhengchangZifuN[i]);
            bop->CtrlJia('V');
        }
    }

    return gaidong;
}

bool Chaxun::getXiaoshuoFromWeb(QString&url,
                                QString& zuozhe,QString& leixing,QString& pingtai,
                                LianzaiZhuangtai& zhuangtai, ChubanZhuangtai& chuban)
{
    bool nengShibie=true;
    //-------------------地址预处理--------------------------------
    QRegularExpression reg;
    //snd<<url;
    reg.setPattern("http://www.jjwxc.net/onebook.php\\?novelid=.*");
    auto match = reg.match(url);
    if (match.hasMatch()) {
        url.replace("http:","https:");
        //snd<<url;
    }

    //--------------------get获取内容-------------------------------------
    QString web;
    SslHttp sh(web);
    //snd<<url;
    sh.getData(url);

    //--------------------分析参考资料内容--------------------
    if(url.indexOf("www.17k.com/book")!=-1){
        //http://www.17k.com/book/708927.html
        //http://www.17k.com/book/95460.html
        //snd<<web;
        QRegularExpression regD;
        regD.setPattern("<meta http-equiv=\"refresh\".*?error.html");
        match = regD.match(web);
        if (match.hasMatch()) {
            //参考资料无内容
        }else{
            pingtai="17K小说网";
            regD.setPattern("作品类别：</th>[\\s]*<td>[\\s]*(?:<[\\s\\S]*?>)?(.*?)(?:<.*?>)?</td>");
            match = regD.match(web);
            if (match.hasMatch()) {
                leixing=match.captured(1).trimmed();
            }
            //snd<<leixing;
            regD.setPattern("--作者信息--[\\s\\S]*?>作者<.*?>[\\s]*(?:<[\\s\\S]*?>)?([^<\\s]*)");
            match = regD.match(web);
            if (match.hasMatch()) {
                zuozhe=match.captured(1).trimmed();
            }
            //snd<<zuozhe;
            regD.setPattern("<div class=\"label\">([\\s\\S]*?)</div>");
            match = regD.match(web);
            if (match.hasMatch()) {
                QString biaoqian=match.captured(1);
                if(biaoqian.indexOf("完本小说")!=-1 || biaoqian.indexOf("全本小说")!=-1){
                    zhuangtai=lzYiwanjie;
                }else if(biaoqian.indexOf("连载小说")!=-1){
                    zhuangtai=lzLianzaizhong;
                }else{
                    nengShibie=false;
                    snd<<QString("不识别的状态");
                }
            }
            //snd<<zhuangtai;
        }
    }else if(url.indexOf("www.jjwxc.net")!=-1){
        pingtai="晋江文学网";
        //
        /*
        itemprop="genre">
                                                    评论                                        </span>
        */
        reg.setPattern("itemprop=\"genre\">[\\r\\n ]*(?:.*?-.*?-.*?-)?(.*?)[ ]*</span>");
        match = reg.match(web);
        if (match.hasMatch()) {
            leixing=match.captured(1).trimmed();
            if(leixing=="未知"){//什么破网站
                leixing="";
            }
        }
        //itemprop="updataStatus"><font color="red">完结</font></span>
        //itemprop="updataStatus">连载</span>
        reg.setPattern("itemprop=\"updataStatus\">(?:<.*?>)?(.*?)<");
        match = reg.match(web);
        if (match.hasMatch()) {
            QString ztstr=match.captured(1).trimmed();
            if(ztstr=="完结")zhuangtai=lzYiwanjie;
            else if(ztstr=="连载")zhuangtai=lzLianzaizhong;
            else if(ztstr=="暂停")zhuangtai=lzZanting;
        }
        reg.setPattern("是否出版：.*?[\\r\\n ]*(.*?)出版");
        chuban=czWeizhi;
        match = reg.match(web);
        if (match.hasMatch()) {
            QString cz=match.captured(1).trimmed();
            if(cz=="尚未")chuban=czWeichuban;
            else if(cz=="xx")chuban=czYichuban;
        }
        reg.setPattern("id=\"pub_rs_novelname\"[\\s\\S]*?作者：(?:<.*?>)*(.*?)<");
        match = reg.match(web);
        if (match.hasMatch()) {
            zuozhe=match.captured(1).trimmed();
            if(zuozhe.indexOf("*******")!=-1){
                zuozhe="";
            }
        }
    }else if(url.indexOf("www.readnovel.com")!=-1){
        //网址实例总结
        //http://www.readnovel.com/book/93028/
        reg.setPattern("<title>出错啦！</title>");
        match = reg.match(web);
        if (match.hasMatch()) {
        }else{
            snd<<QString("不识别 小说阅读网 新页面");
            nengShibie=false;
            snd<<web;
        }
    }else if(url.indexOf("qidian.com")!=-1){
        snd<<url;
        //http://www.qidian.com/Book/2474678.aspx
        //http://www.qidian.com/Book/119176.aspx
        //https://book.qidian.com/info/119176/
        //snd<<web;
        if(web.trimmed()==""){
            nengShibie=true;
        }else{
            reg.setPattern("<title>error--.*?</title>|zrtarsqhmf");
            match = reg.match(web);
            if (match.hasMatch()) {
                nengShibie=true;
            }else{
                snd<<QString("不识别 起点 新页面");
                nengShibie=false;
                snd<<web;
            }
        }
    }else if(url.indexOf("book.zongheng.com/book")!=-1){
        pingtai="纵横中文网";
        //snd<<url;
        //http://book.zongheng.com/book/316244.html
        //snd<<web;
        reg.setPattern("<title>访问页面不存在</title>");
        match = reg.match(web);
        if (match.hasMatch()) {
            nengShibie=true;
        }else{
            snd<<QString("不识别纵横中文网新页面");
            nengShibie=false;
            snd<<web;
        }
    }else{
        snd<<QString("不识别新的网址");
        snd<<url;
        nengShibie=false;
    }
    return nengShibie;
}

void Chaxun::tijiaoQiehuan(){
    xuyaoTijiao=!xuyaoTijiao;
    sow<<(xuyaoTijiao?QString("填表+提交"):QString("不提交"));
}
void Chaxun::xiugaiMuluXinxilan(){
    //临时任务
    UINT sjc[][2][2]={
        //半屏幕
        {
            //
            {},
            {}
        },
        //占满
        {
            //-------------高屏幕---------滚到底以后的坐标----------
            //侧边空点67
            {207,687},
            //-------------低屏幕----------滚到底以后的坐标---------
            {13,510}
        }
    };
    dian(0,1);
    bop->CtrlJia(VK_HOME);

    QString qw;
    YuanmaFenxi yf=bop->GetInfoFormEdit(qw);
    citiaoMing=yf.citiaoMing;

    //词条太长时放弃
    if(citiaoMing.length()>16){
        //bop->fangqi();
        //return;
    }

    bool gaidong=false;
    //有参考资料，但是不识别：false.
    //有参考资料，能识别：true
    //没有参考资料：true
    bool nengShibieZiliao=true;

    //------------------------------------检查模板-----------------------------------------------
    QString muban;//!< 信息栏模板
    QRegularExpression regD;
    regD.setPattern("id=\"J-lemma-type-content\" class=\"lemma-type-content\"><p class=\"lemma-type-sub\">(.*?)<");
    auto match = regD.match(qw);
    if (match.hasMatch()) {
        muban=match.captured(1).trimmed();;
    }
    //------------------------------------模板是小说作品-----------------------------------------------
    if(muban=="小说作品xxx"){//
        //获得参考资料，填写信息栏
        gaidong|=xiaoshuoXinxilan(qw,nengShibieZiliao);
    }
    //------------------------------------模板是书籍-----------------------------------------------
    else if(muban=="书籍"){
        //从正文获得需要的数据填写信息栏。
        gaidong|=shujiXinxilan(qw,nengShibieZiliao);
    }
    //------------------------------------模板是书籍-----------------------------------------------
    else if(muban=="汉字"){
        //从正文获得需要的数据填写信息栏。
        gaidong|=hanziXinxilan(qw,nengShibieZiliao);
    }
    //----------------------------------模板是其他，改成小说，从正文获取内容填写---
    else if(muban=="其他"||muban=="小说作品"){
        //从正文获得需要的数据填写信息栏。
        gaidong|=xiaoshuoXinxilanQita(muban=="其他",qw,nengShibieZiliao);
    }
    //------------------------------------模板是化学-----------------------------------------------
    else if(false){

    }

    //-------------------------------- ----正文是作者+作品简介
    regD.setPattern("___6\" contenteditable=\"true\"><h2.*?>(.*?)</h2><p.*?>.*?</p><h2.*?>(.*?)</h2><p.*?>.*?</p></div>");
    match = regD.match(qw);
    if (match.hasMatch()) {
        if(match.captured(1).trimmed()=="作者" && match.captured(2)!=""){
            xiugaiZhengwenZuozheZuopinjianjie(match.captured(2).trimmed());
            gaidong=true;
        }
    }

    if(!nengShibieZiliao){//不认识参考资料
        sow<<QString("不认识参考资料");
        bop->showWindow();
    }else if(!gaidong){//没有改动
        //bop->fangqi();
    }else{//提交
        if(xuyaoTijiao){
            //bop->tijiao(tfGengzhengCuowu,true);//自动提交
        }
    }
}

void Chaxun::xiugaiZhengwenZuozheZuopinjianjie(const QString& mulu2){
    //两个目录，作者+（作品简介|xxxx）
    //删除作者目录
    //作品简介加粗
    UINT sjc[][2][20]={
        //半屏幕
        {
            //
            {679,132,  611,177,       225,179,       471,        678,
             //  概述空白 89   作者10 11     平台12 13
             293,439,     322,655,     636,587},
            {638,340,  920,86,        243,178,   323,451,       619,356,       70,147,
             41,327,        938,85,                35,231}
        },
        //占满
        {
            //-------------高屏幕---------滚到底以后的坐标----------
            //目录标记色x0 标记点与点击行的y差1  点击行x2 点击行y3=Y-[1] 一级目录45  侧边空点67
            //  |            |               |        |             |         |----空点
            //  |            |               |        |             |----顶部一级目录按钮xy
            //  |            |               |        |--计算作品简介目录行的y
            //  |            |               |---作品简介目录行的右侧空白x
            //  |            |--点击行：作品简介目录所在行
            //  |---作品简介一级目录左边灰色短竖线x，通过FindBiaoji找到y
            {580,        8,                  1260,   0,            672,132,  207,687},
            //-------------低屏幕----------滚到底以后的坐标---------
            {340,        9,                  963,    0,            421,128,  13,510}
        }
    };
    dian(6,7);
    bop->CtrlJia(VK_END);
    if(bop->pingmu==psDiPingmu){
        Sleep(500);
    }else{
        Sleep(300);
    }
    UINT y=bop->FindBiaoji(sjc[bop->banping][bop->pingmu][0],0,0xCCCCCC);
    //snd<<y;//用来测算【1】的值  [1]=点击试验[3](212)-y(203)
    sjc[bop->banping][bop->pingmu][3]=y-sjc[bop->banping][bop->pingmu][1];
    //snd<<sjc[bop->banping][bop->pingmu][3];
    dian(2,3);  //作品简介目录行
    Sleep(100);
    bop->CtrlShiftJia(VK_HOME);
    Sleep(100);
    bop->SetCLB(mulu2);
    Sleep(100);
    bop->CtrlJia('V');
    Sleep(100);
    dian(4,5);
    Sleep(100);
    bop->KeyPress(VK_UP);
    bop->KeyPress(VK_BACK);
    Sleep(200);
    bop->ShiftJia(VK_END);
    Sleep(200);
    bop->CtrlJia('B');
}
