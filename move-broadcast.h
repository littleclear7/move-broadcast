//
// Created by root on 2020/12/18.
//

#ifndef MOVE__BROADCAST_MOVE_BROADCAST_H
#define MOVE__BROADCAST_MOVE_BROADCAST_H

//这四个值构成了移动范围
#define MAX_LATITUDE 100  //最大纬度
#define MIN_LATITUDE -100  //最小纬度
#define MAX_LONGITUDE 100 //最大经度
#define MIN_LONGITUDE -100 //最小经度
#define MAX_SAVE_GNB  10

#define NEED_SWITCH  50//ue需要进行切换的距离阙值，当大于此值时ue可以进行切换

/*
 * 每个实体的位置坐标表示
 */
typedef struct {
    int x;
    int y;
}Point;

/*
 *记录接收到的基站的相关信息，包含基站id、ip、port、point
 */
typedef struct {
    char gnbId[20];
    char GnbIp[20];
    int gnbPort;
    Point gnbPoint;
}GnbPointInfo;

typedef struct {
    char gnbId[20];
    char GnbIp[20];
    int gnbPort;
    Point gnbPoint;
}St_Broadcast_packet;

/*
 * 记录UE的一些信息，包括当前基站的位置，当前收到的基站位置，UE自己的位置
 */
typedef struct {
    pthread_mutex_t infoLock;
    Point ueNowPoint; //ue当前位置信息

    GnbPointInfo nowGnb;

    int saveNums; //；记录当前存储的基站数量
    int cursor; //记录当前存储基站数组游标,指向的位置是下一个可以存储的位置
    GnbPointInfo saveGnb[MAX_SAVE_GNB];
}UeBroadcastInfo;

/*************************************************
 * 函数名：               initUeBroadcastInfo
 * 函数功能描述：          初始化ue接收广播结构体
 * 函数参数：
 * @param
 * 函数返回值
 * @return
 * 作者：                           zt
 * 函数创建日期：                    2020/12/21
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:
 *
**************************************************/
UeBroadcastInfo* initUeBroadcastInfo();

#endif //MOVE__BROADCAST_MOVE_BROADCAST_H
