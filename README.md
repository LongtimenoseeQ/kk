## TuyaOS subdev-zg 开发包介绍

## 简介

TuyaOS Zigbee 子设备开发包适用于开发符合 Zigbee 3.0 标准的智能产品。该开发包在芯片原厂 SDK 基础上进行了二次抽象，屏蔽了复杂的 Zigbee 技术细节，融入了涂鸦特色功能，方便您快速入门。主要包含原厂 vendor sdk、涂鸦特色功能 libs 库、硬件接口、网络接口、 组件、工具等部分，并在 APP 下提供若干品类的示例代码，来展示各种接口的使用和 Zigbee 设备接入涂鸦体系的基本开发规范。

## 目录介绍

```
.
├─ hardware
│  ├─ chip_manual
│  └─ module_manual
├─ pc
│  └─ tools
├─ TuyaOS
│  ├─apps
│  │  └─tuyaos_demo_zg_light2
│  │  └─tuyaos_demo_zg_door_sensor
│  ├─components
│  ├─docs
│  ├─include
│  │  ├─adapter
│  │  ├─base
│  │  └─components
│  ├─libs
│  ├─scripts
│  ├─tools
│  └─vendor
└─ tuya.json
```

| 目录名称  | 作用                                                      |
| :------ | :-------------------------------------------------------- |
| `hardware`     | 包含硬件模组相关资料                       |
| `pc`     | 包含原厂 PC 工具                    |
| `apps`     | 包含有 demo 代码，新建项目只需进行新建文件夹并在文件夹下添加相关代码即可 |
| `components`| 包含涂鸦开发的各类开源组件，用户添加组件需要新建目录即可，然后将组件代码放在此文件夹下 |
| `doc`     | 包含相关说明文档                       |
| `include` | 包含 TuyaOS 子设备开发包各类 API 接口文件 |
| `lib`     | 包含 TuyaOS 子设备开发包依赖的库文件 |
| `scripts` | 包含相关脚本文件 |
| `tools`   | 包含应用在构建和编译过程中所用到的相关工具和脚本 |
| `vendor`  | 包含涂鸦优化过后的芯片原厂 SDK               |

