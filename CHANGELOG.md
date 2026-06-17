# 3.13.0 - 2026/03/16

## 摘要

> TuyaOS ZigBee开发框架3.13.0版本，支持`EFR32MG21A020Fx`平台`Router/End device`设备。

## 添加

> 1. 原厂 SDK 由 Znet 6.10.3更新到 Znet 7.5.0。
> 2. 增加压缩能力，可在 app_config.yaml 文件，通过 `lzma_enable: True ` 实现。
> 3. 增加安全 OTA 能力，可在 app_config.yaml 文件，通过 `secure_ota_enable: True ` 实现。
> 4. 增加允许强电配网过程中被ZLL遥控器拉取的功能。
> 5. 增加 wwah 能力支持。
>
> 

> ## 修改
>
> 1. 优化配网及运行稳定性。
> 2. 修复了一些已知问题。

# 3.12.1 - 2025/10/22

## 摘要

> TuyaOS ZigBee开发框架3.12.1版本，支持`EFR32MG21A020Fx`平台`Router/End device`设备。

## 添加

> 1. 增加tkl_zg_scene_add_or_store_pre_handle_callback接口，允许不对add scene或者store scene做处理。
>
>    

# 3.12.0 - 2025/08/19

## 摘要

> TuyaOS ZigBee开发框架3.12.0版本，支持`EFR32MG21A020Fx`平台`Router/End device`设备。

## 添加

> 1. 增加 GCC 编译支持，可在 app_config.yaml 文件，chip_id 后一行添加 `build_tool: "GCC" #IAR/GCC`。
>
> 2. 增加允许强电配网过程中被ZLL遥控器拉取的功能。
>
> 3. tkl nwk增加 tkl_zg_logic_and_radio_channel_set，设置逻辑和物理信道。
>
> 4. tkl 层 adc，gpio，pwm，timer驱动更改为虚函数。
>
>    

> ## 修改
>
> 1. 优化配网及运行稳定性。
> 2. 修复了一些已知问题。

# 3.11.0 - 2025/02/13

## 摘要

> TuyaOS ZigBee开发框架3.11.0版本，支持`EFR32MG21A020Fx`平台`Router/End device`设备。

## 添加

> 1. 增加 Zigbee 分布式联动功能，使用详见：https://www.tuyaos.com/viewtopic.php?p=15359#p15359。
> 2. 能力值扩展至11个字符长度，向前兼容。

## 修改

> 无


# 3.9.1 - 2024/11/25

## 摘要

> TuyaOS ZigBee开发框架3.9.1版本，支持`EFR32MG21A020Fx`平台`Router/End device`设备。

## 添加

> 无

## 修改

> 1. 修复时间溢出`0x7FFFFFFF`后造成的上报异常及ram耗尽问题。
> 2. 修复`post build`文件中`APP`及`OTA`区域大小判断错误。



# 3.9.0 - 2023/12/20

## 摘要

> TuyaOS ZigBee开发框架3.9.0版本，支持`EFR32MG21A020Fx`平台`Router/End device`设备。

## 添加

> 1. 增加了`tkl_timer`硬件定时器驱动；
> 2. 增加私有属性，支持网关读低功耗设备心跳周期，以确定设备离线时间；
> 3. 支持应用配置仅使用`installcode`配网的接口：`VOID_T tal_zg_nwk_join_by_installcode_only_enable(BOOL_T enable)`；

## 修改

> 1. 修改分布式网络`link key`，支持`Hue`网关；
> 2. 优化群升`OTA`，提高了整体升级成功率；
> 3. 最大支持`endpoint`数量增大至8个；
> 4. 允许`OTA`版本降级(需要网关支持)；
> 5. 优化了`green power`功能；
> 6. `router`设备配网成功后默认开启`permit join`，之前为关闭；
> 7. 更新了`NV`组件；
> 8. 应用工程配置文件由`appconfig.json`更新为`app_config.yml`，提升友好性；
> 9. 应用工程支持配置标准`manufacturer name` 和`model identifier`属性；
> 10. 修复了一些已知问题。

**接口兼容问题说明**

