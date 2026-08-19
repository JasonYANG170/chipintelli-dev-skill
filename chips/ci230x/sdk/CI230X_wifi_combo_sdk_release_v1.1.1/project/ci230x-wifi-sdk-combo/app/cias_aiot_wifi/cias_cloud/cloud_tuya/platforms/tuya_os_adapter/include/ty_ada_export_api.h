/**
 * @file ty_ada_export_api.h
 * 
 * The purpose of this file is not to change the original header file
 * in tuya_os_adapter/include provided by tuya.
*/

#ifndef __TY_ADA_EXPORT_API_H__
#define __TY_ADA_EXPORT_API_H__

/**
 * bt adapter interface
*/
int ty_ada_bt_init(void);
int ty_ada_bt_hw_ready_notify(void);

#endif /* __TY_ADA_EXPORT_API_H__ */
