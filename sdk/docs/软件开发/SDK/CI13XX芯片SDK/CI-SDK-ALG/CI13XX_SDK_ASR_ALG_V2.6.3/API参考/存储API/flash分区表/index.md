<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/flash%E5%88%86%E5%8C%BA%E8%A1%A8/ -->

# flash分区表

## 固件和分区

* 本文对固件、分区格式、分区表、固件内容等进行详细说明。
* 先解释几个名词：
* **固件**：PC串口打包升级工具（PACK\_UPDATE\_TOOL.exe），将多个bin文件打包生成Firmware\_V2.0.0.bin文件，该bin文件可被烧录到FLASH中，最终被引导程序和应用程序读取使用。
* **分区**：多个bin文件位于固件中的区域名为xxx分区。用户可自定义的分区有User、ASR、DNN、Voice、UserFile、NV data分区，其他为固定分区。
* **固件版本**：为了兼顾不同的应用场景，有多种不同格式的固件，例如FW\_V1、FW\_V2、FW\_V3这3种，将来可能会增加版本。
* **分区格式**：说明每个分区是什么，以及多个分区在固件中的排列顺序。
* **分区表**：包含固件各个组成部分的地址、大小、版本号等。
* **引导程序**：芯片上电后，先读取FLASH首地址存放发引导程序并跳转运行，再去指定地址读取分区表，解析分区表得到应用程序地址等信息，再将应用程序拷贝到SRAM运行。以下简称bootloader。
* **应用程序**：编译软件生成的user\_code.bin文件。
* 各分区分别存放的内容分别为：

| 分区名 | 存放内容 |
| --- | --- |
| User分区 | 存放编译软件生成的，应用程序user\_code.bin |
| ASR分区 | 存放语音识别需要的，语言模型文件集合asr.bin |
| DNN分区 | 存放语音识别需要的，声学模型文件集合dnn.bin |
| Voice分区 | 存放语音识别播放提示音需要的，音频文件集合voice.bin |
| UserFile分区 | 存放命令词信息表等文件集合user\_file.bin |
| NV data分区 | 存放用户自定义数据，由应用程序初始化和增删改 |

注意

* 打包固件时，只会把NV data的起始地址和预留大小写入分区表，并不包含NV data的内容，其他分区有内容。升级固件时，可选是否擦除NV data的数据。

### 1、固件（Firmware）

* 固件的生成请参考文档《快速入门.md》。
* 固件通常命名为Firmware\_V2.0.0.bin，Firmware为软件名称，V2.0.0为软件版本。
* 可在打包固件时自定义软件名称、软件版本，生成固件时会保存到分区表中。

### 2、引导程序（Bootloader）

* PC串口打包升级工具（PACK\_UPDATE\_TOOL.exe）内集成了各个系列芯片的引导程序，即bootloader.bin，打包固件时，会自动把bootloader.bin填充到固件的0地址。
* 每个系列都有1个单独的bootloader.bin，一般同1个系列的所有芯片型号共用1个bootloader.bin。
* 为兼顾不同固件版本，同1个系列也会使用不同bootloader.bin，由升级工具配置选择。

### 3、固件版本

* 现目前支持3种固件版本：FW\_V1、FW\_V2、FW\_V3。可通过PC串口打包升级工具（PACK\_UPDATE\_TOOL.exe）配置文件选配，打包哪种版本的固件。
* FW\_V1：对应ota V2 方案，使用具备ota 功能的bootloader\_a.bin（定义为BOOTloaderA） ，user code分区无备份，wifi ota 时下载BOOTloaderA 到sram 进行升级，wifi管理每个分区的升级逻辑。
* FW\_V2：无ota 功能的通用方案；bootloader\_b.bin只做引导【定义为BOOTloaderB】，无其他功能；固件分区无备份。
* FW\_V3：对应ota V3 方案，使用只做引导的的BOOTloaderB ，user code分区有备份，wifi ota时，wifi主要透传和管理版本，ota v3方案的代码进行ota升级
* 即FW\_V3和FW\_V1分区格式一样，只是使用不同的bootloader.bin
* FW\_V1和FW\_V2，不仅分区格式不同，还使用不同的bootloader.bin