| 3.9.0 API                                       | 旧版本API           | 说明                                                         |
| ----------------------------------------------- | ------------------- | ------------------------------------------------------------ |
| `tal_zg_poll_forever_with_param`                | `tal_zg_poll_start` | 功能一致                                                     |
| `tal_zg_poll_recover_param`                     | `tal_zg_poll_stop`  | 功能一致                                                     |
| `tal_get_group_ota_capa_frist_report_delaytime` | 无                  | 虚函数<br/>可配置设备上电上报群升能力值延时，默认是`45s+15s`随机，规避过早发送造成大规模路由请求 |
| `tal_zg_nwk_join_by_installcode_only_enable`    | 无                  | 应用配置仅使用`installcode`配网                              |



# 3.6.1 - 2022/11/30

## 摘要

> 此版本支持了`EFR32MG21A020F1024`平台`End device`设备、`EFR32MG21A020F768`平台`Router/End device`设备。
>

## 添加

> 无
>

## 修改

> 1. 优化了flash map，768平台支持更大的应用固件；
> 2. 产测支持读installcode功能；
> 3. 读当前时间戳接口统一为zigbee标准时间戳；
> 4. 开启固件签名检查；
>



# 3.5.2 - 2022/09/20

## 摘要

> 此版本支持了`EFR32MG21A020F1024`平台`End device`设备、`EFR32MG21A020F768`平台`Router/End device`设备。
>

## 添加

> 1. 支持`EFR32MG21A020F1024`平台`End device`设备、`EFR32MG21A020F768`平台`Router/End device`设备；
>

## 修改

> 1. 生成`DIFF`文件格式脚本修改；
> 2. 生成OTA文件大小超限时，更新提示；
>



# 3.5.0 - 2022/07/15

## 摘要

> 此版本主要是修复已知问题，增加了一些功能。仅支持`EFR32MG21A020F1024`平台`Router`设备。
>

## 添加

> 1. 增加了用户自定义扩展信息接口；
> 2. 增加了ZLL identify回调接口；
> 3. 增加国家码限制功率；
>

## 修改

> 1. 修复双dongle产测时，芯科协议栈裸数据发送接口特殊格式数据被过滤的问题；
> 2. 修复低功耗回调函数注册 ;
> 3. 修复原厂协议栈部分场景会修改发射功率的问题；
> 4. 修复ZLL client端扫描异常停止的问题；
>



# 3.4.0 - 2022/04/10

## 摘要

> 此版本主要是调整 vendor 目录结构，释放 TKL/TAL 层的部分接口和实现，方便用户使用原厂 SDK 的驱动接口封装特殊的驱动接口。
>

## 添加

> 1. 增加 vendor 目录，释放TKL/TAL 层的部分接口和实现；
>

## 修改

> 1. PWM 频率自适应问题修复；
> 2. 设备入网完成后禁止发送 permit joining request，如有需要手动开启 ;
> 3. 修改out of band 入网方式无法自动指定node id 的问题；
>

## 更改

> ​	1.更新 TKL/TAL 驱动接口；



# 3.3.0 - 2022/03/17

## 摘要

> 支持 **EFR32MG21F1024IM32** 和 **EFR32MG21F768IM32** 两款芯片，并支持全功能的 Router 设备开发和低功耗设备开发。
>
> 注意：目前低功耗设备不支持网络自恢复功能，需要在 Zigbee 初始化完成后手动调用 **tal_zg_nwk_recovery_disable(TRUE)** 来禁止网络自恢复功能。
>

## 添加

> 1. 支持 EFR32MG21F1024IM32 芯片低功耗设备开发；
> 2. 支持 EFR32MG21F768IM32 芯片 Router 设备开发和开发低功耗设备；
> 3. 支持 innerpan 数据收发；
> 4. 支持网关下发 poll 控制指令；
> 5. 数据接收接口新增字段标识数据来源是client or server;
>

## 更改

> 1. 修复PWM 频率无法动态调整的问题；
> 2. 修改 heartbeat 支持在心跳回调中电池电量上报；
> 3. 修复 ADC 单次采样后没有关闭 ADC，导致功耗增加的问题；
>



