/*============================================================================*/
/* @file exe_profile_hid.c
 * @brief EXE Profile layer, including DIS, BAS, NUS (Nordic UART Service).
 * @author onmicro
 * @date 2021/03
 */

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts.h"
#include "exe_hal.h"
#include "exe_api.h"
#include "exe_app.h"
#include "svn_rev.h"

/* Profile built-in support. */
#define PROFILE_SIG_GATT        1
#define PROFILE_SIG_DIS         1
#define PROFILE_SIG_BAS         1
#define PROFILE_VENDOR_UART      1

/**
 * @brief Abbr. in ATT database.
 * PS:  Primary Service, its value ={ServiceUUID}
 *  CD:  Char Declaration, its value ={Char Properties, Char Value Handle, Char UUID}
 *   CV:   Char Value
 *   CCCD: Client Char Config Descriptor, its value CCC=[notification | indication]
 *   RRD:  Report Reference Descriptor,   its value RR={Report ID, Report Type[in|out]}
 */
typedef enum
{
  ATT_HANDLE_BEGIN = 0,

  /* Generic Access. */
  ATT_HANDLE_GAP_PS,            //1800
    ATT_HANDLE_GAP_DEVNAME_CD,    //2A00: Read
    ATT_HANDLE_GAP_DEVNAME_CV,      //utf8s
	ATT_HANDLE_GAP_APPEARANCE_CD, //2A01: Read
	ATT_HANDLE_GAP_APPEARANCE_CV,
	ATT_HANDLE_GAP_PPCP_CD,       //2A04: Read
    ATT_HANDLE_GAP_PPCP_CV,

#if (PROFILE_SIG_GATT)
  /* Generic Attribute. */
  ATT_HANDLE_GATT_PS,	        //1801
    ATT_HANDLE_GATT_SCC_CD,	      //2A05: Indicate
	ATT_HANDLE_GATT_SCC_CV,
	ATT_HANDLE_GATT_SCC_CCCD,     //2902: CCC
#endif

#if (PROFILE_SIG_DIS)
  /* Device Information. */
  ATT_HANDLE_DIS_PS,            //180A
    ATT_HANDLE_DIS_PNP_CD,        //2A50: Read
    ATT_HANDLE_DIS_PNP_CV,
#endif

#if (PROFILE_SIG_BAS)
  /* Battery. */
  ATT_HANDLE_BAS_PS,            //1812
    ATT_HANDLE_BAS_BAT_LVL_CD,    //2A19: Read | Notify
    ATT_HANDLE_BAS_BAT_LVL_CV,
    ATT_HANDLE_BAS_BAT_LVL_CCCD,  //2902: CCC
#endif

#if (PROFILE_VENDOR_UART)
  /* Nordic UART Service. */
  ATT_HANDLE_VENDOR_UART_PS,   //UUID128
    ATT_HANDLE_UART_TX_CD,       //Vendor: Read | Notify
    ATT_HANDLE_UART_TX_CV,
    ATT_HANDLE_UART_TX_CCCD,     //2902: CCC

    ATT_HANDLE_UART_RX_CD,       //Vendor: Write | Write_noRSP
    ATT_HANDLE_UART_RX_CV,
#endif

  ATT_HANDLE_END,
} att_handles_set_t;

/* Service UUIDs. */
static const uint16_t _gap_service_uuid = BLE_UUID_GAP;                    //0x1800
static const uint16_t _gatt_service_uuid = BLE_UUID_GATT;                  //0x1801
static const uint16_t _dis_service_uuid = BLE_UUID_DEVICE_INFORMATION_SERVICE;     //0x180a
static const uint16_t _bas_service_uuid = BLE_UUID_BATTERY_SERVICE;                //0x180f

/* Properites. */
static const uint8_t _properties_read = BLE_CHAR_PROP_READ;
static const uint8_t _properties_write = BLE_CHAR_PROP_WRITE;
static const uint8_t _properties_indicate = BLE_CHAR_PROP_INDICATE;
static const uint8_t _properties_writenak = BLE_CHAR_PROP_WRITE_WITHOUT_RSP;
static const uint8_t _properties_read_notify = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_NOTIFY;
static const uint8_t _properties_read_writenak = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_WRITE_WITHOUT_RSP;
static const uint8_t _properties_read_write_writenak = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_WRITE | BLE_CHAR_PROP_WRITE_WITHOUT_RSP;
static const uint8_t _properties_read_write = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_WRITE;
static const uint8_t _properties_read_writenak_notify = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_WRITE_WITHOUT_RSP | BLE_CHAR_PROP_NOTIFY;
static const uint8_t _properties_write_writenak = BLE_CHAR_PROP_WRITE | BLE_CHAR_PROP_WRITE_WITHOUT_RSP;

