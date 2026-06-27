
#ifndef __TUYA_MF_TEST_H__
#define __TUYA_MF_TEST_H__


#ifdef __cplusplus
    extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tal_uart.h"

/**
 * @enum test command
 */
typedef enum {
    TUYA_MF_TEST_LED_ON_ALL,         ///< Switch on all LED
    TUYA_MF_TEST_LED_OFF_ALL,        ///< Switch off all LED
    TUYA_MF_TEST_LED_BLINK_ALL,      ///< Blink all LED
    TUYA_MF_TEST_RELAY_ON_ALL,       ///< Switch on all relay
    TUYA_MF_TEST_RELAY_OFF_ALL,      ///< Switch off all relay
    TUYA_MF_TEST_RELAY_BLINK_ALL,    ///< Blink all relay
    TUYA_MF_TEST_BUTTON,             ///< Button testing
    TUYA_MF_TEST_GET_ELEC_INFO,      ///< Get electricity information
    TUYA_MF_TEST_ADJUST_ELEC_INFO,   ///< Calibration quantity information
    TUYA_MF_TRANSFER,                ///< transfer rare test data
}TUYA_MF_TEST_CMD_T;


/**
 * @enum Test data source
 */
typedef enum {
    TUYA_MF_TEST_SRC_UART = 0, ///< from uart
    TUYA_MF_TEST_SRC_ZIGBEE,   ///< from zigbee gateway
    TUYA_MF_TEST_SRC_BEACON,   ///< from beacon
    TUYA_MF_TEST_SRC_DONGLE,   ///< from double dongle
}TUYA_MF_TEST_SRC_T;

/**
 * @enum testing result, dev_mf_test_callback using
 */
typedef enum {
    TUYA_MF_TEST_SUCCESS,        ///< Test success
    TUYA_MF_TEST_FAILED,         ///< Test failure
    TUYA_MF_TEST_DOING           ///< Testing is underway
}TUYA_MF_TEST_RET_T;



typedef VOID_T(*TUYA_TEST_EXIT_CB_T)(VOID_T);
typedef TUYA_MF_TEST_RET_T (*TUYA_TEST_HANDLE_CB_T)(TUYA_MF_TEST_CMD_T cmd, UINT8_T*arg, UINT16_T arg_len);


/**
 * @brief disable  beacon mf test
 *
 * @param[in]   none
 * @return  none
 */
VOID_T tal_mf_test_disable_beacon_test(VOID_T);

/**
 * @brief button mf test notify
 *
 * @param[in]   key_id: key id(start at index 0)
 * @return  none
 */
VOID_T tal_mf_test_button_test_notify(UINT_T key_id);

/**
 * @brief sensor mf test notify
 *
 * @param[in]   sensor_type: sensor type(see mf test protocol)
 * @param[in]   index:       sensor index(see mf test protocol)
 * @param[in]   result:      sensor result(LOW level or HIGHT level)
 * 
 * @return  none
 */
VOID_T tal_mf_test_bool_sensor_notify(UINT8_T sensor_type, UINT8_T index, BOOL_T result);

/**
 * @brief mf test result notify
 *
 * @param[in]   result: mf test result
 * @return  none
 */
VOID_T tal_mf_test_result_notify(TUYA_MF_TEST_RET_T result);

/**
 * @brief mf test general response send
 *
 * @param[in]   cmd: mf test command
 * @param[in]   len: length of response data
 * @param[in]   data: point to response data
 * @return  none
 */
VOID_T tal_mf_test_general_send(UINT8_T cmd, UINT8_T len, UINT8_T *data);

/**
 * @brief pre-enter mf test will call this function(only use for double dongle test)
 * @note < this is a weak function you can rewrite this API in app layer >
 *
 * @param[out]   args: not used
 * @return none
 */
VOID_T tal_mf_test_pre_start_callback(VOID_T *args);

/**
 * @brief end mf test will call this function(only use for double dongle test)
 * @note < this is a weak function you can rewrite this API in app layer >
 *
 * @param[out]   data: point mf test result(TRUE/FALSEZ)
 * @return none
 */
VOID_T tal_mf_test_end_callback(UINT8_T *data);

/**
 * @brief beacon mf test enter will call this function(only used for beacon mf test)
 * @note < this is a weak function you can rewrite this API in app layer >
 *
 * @param[in]   none
 * @return none
 */
VOID_T tal_beacon_mf_test_callback(VOID_T);

/**
 * @brief mf test handle callback(uart mf test or zigbee gateway mf test or doubl dongle mf test)
 * @note < this is a weak function you can rewrite this API in app layer >
 *
 * @param[in]   cmd: mf tset command
 * @param[in]   arg: point to mf tset data(see mf test protocol)
 * @param[in]   arg_len: data len
 * @return none
 */
TUYA_MF_TEST_RET_T tal_mf_test_handle_callback(TUYA_MF_TEST_CMD_T cmd,
                                                UINT8_T*arg, 
                                                UINT16_T arg_len);

/**
 * @brief uart mf test uart config
 * @note < this is a weak function you can rewrite this API in app layer >
 *
 * @param[out]   out_uart_id: point to uart number(only support 0)
 * @param[out]   out_cfg:     point to uart config
 * @return none
 */
VOID_T tal_mf_test_get_uart_cfg(TUYA_UART_NUM_E *out_uart_id, TAL_UART_CFG_T *out_cfg);

/**
 * @brief get zg db list according country code
 * 
 * @param[out] out_db_list: db list
 * @param[in] db_list_sums: db list sums 
 * @param[out] out_min_db: min db
 * @return TRUE: success; FALSE: failed 
 */
BOOL_T tal_mf_get_zg_db_list(INT_T *out_db_list, INT_T db_list_sums, INT_T *out_min_db);

#ifdef __cplusplus
}
#endif

#endif

