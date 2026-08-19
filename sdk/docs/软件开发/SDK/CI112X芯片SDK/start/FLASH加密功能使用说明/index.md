<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/start/FLASH%E5%8A%A0%E5%AF%86%E5%8A%9F%E8%83%BD%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# FLASH加密功能使用说明

---

## 1. FLASH加密的目的

FLASH加密的目的是为了防止应用开发商开发的产品被不良厂商通过复制FLASH的方式，抄袭生产。如果不以此为目的，使用该加密功能将无任何意义。使用此加密功能后，如果FLASH固件被COPY到另一块FLASH中，将提示校验失败（用户也可以自定义其他处理方式）。

---

## 2. SDK支持的加密方式

SDK支持两种加密方式，一种是用户自定义加密算法，提供给升级工具和SDK调用，此种方式能让用户的加密算法得到保密，安全性更高，但复杂性和工作量也相对较大一些。另一种是SDK自带默认加密算法，用户只需要在升级工具和SDK中输入相同的密码，用户使用简单。

### 2.1. 默认加密算法方式

此种加密方式，使用简单，用户开发量小，对安全性要求不高的客户可以使用此式。

#### 2.1.1. SDK的相关修改

* 修改user\_config.h, 开启加密功能，COPYRIGHT\_VERIFICATION 定义为1；
* 修改user\_config.h, 设置加密方式，ENCRYPT\_ALGORITHM 默认定义为 ENCRYPT\_DEFAULT；
* 修改ci\_flash\_data\_info.c, 在函数 ci\_flash\_data\_info\_init中，copyright\_verification2 被调用之前，修改加密密码为自己的密码，如下图；

![修改加密密码](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/start/img/FLASH%E5%8A%A0%E5%AF%86%E5%8A%9F%E8%83%BD%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.jpg)

图2-1 修改加密密码

* 修改修改ci\_flash\_data\_info.c, copyright\_verification2 被调用之后，校验失败的处理方式，SDK中的示例代码是死循环不停打印校验失败的信息，建议用户修改此种方式，因为这种提示方式容易被逆向工程破解。

#### 2.1.2. 升级工具的使用

* 在用升级工具更新固件时，选择加密算法为“标准加密”，填入密码，如下图：

![升级工具的使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/start/img/FLASH%E5%8A%A0%E5%AF%86%E5%8A%9F%E8%83%BD%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-2.jpg)

图2-2 升级工具的使用

* 打开串口正常升级即可。

### 2.2. 用户自定义加密算法方式

此种加密方式，使用稍微复杂一些，用户开发量稍微大一点，对安全性要求高一点客户可以使用此式。

#### 2.2.1. 自定义加密算法接口

##### c++ 接口：

```
//psrc:     源数据buffer
//src_len:  源数据长度
//pdst:     输出buffer，用于保存加密结果
//dst_len:  输出buffer大小
//out_len:  用于输出结果数据的长度
//注意: 如果pdst为NULL,则只计算结果数据的长度
bool func(char *psrc, int src_len, char *pdst, int dst_len, int *out_len)
```

##### python 接口：

```
def encrypt(data):
    out_data = your_encrypt(data)   #示例代码，用户需要修改为自己的算法
    return out_data
```

#### 2.2.2. SDK的相关修改

* 修改user\_config.h, COPYRIGHT\_VERIFICATION 定义为1；
* 修改user\_config.h, ENCRYPT\_ALGORITHM 定义为 ENCRYPT\_USER\_DEFINE；
* 按照2.2.1节自定义加密算法接口所给接口编写自定义加密算法函数；
* 修改ci\_flash\_data\_info.c, 在函数 ci\_flash\_data\_info\_init中，调用copyright\_verification1的时候，修改参数，传入自定义的加密函数，如下图；

![SDK的相关修改](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/start/img/FLASH%E5%8A%A0%E5%AF%86%E5%8A%9F%E8%83%BD%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-3.png)

图2-3 SDK的相关修改

* 修改ci\_flash\_data\_info.c, copyright\_verification1 被调用之后，校验失败的处理方式，SDK中的示例代码是死循环不停打印校验失败的信息，建议用户修改此种方式，因为这种提示方式容易被逆向工程破解。

#### 2.2.3. 升级工具的使用

* 按照2.2.1节自定义加密算法接口所给接口编写自定义加密算法函数，当前升级工具支持的调用方式有：C++动态链接库(.dll)；
* 在用升级工具更新固件时，选择加密算法为“自定义加密算法”，填入密码，如下图；

![升级工具的使用](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI112X%E8%8A%AF%E7%89%87SDK/start/img/FLASH%E5%8A%A0%E5%AF%86%E5%8A%9F%E8%83%BD%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-4.png)

图2-4 升级工具的使用

* 打开串口正常升级即可。

---

## 3. 适用环境

需要每颗FLASH芯片有唯一的unique id。