/* GAP & GATT */
static const uint16_t _gap_appearance_value = APP_GAP_APPEARANCE;
#if (PROFILE_SIG_GATT)
static uint16_t _gatt_service_changed_value[4] = {0};
static uint8_t _gatt_service_changed_ccc_value[2] = {0};
#endif
uint8_t profile_unified_ccc_value[2];

#if (PROFILE_SIG_DIS)
#define USB_IF_VID            12994
#if defined(SVN_REV_NUM)
#define FW_VERSION_ID         SVN_REV_NUM
#else
#define FW_VERSION_ID         4268
#endif
static const uint8_t _dis_pnp_value[] = {
  0x02,
  USB_IF_VID & 0xff, USB_IF_VID >> 8, /* LSB */
  0x20, 0x62,
  FW_VERSION_ID & 0xff, FW_VERSION_ID >> 8, /* LSB */
};
#endif

uint8_t bas_bat_lvl_value = 100;

#if (PROFILE_VENDOR_UART)
static const uint8_t _vendor_uart_service_uuid[16] = {
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e
};
#endif
static uint8_t vendor_uart_tx_value[EXE_ATT_PAYLOAD_LEN_MAX];
static uint8_t vendor_uart_rx_value[EXE_ATT_PAYLOAD_LEN_MAX];

/* UUID indexes for smaller code size. */
#define _UUID_IDX_PS                    0
#define _UUID_IDX_CD                    3
#define _UUID_IDX_CCCD                  4
#define _UUID_IDX_RRD                   6
#define _UUID_IDX_CHAR_GAP_DEVNAME      8
#define _UUID_IDX_CHAR_GAP_APPEARANCE   9
#define _UUID_IDX_CHAR_GAP_PPCP         10
#define _UUID_IDX_CHAR_GATT_SC          11
#define _UUID_IDX_CHAR_FF00             14
#define _UUID_IDX_CHAR_FF01             15
#define _UUID_IDX_CHAR_BAS_BAT          16
#define _UUID_IDX_CHAR_HID_INFO         17
#define _UUID_IDX_CHAR_HID_RMAP         18
#define _UUID_IDX_CHAR_HID_CPOINT       19
#define _UUID_IDX_CHAR_HID_REPORT       20
#define _UUID_IDX_CHAR_HID_PMODE        21
#define _UUID_IDX_CHAR_DIS_PNP          22
const uint16_t profile_uuids_set_sig[] = {
  //0
  BLE_UUID_SERVICE_PRIMARY,    //0x2800
  BLE_UUID_SERVICE_SECONDARY,  //0x2801
  BLE_UUID_SERVICE_INCLUDE,    //0x2802
  BLE_UUID_CHARACTERISTIC,     //0x2803
  BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG, //0x2902
  BLE_UUID_DESCRIPTOR_SERVER_CHAR_CONFIG, //0x2903
  BLE_UUID_REPORT_REF_DESCR,              //0x2908
  0,

  //8
  BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME,      //0x2A00
  BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE,       //0x2A01
  BLE_UUID_GAP_CHARACTERISTIC_PPCP,             //0x2A04
  BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED, //0x2A05
  0,0,
  0xFF00,
  0xFF01,

  //16
  BLE_UUID_BATTERY_LEVEL_CHAR,     //0x2A19
  BLE_UUID_HID_INFORMATION_CHAR,   //0x2A4A
  BLE_UUID_REPORT_MAP_CHAR,        //0x2A4B
  BLE_UUID_HID_CONTROL_POINT_CHAR, //0x2A4C
  BLE_UUID_REPORT_CHAR,            //0x2A4D
  BLE_UUID_PROTOCOL_MODE_CHAR,     //0x2A4E
  BLE_UUID_PNP_ID_CHAR,            //0x2A50

  //23
  
};

/* Vendor UART Service's TX charactoristic is used to send data to the central with Notification. */
#define _UUID128_IDX_CHAR_UART_TX        0
/* Vendor UART Service's RX charactoristic is used to receive data from the central with Write. */
#define _UUID128_IDX_CHAR_UART_RX        1
#define _UUID128_IDX_CHAR_OTA_DATA       2
const uint8_t profile_uuids_set_vendor[][16] = {
  {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e},
  {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e},
  {0x12,0x2B,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00},
};


