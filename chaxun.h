#ifndef MIGUYINYUE_H
#define MIGUYINYUE_H
#include "../rizhi/httpreq.h"
#include "../baikePub/baseop.h"
#include <QMutexLocker>

enum LianzaiZhuangtai{
    lzLianzaizhong=0,//!<连载中
    lzZanting=1,     //!<暂停更新
    lzYiwanjie=2,    //!<完结
    lzWeizhi       //!<没有找到
};

enum ChubanZhuangtai{
    czWeichuban,//!<未出版
    czYichuban, //!<已出版
    czWeizhi    //!<不知
};

//-------------书籍信息栏

enum xinxilanShuji{
    xsShuming,
    xsBieming,
    xsZuozhe,
    xsLeibie,
    xsYuanzuopin,
    xsYizhe,
    xsChubanshe,
    xsChubanSJn,
    xsChubanSJy,
    xsChubanSJr,
    xsYeshu,
    xsDingjia,
    xsKaiben,
    xsZhuangzhen,
    xsIsbn,
    xsGudingCNT,
    xsCongshu=xsGudingCNT,
    xsASIN,
    xsZishu,
    xsZhizhang,
    xsFubiaoti,
    xsCNT
};
//-------------书籍信息栏

//-------------汉字信息栏----
enum xinxilanHanzi{
    xhPinyin,
    xhFanti,
    xhBushou,
    xhZima,
    xhWubi,
    xhCangjie,
    xhZhengma,
    xhBishun,
    xhGudingCNT,
    xhZhuyin=xhGudingCNT,
    xhZongbihua,
    xhSijiaohaoma,
    xhGBK,
    xhJiegou,
    xhZaozifa,
    xhCNT
};
//-------------汉字信息栏----

class Chaxun: public QObject
{
    Q_OBJECT
public:
    explicit Chaxun(QObject *parent = 0):QObject(parent),
        bop(nullptr),xuyaoShuminghao(false),
        duoyuShuminghao(false),
        xuyaoJuhao(false),
        pingtaiShiBaidu(false),
        youGaishutu(false),
        xuyaoTijiao(true)
    {
    }
    ~Chaxun(){}
    void setBop(Baseop *vbop);
    Baseop *bop;
    //
    QString citiaoMing;//!< 词条名称
    bool gaishu();
    bool xuyaoShuminghao;
    bool duoyuShuminghao;
    bool xuyaoJuhao;
    bool pingtaiShiBaidu;
    bool youGaishutu;//!<是否有概述图
    //
    QString zlurl;//!<第一个参考资料url
    //
    void reset();
    ///修改连载状态-小说作品
    void xiugaiLianzaiZhuangtai(LianzaiZhuangtai lz);
    //
    QNetworkAccessManager nam;
    /// 临时任务：修改目录信息栏
    void xiugaiMuluXinxilan();
    /// 修改小说作品信息栏,返回是否改动过,是true否false
    bool xiaoshuoXinxilan(const QString &qw, bool &nengShibie);
    /// 在参考资料中获取指定信息,返回true表示参考资料能识别，false表示不能识别
    bool getXiaoshuoFromWeb(QString &url,
                            QString &zuozhe, QString &leixing, QString &pingtai,
                            LianzaiZhuangtai &zhuangtai, ChubanZhuangtai &chuban);
    /// 修改正文，两个目录：作者+作品简介模式 mulu2=第二个一级目录
    void xiugaiZhengwenZuozheZuopinjianjie(const QString &mulu2);
    void tijiaoQiehuan();
    /// 修改书籍模板的信息栏,返回是否改动过,是true否false
    bool shujiXinxilan(const QString &qw, bool &nengShibie);
    /// 从正文获取书籍信息栏需要的内容
    bool getShujiFromQW(const QString &qw, QString zhengchangZifuN[]);
    /// 修改汉字模板的信息栏,返回是否改动过,是true否false
    bool hanziXinxilan(const QString &qw, bool &nengShibie);
    /// 从正文获取汉字信息栏需要的内容
    bool getHanziFromQW(const QString &qw, QString zhengchangZifuN[]);
    /// 从正文获取小说信息栏需要的内容
    bool xiaoshuoXinxilanQita(bool isQita, const QString &qw, bool &nengShibie);
    bool getXiaoshuoFromQW(const QString &qw, QString zhengchangZifuN[]);
public slots:
private:
    HttpReq *hr;
    bool xuyaoTijiao;//!<true=填表完成自动点击提交,false则不提交
};

#endif // MIGUYINYUE_H
