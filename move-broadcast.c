//
// Created by root on 2020/12/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "move-broadcast.h"
#include "time.h"
#include "math.h"
#include "pthread.h"
#include "string.h"

UeBroadcastInfo *ueBroadcastInfo; //ue端接受基站广播切换工作结构体
int isInitRandom = 0; //初始化随机数种子标识

UeBroadcastInfo* initUeBroadcastInfo(){
    UeBroadcastInfo *ueBroadcastInfo1 = (UeBroadcastInfo *)calloc(sizeof(UeBroadcastInfo),1);
    pthread_mutex_init(&ueBroadcastInfo1->infoLock,NULL);
    ueBroadcastInfo1->cursor = 0;
    ueBroadcastInfo1->saveNums = 0;
    return ueBroadcastInfo1;
}

void init_move_system(int moveSeed){
    //初始化移动随机数种子
    if (isInitRandom == 0) {
        srand(time(0) + moveSeed);
        isInitRandom = 1;
    }
    return;
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

/*************************************************
 * 函数名：
 * 函数功能描述：                  将节点切换至新的基站（只做修改节点的基站信息）
 * 函数参数：
 * @param
 * 函数返回值
 * @return                         0  切换成功
 * @return                         -1 切换失败
 * 作者：                           zt
 * 函数创建日期：                    2020/12/21
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:                           需加锁
 *
**************************************************/
int switchNewGnb(GnbPointInfo newGnbPoiont){
    if (ueBroadcastInfo != NULL) {
//        pthread_mutex_lock(&ueBroadcastInfo->infoLock);
        ueBroadcastInfo->nowGnb = newGnbPoiont;
//        pthread_mutex_unlock(&ueBroadcastInfo->infoLock);
        return 0;
    }
    return -1;
}



/*************************************************
 * 函数名：
 * 函数功能描述：                  检测是否已经存储过该gnb
 * 函数参数：
 * @param
 * 函数返回值
 * @return                          0 已经存储过该gnb
 * @return                          -1 未存储过该gnb
 * 作者：                           zt
 * 函数创建日期：                    2020/12/21
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:
 *
**************************************************/

int isSaveGnb(GnbPointInfo new){
    for (int i = 0; i < ueBroadcastInfo->saveNums; ++i) {
        if (strcmp(ueBroadcastInfo->saveGnb[i].gnbId, new.GnbIp) == 0)
            return 0;
    }
    return -1;
}

/*
 * 向广播工作结构体加入新的基站信息
 * 需加锁
 */
void addNewGnbInfo(GnbPointInfo new){
    if (ueBroadcastInfo == NULL)
        return;
//    pthread_mutex_lock(&ueBroadcastInfo->infoLock);
    if (isSaveGnb(new) == 0)
        return;
    ueBroadcastInfo->saveGnb[ueBroadcastInfo->cursor] = new;
    ueBroadcastInfo->cursor = (ueBroadcastInfo->cursor +1) % MAX_SAVE_GNB;
    ueBroadcastInfo->saveNums == MAX_SAVE_GNB ?  :  (ueBroadcastInfo->saveNums)++;
//    pthread_mutex_unlock(&ueBroadcastInfo->infoLock);
    return;
}

/*************************************************
 * 函数名：                         
 * 函数功能描述：      查找要切换的基站，如果当前基站为信号最优基站，则不需要做切换。
 * 函数参数：
 * @param  			
 * 函数返回值               GnbPointInfo*  查找成功，返回要切换的基站的info信息    
 *                         NULL          当前基站以为最优基站，无需进行切换
 * @return 
 * 作者：                           zt
 * 函数创建日期：                    2020/12/21
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:              需加锁
 * 		
**************************************************/
GnbPointInfo *chose_switch_gnb(){
    if (ueBroadcastInfo == NULL)
        return NULL;
//    pthread_mutex_lock(&ueBroadcastInfo->infoLock);
    int nowDistance = get_points_distance(ueBroadcastInfo->ueNowPoint, ueBroadcastInfo->nowGnb.gnbPoint);
    int minDistance = nowDistance;
    int result = -1;
    for (int i = 0; i < ueBroadcastInfo->saveNums; ++i) {
        int tmpDistance = get_points_distance(ueBroadcastInfo->ueNowPoint,ueBroadcastInfo->saveGnb[i].gnbPoint);
        if(minDistance > tmpDistance) {
            minDistance = tmpDistance;
            result = i;
        }
    }
//    pthread_mutex_unlock(&ueBroadcastInfo->infoLock);
    if(result == -1)
        return NULL;
    GnbPointInfo *gnbPointInfo1 = calloc(sizeof(GnbPointInfo),1);
    *gnbPointInfo1 = ueBroadcastInfo->saveGnb[result];
    return gnbPointInfo1;

}

/*************************************************
 * 函数名：
 * 函数功能描述：                  判断当前ue是否需要切换
 * 函数参数：
 * @param
 * 函数返回值
 * @return                       0   需要切换
 * @return                       -1  不需要切换
 * @return                       -2  需要先初始化ueBroadcastInfo
 * 作者：                           zt
 * 函数创建日期：                    2020/12/18
 * 目前版本：                       v1.0.0
 * 历史版本：                       v1.0.0
 * 备注:                            需加锁
 *
**************************************************/
int needSwitch(){
    if (ueBroadcastInfo == NULL){
        printf("need init ueBroadcastInfo!\n");
        return -2;
    }
//    pthread_mutex_lock(&ueBroadcastInfo->infoLock);
    int distance = get_points_distance(ueBroadcastInfo->ueNowPoint,ueBroadcastInfo->nowGnb.gnbPoint);
//    pthread_mutex_unlock(&ueBroadcastInfo->infoLock);
    if(distance < NEED_SWITCH)
        return -1;
    return 0;
}

/*
 * 开始移动，会加锁
 */
void start_move(int moveSeed){
    printf("moveSeed:%d\n",moveSeed);
    init_move_system(5);
    while (1){
        sleep(2);
        pthread_mutex_lock(&ueBroadcastInfo->infoLock);
        move_point(&ueBroadcastInfo->ueNowPoint);
        printf("now place: x : %d, y : %d\n",ueBroadcastInfo->ueNowPoint.x,ueBroadcastInfo->ueNowPoint.y);
        pthread_mutex_unlock(&ueBroadcastInfo->infoLock);
    }
}

void create_move_thread(int moveSeed){
    pthread_t tid;
    pthread_create(&tid,NULL,start_move,moveSeed);
    pthread_detach(tid);
}

int start_broadcast(int sockFd, void *args) {
    socklen_t addrLen = sizeof(struct sockaddr);
    struct sockaddr_in userAddr;
//    unsigned char recvBuf[D_NSP_TDYTH_NAAAS_5G_UE_BUFF_LEN] = {0};
//    unsigned char sendBuf[D_NSP_TDYTH_NAAAS_5G_UE_BUFF_LEN] = {0};
    unsigned char recvBuf[] = {0};
    unsigned char sendBuf[] = {0};
    unsigned char type, subType, step;
    int recvRet, sendLen = 0;
    while (1) {
        recvRet = recvfrom(sockFd, recvBuf, sizeof(recvBuf) - 1, 0, (struct sockaddr *) (&userAddr), &addrLen);
        if (-1 == recvRet) {
//            log_e("recvfrom fail\n");
            break;
        }
    }
}



int main(){
#if 0 //切换基站选择函数测试
    ueNowPoint.x = 0;
    ueNowPoint.y = 0;
    ueBroadcastInfo = initUeBroadcastInfo();
    ueBroadcastInfo->nowGnb.gnbPoint.y = 5;
    ueBroadcastInfo->nowGnb.gnbPoint.x = 5;
    GnbPointInfo gnbPointInfo;
    gnbPointInfo.gnbPoint.y = 1;
    gnbPointInfo.gnbPoint.x = -1;
    addNewGnbInfo(gnbPointInfo);
//    GnbPointInfo *gnbPointInfo1 = calloc(sizeof(GnbPointInfo),1);
//    *gnbPointInfo1 = ueBroadcastInfo->saveGnb[0];
    GnbPointInfo *gnbPointInfo1 = chose_switch_gnb();
#endif
    ueBroadcastInfo = initUeBroadcastInfo();
    create_move_thread(5);
    getchar();
    return 0;
}