### 4、分区格式

* 虽然固件版本有3种，但最根本的分区格式只有2种，分区格式1和分区格式2。
* PC串口打包升级工具（PACK\_UPDATE\_TOOL.exe）的打包界面，可供用户选择打包的文件，配置各分区预留大小等信息，打包固件时会将信息写到分区表中：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-3.png)

图1 分区配置

#### 4.1、分区格式1

* FW\_V1和FW\_V3版本固件，使用分区格式1排列，下图绿色标注是区别于分区格式2的部分：
* 按分区格式1打包固件时，会生成2份同样的分区表，2份同样的User分区，其他分区各1份。
* 用户可通过ota程序修改其中1个User分区的内容，实现更多场景的应用。

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-1.png)

图2 分区格式1

#### 4.2、分区格式2

* FW\_V2版本固件，按照分区格式2排列，也是最常用的，所有分区都只有1份：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-2.png)

图3 分区格式2

### 5、分区表

* 分区表的内容用于指示硬件名称、软件名称、各分区的起始地址、实际大小、版本号等。
* 应用程序需要的固件数据，都必须通过分区表来查找，下图描述分区表每个字段的含义：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-4.png)

图4 分区表格式

* 例如分区表的起始地址为0x2000，参照上图各信息地址，解析下图的分区表信息为：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-5.png)

图5 分区表示例图

| 分区表信息字段 | 查找的地址 | 解析的内容 |
| --- | --- | --- |
| 厂商编号 | 0x2000 + 0x0 | 0x64（100） |
| 产品编号 | 0x2000 + 0x4 | 0x64（100） |
| 硬件名称 | 0x2000 + 0xC | DEMO\_Board |
| 硬件版本 | 0x2000 + 0x4C | 0x20000（V2.0.0） |
| 软件名称 | 0x2000 + 0x50 | Firmware-V2 |
| 软件版本 | 0x2000 + 0x90 | 0x20000（V2.0.0） |
| bootloader版本 | 0x2000 + 0x94 | 0x100 （V0.1.0） |
| 芯片系列 | 0x2000 + 0x98 | CI130\*（CI13XX系列） |
| 分区格式 | 0x2000 + 0xA1 | 0x2（分区格式2） |
| PC串口升级工具版本 | 0x2000 + 0xA2 | P403（PC串口升级工具V4.0.3版本打包的固件） |
| user\_codec1分区当前版本 | 0x2000 + 0xA6 | 0x64（100） |
| user\_codec1分区起始地址 | 0x2000 + 0xAA | 0x4000 |
| user\_codec1分区实际大小 | 0x2000 + 0xAE | 0x2404C |
| user\_codec1分区CRC校验码 | 0x2000 + 0xB2 | 0xFE5F |
| user\_codec1分区状态 | 0x2000 + 0xB3 | 0xF0 |
| 其他分区当前版本、CRC校验码、状态 | - | 可参考user\_codec1解析 |
| user\_codec2分区起始地址 | 0x2000 + 0xBB | 0xFFFFFFFF（分区格式2无该分区） |
| user\_codec2分区实际大小 | 0x2000 + 0xBF | 0xFFFFFFFF（分区格式2无该分区） |
| ASR分区起始地址 | 0x2000 + 0xCC | 0x29000 |
| ASR分区实际大小 | 0x2000 + 0xD0 | 0x11559 |
| DNN分区起始地址 | 0x2000 + 0xDD | 0x3B000 |
| DNN分区实际大小 | 0x2000 + 0xE1 | 0x14DDA0 |
| Voice分区起始地址 | 0x2000 + 0xEE | 0x189000 |
| Voice分区实际大小 | 0x2000 + 0xAE | 0x936 |
| UserFile分区起始地址 | 0x2000 + 0xFF | 0x193000 |
| UserFile分区实际大小 | 0x2000 + 0x103 | 0xF87 |
| NV data分区起始地址 | 0x2000 + 0xAA | 0x1FC000 |
| NV data分区预留大小 | 0x2000 + 0xAE | 0x2404C |
| 分区表校验和 | 0x2000 + 0xAE | 0x4000 |

