# coding:utf-8
import os
import sys
import json
import collections
import shutil
import subprocess
import tarfile
from tools.templates.my_depend import my_depend
from pathlib import Path

# Parameter description:
# $1 - Application project path: apps/tuyaos_zigbee_light_sample
# $2 - Application project name: tuyaos_zigbee_light_sample
# $3 - Application project version: 1.0.0
# $4 - Application project command: build or clean
# python3 build_app.py apps/tuyaos_zigbee_light_sample tuyaos_zigbee_light_sample 1.0.0 clean

# Get the path to the current python interpreter
PYTHON_PATH = sys.executable

# firmware info parser
def device_type_info_collect():
    global DEV_TYPE
    global CHIP_ID
    global BUILD_TOOL
    print("firmware information parse start...")
    DEV_TYPE = dev_info_check()
    CHIP_ID = chip_id_get()
    BUILD_TOOL = build_tool_get()
    if ('router'==DEV_TYPE) or ('sleep_end_dev'==DEV_TYPE):
        print("firmware information parse success")
        print(DEV_TYPE)
        print(CHIP_ID)
    else:
        print("firmware information dev_role parse fail")

# board name
def get_board_name(path):
    for root, dirs, files in os.walk(path):
        return dirs[0]

# remove folder
def rm_dir(path):
    def readonly_handler(func, path, execinfo):
        os.chmod(path, stat.S_IWRITE)
        func(path)

    if os.path.exists(path):
        shutil.rmtree(path, onerror=readonly_handler)


def exe(note,cmd):
    print(note)
    ret = subprocess.call(cmd,shell=True)
    if ret != 0:
        print("execution failed !!!")
        sys.exit(1)

print(len(sys.argv))
if len(sys.argv) < 4:
    print("Script parameter error !!!")

DEMO_PATH = sys.argv[1].replace('\\','/')
DEMO_NAME = sys.argv[2].replace('\\','/')
DEMO_FIRMWARE_VERSION = sys.argv[3]
DEMO_OUTPUT_PATH = "_output"

BUILD_COMMAND = 'build'
PARAMS1 = 'NONE'

if len(sys.argv) == 5:
    BUILD_COMMAND = sys.argv[4]

if len(sys.argv) == 6:
    BUILD_COMMAND = sys.argv[4]
    DEMO_OUTPUT_PATH = sys.argv[5] 
    PARAMS1 = sys.argv[5]

# app project product name and path
CURR_PATH = os.getcwd()
ROOT_PATH = CURR_PATH

APP_BUILD_PATH = DEMO_PATH + '/_build' #os.path.join(DEMO_PATH, '_build')
APP_INCLUDE_PATH = DEMO_PATH + '/include' #os.path.join(DEMO_PATH, 'include')
APP_SRC_PATH = DEMO_PATH + '/src' #os.path.join(DEMO_PATH, 'src')

COPY_SOURCE_PATH = ROOT_PATH + '/tools/templates/'#os.path.join(ROOT_PATH, 'tools/templates/')
APP_FOLDER_PATH = os.path.join(ROOT_PATH, DEMO_PATH)

BOARD_NAME = get_board_name('./vendor')

if not os.path.exists(DEMO_PATH):
    print(DEMO_PATH + "not exist!!!")
    sys.exit(1)

print("DEMO_PATH: " + DEMO_PATH)
print("DEMO_NAME: " + DEMO_NAME)
print("DEMO_FIRMWARE_VERSION: " + DEMO_FIRMWARE_VERSION)
print("BOARD_NAME: " + BOARD_NAME)
print("BUILD_COMMAND: " + BUILD_COMMAND)

PYTHON_PATH     = '"'+sys.executable+'"'
SCRIPT_IDE_TOOL = PYTHON_PATH + ' ./.ide_tool/ide_tool.py'
SCRIPT_PREPARE  = PYTHON_PATH + ' ./vendor/'+BOARD_NAME+'/prepare.py'

if BUILD_COMMAND == "config":
    CONFIG_AUTO = 0
    if PARAMS1 == 'gui':
        ENABLE_GUI = 0
    else:
        ENABLE_GUI = 1
    cmd = "%s menuconfig \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %d %d"%(SCRIPT_IDE_TOOL, './', DEMO_PATH, DEMO_NAME, DEMO_FIRMWARE_VERSION, BOARD_NAME, CONFIG_AUTO, ENABLE_GUI)
    exe("config...", cmd)

if BUILD_COMMAND == "build":

    my_obj = my_depend()
    my_obj.check()

    from tools.templates.prebuild import Pre_build
    from tools.templates.postbuild import Post_build
    from tools.templates.dev_info_check import dev_info_check,chip_id_get,build_tool_get,json_transform_to_yaml
    # from tools.OTA.tl_check_fw import OTA_build

    if not os.path.exists(APP_INCLUDE_PATH):
        os.makedirs(APP_INCLUDE_PATH)

    if not os.path.exists(APP_SRC_PATH):
        os.makedirs(APP_SRC_PATH)
        
    os.chdir('tools/templates')

    json_transform_to_yaml()

    device_type_info_collect()

    if BUILD_TOOL != 'IAR' and BUILD_TOOL != 'GCC':
        BUILD_TOOL = 'IAR'

    if BUILD_TOOL == 'GCC':

        if not os.path.exists('../../tools/gcc/bin'):
            filename = '../../vendor/zigbee_efr32mg21/toolchain/gcc.tar.gz'
            print('uncompressing gcc.tar.gz ... ')
            tf = tarfile.open(filename)
            tf.extractall('../../tools')  

        print('--- Build tool:'+BUILD_TOOL)
        print('gcc end')
        Pre_build()

        Post_build(DEV_TYPE, BUILD_COMMAND, CHIP_ID)
        os.chdir(CURR_PATH)

        python_path = 'python3 ./tools/templates/ota-generate.py'
        para = "%s \"%s\" \"%s\" \"%s\""%(python_path, DEMO_PATH, DEMO_NAME, DEMO_FIRMWARE_VERSION)
        ret = subprocess.call(para)
        if ret != 0:
            print("ota-generate.py execution failed !!!")
            sys.exit(1)

    elif BUILD_TOOL == 'IAR':
        os.chdir(CURR_PATH)
        cmd = "%s pr-build \"%s\" \"%s\" \"%s\" \"%s\" \"%s\""%(SCRIPT_PREPARE, DEMO_PATH,BOARD_NAME,DEMO_OUTPUT_PATH,DEMO_NAME,DEMO_FIRMWARE_VERSION)
        exe("build-pre...", cmd)
        
        cmd = "%s build"%(SCRIPT_PREPARE)
        exe("build...", cmd)

if BUILD_COMMAND == "clean":
    print("clean...")
    rm_dir('.log')
    rm_dir(APP_BUILD_PATH)
