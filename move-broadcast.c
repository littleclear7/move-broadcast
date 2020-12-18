//
// Created by root on 2020/12/18.
//
#include <stdlib.h>
#include <stdio.h>
#include "move-broadcast.h"
#include "time.h"
#include "math.h"
UeBroadcastInfo *ueBroadcastInfo;
Point ueNowPoint;
int isInitRandom = 0; //初始化随机数种子标识

UeBroadcastInfo* initUeBroadcastInfo(){
    UeBroadcastInfo *ueBroadcastInfo1 = (UeBroadcastInfo *)calloc(0,sizeof(UeBroadcastInfo));
    ueBroadcastInfo1->cursor = 0;
    return ueBroadcastInfo1;
}

void init_move_system(int moveSeed){
    //初始化移动随机数种子
    if (isInitRandom == 0) {
        srand(time(0) + moveSeed);
        isInitRandom = 1;
    }
}

/*************************************************
 * 函数名：
 * 函数功能描述：          人物移动函数，返回要移动的x,y距离
 *                       首先随机生成移动方向，随后随机生成速度与时间，最终位置为方向*速度*时间
 * 函数参数：
 * @param           seed 移动种子，在第一执行移动时被初始化
 * @param             point   存放向x，y轴移动的距离（此处需要传入point *，用于存放结果）
 * 函数返回值          0  计算成功
 * @return
 * 作者：                           zt
 * 函数创建日期：                    2020/12/18
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:
 *
**************************************************/

int _get_move_distance(Point *point){
    //如果未被主动初始化move种子，那么会使用time(0)进行初始化，注意这样如果在同一时间多开，那么会导致每一个节点的移动轨迹都相同哦
    if ( isInitRandom == 0){
        printf("为初始化move种子，请先执行init_move_system(int moveSeed)!\n");
        return -1;
    }
    if (point == NULL)
        return -1;
    //设定速度与时间范围
    int speedMin = 1;
    int speedMax = 10;
    int timeMin = 2;
    int timeMax = 10;

    //生成角度
    double angle =  rand()%360;
    double movx =sin(angle*3.1415926/180);
    double movy = cos(angle*3.1415926/180);
    int speed = rand()%100;
    speed = speed * (speedMax - speedMin)/100 +speedMin;
    int time = rand()%100;
    time = time * (timeMax - timeMin)/100 +timeMin;
    int distince = speed*time;
    point->x = movx * distince;
    point->y = movy * distince;
    return 0;
}

/*************************************************
 * 函数名：
 * 函数功能描述：                  把当前节点(x,y)进行随机移动
 * 函数参数：
 * @param  			   point   节点现在的坐标，函数执行完成后会变为移动后坐标(此处需要传入point *，用于存放结果)
 * 函数返回值
 * @return
 * 作者：                           zt
 * 函数创建日期：                    2020/12/18
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:
 *
**************************************************/
void move_point(Point *point){
    if (point == NULL)
        return;
    Point distancePoint;
    int get_move_Re = _get_move_distance(&distancePoint);
    if(get_move_Re != 0)
        return;
    point->x += distancePoint.x;
    point->y += distancePoint.y;
    point->x = point->x > MAX_LATITUDE? MAX_LATITUDE: point->x;
    point->x = point->x < MIN_LATITUDE? MIN_LATITUDE: point->x;
    point->y = point->y > MAX_LONGITUDE? MAX_LONGITUDE : point->y;
    point->y = point->y < MIN_LONGITUDE? MIN_LONGITUDE : point->y;
    return ;
}

/*************************************************
 * 函数名：
 * 函数功能描述：                  获取两点之间的距离
 * 函数参数：
 * @param
 * 函数返回值
 * @return
 * 作者：                           zt
 * 函数创建日期：                    2020/12/18
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:
 *
**************************************************/
int get_points_distance(Point point1, Point point2){
    return sqrt((point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y));
}

void switchNewGnb(GnbPointInfo newGnbPoiont){
    //fixme:去掉printf会导致如果在后续继续使用printf会产生malloc(): corrupted top size错误
    printf("切换至新基站\n");
    ueBroadcastInfo->nowGnb = newGnbPoiont;
    return;
}

void addNewGnbInfo(GnbPointInfo new){
    ueBroadcastInfo->saveGnb[ueBroadcastInfo->cursor] = new;
    ueBroadcastInfo->cursor = (ueBroadcastInfo->cursor +1) % MAX_SAVE_GNB;
}

/*************************************************
 * 函数名：
 * 函数功能描述：                  判断当前ue是否需要切换
 * 函数参数：
 * @param
 * 函数返回值
 * @return                       0   需要切换
 * @return                       -1  不需要切换
 * 作者：                           zt
 * 函数创建日期：                    2020/12/18
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:
 *
**************************************************/
int needSwitch(){
    int distance = get_points_distance(ueNowPoint,ueBroadcastInfo->nowGnb.gnbPoint);
    if(distance < NEED_SWITCH)
        return -1;
    return 0;
}


int main(){
//    int x = 0, y = 0;
//    Point point;
//    point.x = 1;
//    point.y = 2;
//    Point point1;
//    point1.x = 5;
//    point1.y = 1;
//    int distance = get_points_distance(point1,point);
//    printf("%d\n",distance);
//    init_move_system(12);
//    for (int i = 0; i < 15; ++i) {
//        move_point(&point);
//        printf("gen:x : %d,y : %d\n",point.x,point.y);
//    }
    ueBroadcastInfo = initUeBroadcastInfo();
    GnbPointInfo new;
    new.gnbPoint.x =1;
    new.gnbPoint.y = -1;
    switchNewGnb(new);
//    ueNowPoint.x = 1;
//    ueNowPoint.y = 1;
    int r = needSwitch();
//    int t= 1;
//    printf("rrr\n");
    printf("rrr\n");
    return 0;
}