#include "cias_flash_update.h"

update_result_param_info_st gupdate_result_param_info; // and by yjd

//升级结果信息写入flash
int update_result_param_info_write(void)
{
	int rc = 0;
#if 0
  //将信息写入到备份分区-and by yjd
  bk_flash_enable_security(FLASH_PROTECT_NONE);
  bk_flash_erase(BK_PARTITION_IOT_UPDATE_PARAM, 0, sizeof(update_result_param_info_st));
  rc = bk_flash_write(BK_PARTITION_IOT_UPDATE_PARAM, 0, &gupdate_result_param_info, sizeof(update_result_param_info_st));
  bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
  CIAS_LOG_INFO("write-gupdate_result_param_info.curent_update_flag = %d\r\n", gupdate_result_param_info.curent_update_flag);
  CIAS_LOG_INFO("write-gupdate_result_param_info.current_update_partition_index = %d\r\n", gupdate_result_param_info.current_update_partition_index);
  CIAS_LOG_INFO("gupdate_result_param_info.current_update_partition_status = 0x%x\r\n", gupdate_result_param_info.current_update_partition_status);
  CIAS_LOG_INFO("gupdate_result_param_info.update_type = %d\r\n", gupdate_result_param_info.update_type);
  CIAS_LOG_INFO("gupdate_result_param_info.current_update_partition_offset = 0x%x\r\n", gupdate_result_param_info.current_update_partition_offset);
#endif
	return rc;
}
//设置固件升级状态为成功
int set_update_status_to_success(void)
{
	update_result_param_info_read();
	gupdate_result_param_info.current_update_partition_status = 0x0d; // 0f-更新失败，0e-更新成功，0d-还未更新
	gupdate_result_param_info.current_update_partition_offset = 0x0;
	update_result_param_info_write();
}
//设置固件升级状态为失败
int set_update_status_to_fail(void)
{
	update_result_param_info_read();
	gupdate_result_param_info.current_update_partition_status = 0x0f; // 0f-更新失败，0e-更新成功，0d-还未更新
	gupdate_result_param_info.current_update_partition_offset = 0x0;
	update_result_param_info_write();
}

//从flash读出升级结果信息
int update_result_param_info_read(void)
{

  int rc = 0;
#if 0
  rc = bk_flash_read(BK_PARTITION_IOT_UPDATE_PARAM, 0, &gupdate_result_param_info, sizeof(update_result_param_info_st)); //读升级参数信息
  CIAS_LOG_INFO("read-gupdate_result_param_info.current_update_partition_index = %d\r\n", gupdate_result_param_info.current_update_partition_index);
  CIAS_LOG_INFO("gupdate_result_param_info.current_update_partition_status = 0x%02x\r\n", gupdate_result_param_info.current_update_partition_status);
  CIAS_LOG_INFO("gupdate_result_param_info.update_type = %d\r\n", gupdate_result_param_info.update_type);
  CIAS_LOG_INFO("gupdate_result_param_info.current_update_partition_offset = 0x%x\r\n", gupdate_result_param_info.current_update_partition_offset);
  CIAS_LOG_INFO("gupdate_result_param_info.iot_img_net_address.img_data_len = 0x%x\r\n", gupdate_result_param_info.iot_img_net_address.img_data_len);
  CIAS_LOG_INFO("gupdate_result_param_info.iot_img_net_address.img_data = %s\r\n", gupdate_result_param_info.iot_img_net_address.img_data);
#endif  
  return rc;

}
int update_img_address_param_init(void)
{
	/*
	//将信息写入到备份分区-and by yjd
	bk_flash_enable_security(FLASH_PROTECT_NONE);
	bk_flash_erase(BK_PARTITION_IOT_UPDATE_PARAM, sizeof(update_result_param_info_st), sizeof(iot_img_net_address_st));
	bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);*/
}