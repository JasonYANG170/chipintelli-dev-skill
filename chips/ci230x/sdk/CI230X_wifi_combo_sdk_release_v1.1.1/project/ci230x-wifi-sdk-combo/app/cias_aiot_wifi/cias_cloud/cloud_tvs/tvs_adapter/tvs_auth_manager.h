/*
 * @FileName:: 
 * @Author: 
 * @Date: 2022-05-10 19:17:11
 * @LastEditTime: 2022-05-20 16:43:35
 * @Description: 
 */
#ifndef __TVS_AUTH_MANAGER_H__
#define __TVS_AUTH_MANAGER_H__


void init_authorize_on_boot();

void start_authorize_with_client_id(const char* client_id);


// #define CLOUD_TVS_DEFAULT_PROFILE "{\r\n"  \
//                                   "\"DEVICE_CLIENT\": \"QQOPEN,101470979,719BDCB083A89DCEC7ABAB1D227A77B9,4D14648972218AE4155A6E78D24E2DD5,refreshToken,418670c8007b4f85,7776000,feb711004ae411e9b83f6713e1d602f3:db4eee2b600a49b3b7ee3e764513c61c,8989898989880000\",\r\n" \
//                                   "\"IOT_PRODUCT_ID\": \"ZDFVDMWJWB\",\r\n" \
//                                   "\"IOT_DEVICE_NAME\": \"D003\",\r\n" \
//                                   "\"IOT_DEVICE_SECRET\": \"pMnLQo3JlAHOMsjtfpgR+w==\"\r\n" \
//                                   "}\r\n"  \
//                                   "\r\n"
// #endif
#if 0
#define CLOUD_TVS_DEFAULT_PROFILE "{\r\n"  \
                                  "\"DEVICE_CLIENT\": \"QQOPEN,101470979,9C78976F12BAD3D527D4D5773017D38F,D53FA920E9791C7BF6B30CF3F3FE50A3,refreshToken,43733cc6009c4b18,7776000,feb711004ae411e9b83f6713e1d602f3:db4eee2b600a49b3b7ee3e764513c61c,8989898911280721\",\r\n" \
                                  "\"IOT_PRODUCT_ID\": \"ZDFVDMWJWB\",\r\n" \
                                  "\"IOT_DEVICE_NAME\": \"D003\",\r\n" \
                                  "\"IOT_DEVICE_SECRET\": \"pMnLQo3JlAHOMsjtfpgR+w==\"\r\n" \
                                  "}\r\n"  \
                                  "\r\n"
#endif
// #define CLOUD_TVS_DEFAULT_PROFILE "{\r\n"  \
//                                   "\"DEVICE_CLIENT\": \"QQOPEN,101470979,9C78976F12BAD3D527D4D5773017D38F,D53FA920E9791C7BF6B30CF3F3FE50A3,refreshToken,43733cc6009c4b18,7776000,feb711004ae411e9b83f6713e1d602f3:db4eee2b600a49b3b7ee3e764513c61c,8989898911280721\"\r\n" \
//                                   "}\r\n"  \
//                                   "\r\n"
#define CLOUD_TVS_DEFAULT_PROFILE "QQOPEN,101470979,9C78976F12BAD3D527D4D5773017D38F,D53FA920E9791C7BF6B30CF3F3FE50A3,refreshToken,43733cc6009c4b18,7776000,feb711004ae411e9b83f6713e1d602f3:db4eee2b600a49b3b7ee3e764513c61c,8989898911280721"
//AT+TENCENT_TVS_PROFILE="QQOPEN\,101470979\,9C78976F12BAD3D527D4D5773017D38F\,D53FA920E9791C7BF6B30CF3F3FE50A3\,refreshToken\,43733cc6009c4b18\,7776000\,feb711004ae411e9b83f6713e1d602f3:db4eee2b600a49b3b7ee3e764513c61c\,8989898911280721"
//使用AT指令的时候，逗号要用\转义，不然会AT指令会解析失败
 #endif