* 分区表地址为0x6000和0x8000时，可参考上述方法解析分区表内容
* 也能通过代码读取分区表的内容，可以3个地址的分区表都读一遍，根据校验和确定固件版本：

```
#include "flash_manage_outside_port.h"
#include "ci_flash_data_info.h"

//计算分区表校验和的函数
extern uint16_t get_partition_list_checksum(partition_table_t *file_config);

#define FILECONFIG_START_ADDR1     (0x2000)   //分区表1起始地址
#define FILECONFIG_START_ADDR2     (0x6000)   //分区表2起始地址
#define FILECONFIG_START_ADDR3     (0x8000)   //分区表3起始地址

partition_table_t partition_table = {0};

void read_partition_table()
{
  //读取分区表1内容，并计算校验和是否相等
  post_read_flash((char *)&partition_table,FILECONFIG_START_ADDR1,sizeof(partition_table_t)); 
  if (partition_table.patitiontablechecksum != get_partition_list_checksum(&partition_table))
  {
      //分区表1校验和错误，再读分区表2内容，并计算校验和是否相等
      post_read_flash((char *)&partition_table,FILECONFIG_START_ADDR2,sizeof(partition_table_t));
      if (partition_table.patitiontablechecksum != get_partition_list_checksum(&partition_table))
      {
        //分区表2校验和错误，再读分区表3内容，并计算校验和是否相等
        post_read_flash((char *)&partition_table,FILECONFIG_START_ADDR3,sizeof(partition_table_t));
        if (partition_table.patitiontablechecksum != get_partition_list_checksum(&partition_table))
        {
          //分区表3校验和错误
        }
        else
        {
          //分区表3校验和正确
        }
      }
      else
      {
        //分区表2校验和正确
      }
  }
  else
  {
      //分区表1校验和正确
  }
}
```

### 6、ASR分区文件排列格式

* 每个分区，都可以由多个文件组成，文件名用标签”[ID]”区分开。用户可以通过分区表得到各分区首地址，再解析分区首地址的文件信息头内容（文件信息头包含文件数量、各文件偏移地址和大小），去指定地址访问文件内容。
* ASR分区的asr.bin，可由多个文件合成，例如下图2个.dat文件合成asr.bin：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-9.png)

图9 合成asr.bin

* asr.bin的在固件中的文件排列格式如下：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-12.png)

图12 合成asr.bin

* 文件信息头各字段排列顺序如下：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-8.png)

图8 各分区文件头排列

：

* 例如ASR分区起始地址为0x29000，首地址的一部分内容为文件信息头，文件内容排在文件信息头后面：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-10.png)

图10 ASR分区内容

| 文件头信息字段 | 查找的地址 | 解析的内容 |
| --- | --- | --- |
| file\_number文件数量 | 0x29000 + 0x0 （上图红色框） | 0x01（只有1个文件） |
| file\_id第1个文件ID | 0x29000 + 0x2 （上图第1个蓝色框） | 0x0（第1个文件ID为0） |
| file\_addr第1个文件偏移地址 | 0x29000 + 0x4 （上图第1个黑色框） | 0x20（第1个文件偏移地址为0x29000+0x20） |
| file\_size第1个文件大小 | 0x29000 + 0x8 （上图第1个黄色框） | 0xA470 |
| file\_id第2个文件ID | 0x29000 + 0xC （上图第2个蓝色框） | 0x0（第1个文件ID为1） |
| file\_addr第2个文件偏移地址 | 0x29000 + 0xE （上图第2个黑色框） | 0x20（第1个文件偏移地址为0x29000+0xA490） |
| file\_size第2个文件大小 | 0x29000 + 0x12 （上图第2个黄色框） | 0x70C9 |

