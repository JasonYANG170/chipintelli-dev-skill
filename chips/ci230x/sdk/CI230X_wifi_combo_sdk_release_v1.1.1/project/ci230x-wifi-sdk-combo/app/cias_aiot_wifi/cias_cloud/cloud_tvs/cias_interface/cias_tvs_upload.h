/*
 * @Description: TVS接口入口声明
 * @Author: wuhongchuan
 * @Date: 2021-11-16 10:11:04
 * @LastEditTime: 2021-11-17 10:44:32
 * @LastEditors: liutq
 * @Reference: 
 */
#ifndef CIAS_TVS_UPLOAD_H
#define CIAS_TVS_UPLOAD_H

typedef struct 
{
  bool tvs_connect_state; 
}TvsHalParamTypedef;

int cias_tvs_main(void);
void set_tvs_network_state(bool sta);
bool get_tvs_network_state(void);

#endif