const exe_att_ent_t exe_gtbl_gatt_database[] =
{
  /* The total number of ATT handlers. */
  {ATT_HANDLE_END-1, 0, 0, 0, 0},

  /* Generic Access PS: DevName CD,CV; Appearance CD,CV; PPCP CD,CV. */
  {7, 2, 2,                    _UUID_IDX_PS, (uint8_t*)(&_gap_service_uuid)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, EXE_ADV_DEV_NAME_LEN, _UUID_IDX_CHAR_GAP_DEVNAME, (uint8_t*)(exe_gbuf_adv_ind+EXE_ADV_DEV_NAME_OFF)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, 2,                    _UUID_IDX_CHAR_GAP_APPEARANCE, (uint8_t*)(&_gap_appearance_value)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, 8,                    _UUID_IDX_CHAR_GAP_PPCP, (uint8_t*)(&exe_gu16_gap_ppcp_value[0])},

#if (PROFILE_SIG_GATT)
  /* Generic Attribute PS: SCC CD,CV,CCCD. */
  {4, 2, 2,                    _UUID_IDX_PS, (uint8_t*)(&_gatt_service_uuid)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_indicate)},
  {0, 2, 4,                    _UUID_IDX_CHAR_GATT_SC, (uint8_t*)(_gatt_service_changed_value)},
  {0, 2, 2,                    _UUID_IDX_CCCD, (uint8_t*)(_gatt_service_changed_ccc_value)},
#endif

#if (PROFILE_SIG_DIS)
  /* Device Information PS: PnP CD,CV. */
  {3, 2, 2,                    _UUID_IDX_PS, (uint8_t*)(&_dis_service_uuid)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, sizeof(_dis_pnp_value),    _UUID_IDX_CHAR_DIS_PNP, (uint8_t*)(_dis_pnp_value)},
#endif

#if (PROFILE_SIG_BAS)
  /* Battery PS: BatteryLevel's CD,CV,CCCD. */
  {4, 2, 2,	 _UUID_IDX_PS, (uint8_t*)(&_bas_service_uuid)},
  {0, 2, 1,	 _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0, 2, 1,	 _UUID_IDX_CHAR_BAS_BAT, (uint8_t*)(&bas_bat_lvl_value)},
  {0, 2, 2,	 _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
#endif

#if (PROFILE_VENDOR_UART)
  /* Vendor UART PS: READ's CD,CV,CCCD; WRITE's CD,CV */
  {6, 2, 16, _UUID_IDX_PS, (uint8_t*)(_vendor_uart_service_uuid)},
  {0, 2, 1,  _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0,16, sizeof(vendor_uart_tx_value), _UUID128_IDX_CHAR_UART_TX, (uint8_t*)(vendor_uart_tx_value)},
  {0, 2, 2,  _UUID_IDX_CCCD, (uint8_t*)(&profile_unified_ccc_value)},
  {0, 2, 1,  _UUID_IDX_CD, (uint8_t*)(&_properties_write_writenak)},
  {0,16, sizeof(vendor_uart_rx_value), _UUID128_IDX_CHAR_UART_RX, (uint8_t*)(vendor_uart_rx_value)},
#endif
};

/* Dummy routines to make Compatible with the existing sdk library. */
uint8_t vendor_ota_data_value[20];
const uint8_t *hid_get_report_map(void)
{ return NULL; }
int hid_get_report_map_size(void)
{ return 0; }
void hid_notification_enable(uint8_t flag)
{
  profile_unified_ccc_value[0] = flag;
}
uint8_t hid_notification_is_enabled(void)
{
  return profile_unified_ccc_value[0];
}
bool hid_tx_notification(hid_data_type_t data_type)
{ return false; }

bool bas_tx_notification(uint8_t level)
{
  bool ret = false;

  if (level > 100) {
    level = 100;
  }
  if (bas_bat_lvl_value != level) {
    bas_bat_lvl_value = level;
    if (profile_unified_ccc_value[0] & 0x01/*notify*/) {
      ret = exe_att_tx_notification(ATT_HANDLE_BAS_BAT_LVL_CV, &bas_bat_lvl_value, sizeof(bas_bat_lvl_value));
    }
  }
  return ret;
}

bool nus_tx_notification(uint8_t *p_data, uint8_t len)
{
  bool ret = false;

  if (len > EXE_ATT_PAYLOAD_LEN_MAX) {
    len = EXE_ATT_PAYLOAD_LEN_MAX;
  }
  if (profile_unified_ccc_value[0] & 0x01/*notify*/) {
    ret = exe_att_tx_notification(ATT_HANDLE_UART_TX_CV, p_data, len);
  }
  /* Sync to tx value if this data comes from rx value in loopback. */
  if (p_data != vendor_uart_tx_value) {
    memcpy(vendor_uart_tx_value, p_data, len);
    memset(vendor_uart_tx_value+len, 0x00, EXE_ATT_PAYLOAD_LEN_MAX-len);
  }

  return ret;
}