* 代码中定义了文件信息头的结构体如下：

```
//位于#include "ci_flash_data_info.h"中

typedef struct
{
    uint16_t file_id;      //文件ID
    uint32_t file_addr;    //文件偏移地址
    uint32_t file_size;    //文件大小
}file_header_t;

typedef struct
{
    uint16_t file_number;          //文件数量
    file_header_t file_header[1];  //单个文件头信息
}file_table_t;           //此结构当作变长数组用
```

### 7、DNN分区文件排列格式

* DNN分区文件dnn.bin，可由多个文件合成，例如将ID为[0]的文件合成dnn.bin：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-11.png)

图11 合成dnn.bin

* DNN分区文件排列格式同ASR分区，可参考ASR分区的示例解析DNN分区文件

### 8、UserFile分区文件排列格式

* UserFile分区的user\_file.bin，可由多个文件合成，例如下图1个“[60000]{智能管家}V2.xslx”文件合成user\_file.bin（其他bin为表格转换的中间文件）：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-6.png)

图6 合成user\_file.bin

* UserFile分区文件排列格式同ASR分区，可参考ASR分区的示例解析UserFile分区文件

### 9、Voice分区文件排列格式

* Voice分区的voice.bin，可由多个文件合成，例如下图10个音频文件合成voice.bin：

![模块框图](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E5%8F%82%E8%80%83/%E5%AD%98%E5%82%A8API/img/flash%E5%88%86%E5%8C%BA%E8%A1%A8-14.png)

图14 合成voice.bin

* Voice分区文件排列格式同ASR分区，可参考ASR分区的示例解析Voice分区文件

### 10、读各分区文件示例代码

* 下面示例代码，先读取UserFile分区第1个文件的内容：

```
#include "flash_manage_outside_port.h"
#include "ci_flash_data_info.h"

#define COMMAND_INFO_FILE_TEST_ID    60000   //要读取UserFile分区的文件ID为60000

partition_table_t partition_table = {0};     //分区表结构体

void read_user_file()
{
  //假定参考【5、分区表】章节的示例代码，已得到分区表的内容
  read_partition_table();  

  uint32_t user_file_addr = 0;
  uint32_t user_file_size = 0;
  uint32_t file_addr;
  //根据分区表的UserFile分区起始地址，得到ID为60000的文件偏移地址、文件大小
  if (get_file_addr(partition_table.user_file_offset, COMMAND_INFO_FILE_TEST_ID, &file_addr, &user_file_size))   
  {
    //得到ID为60000的文件在FLASH中的偏移地址
    user_file_addr = partition_table.user_file_offset + file_addr;  
    uint8_t * userfile_buff = pvPortMalloc(user_file_addr);
    //传入ID为60000的文件在FLASH中的偏移地址、文件大小，读取文件内容
    post_read_flash((char *)userfile_buff,user_file_addr,user_file_size); 
  }
}
```

* 或者使用包好的接口，读取各分区文件的地址和大小，再读取文件内容：

```
//位于#include "ci_flash_data_info.h"中

uint32_t get_userfile_addr(uint16_t file_id, uint32_t *p_file_addr, uint32_t *p_file_size)
uint32_t get_dnn_addr_by_id(uint16_t dnn_file_id, uint32_t *p_dnn_addr, uint32_t *p_dnn_size)
uint32_t get_asr_addr_by_id(int asr_id, uint32_t *p_asr_addr, uint32_t *p_asr_size)
uint32_t get_voice_addr_by_id(uint16_t * voice_id_buffer, uint32_t * voice_addr_buffer, uint32_t voice_num)

//此类接口的示例，请参考SDK，相互需要关联使用。
```