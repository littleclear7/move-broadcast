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

typedef struct {
    char gnbId[20];
    char GnbIp[20];
    int gnbPort;
    Point gnbPoint;
}GnbPointInfo;

/*
 * 记录UE的一些信息，包括当前基站的位置，当前收到的位置
 */
typedef struct {
    GnbPointInfo nowGnb;

    int cursor; //记录当前存储基站数组游标,指向的位置是下一个可以存储的位置
    GnbPointInfo saveGnb[MAX_SAVE_GNB];
}UeBroadcastInfo;
#endif //MOVE__BROADCAST_MOVE_BROADCAST_H
