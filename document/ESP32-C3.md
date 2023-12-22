# ESP32-C3使用手册

## 安装

VS Code+ESP-IDF扩展 网上都有

[小陈笔记(一):Vscode 安装ESP-IDF插件\_vscode esp-idf-CSDN博客](https://blog.csdn.net/A06537/article/details/132118541)

[使用 VS Code 快速搭建 ESP-IDF 开发环境 (Windows、Linux、MacOS) - 知乎](https://zhuanlan.zhihu.com/p/630698425)

[Vscode搭建ESP-IDF开发环境指南-Ubuntu ESP32-C3 合宙\_vscode+esp-idf-CSDN博客](https://blog.csdn.net/weixin_51954217/article/details/130462085)

## 选择ESP-IDF Board

在VSCode中使用ESP-IDF时，你需要根据你的连接方式选择不同的ESP-IDF Board选项。下面是每个选项的含义：

- ESP32-C3 chip (via ESP-PROG): 这个选项表示你使用了一个外部的ESP-PROG调试器，通过JTAG接口连接到ESP32C3芯片。这种方式可以实现高速的烧写和调试功能，但需要额外的硬件和连接线。
- ESP32-C3 chip (via builtin USB-JTAG): 这个选项表示你使用了板子上自带的USB  OTG接口，通过USB协议连接到ESP32C3芯片，并利用芯片内置的USB-JTAG功能进行烧写和调试。这种方式不需要额外的硬件，但速度可能比ESP-PROG慢一些。
- ESP32-C3 chip (via ESP USB Bridge):  这个选项表示你使用了板子上自带的USB转UART桥接器，通过串口连接到ESP32C3芯片，并利用芯片内置的ROM引导加载器进行烧写。这种方式也不需要额外的硬件，但速度可能比USB-JTAG慢一些，并且不支持调试功能。
   根据你的实际情况，如果你只想简单地烧写应用程序，并且不需要调试功能，那么你可以选择**ESP32-C3 chip (via ESP USB Bridge)**选项，并且使用标注为COM的TypeC口连接到电脑。如果你想要调试功能，并且不想购买额外的硬件，那么你可以选择**ESP32-C3 chip (via builtin USB-JTAG)**选项，并且使用标注为USB的TypeC口连接到电脑。如果你想要最快的烧写和调试速度，并且愿意购买额外的硬件，那么你可以选择**ESP32-C3 chip (via ESP-PROG)**选项，并且使用ESP-PROG调试器连接到电脑和板子。

## 工程文件结构

### 通用工程目录结构树

```
- myProject/
             - CMakeLists.txt
             - sdkconfig
             - components/ - component1/ - CMakeLists.txt
                                         - Kconfig
                                         - src1.c
                           - component2/ - CMakeLists.txt
                                         - Kconfig
                                         - src1.c
                                         - include/ - component2.h
             - main/       - CMakeLists.txt
                           - src1.c
                           - src2.c

             - build/
```

### 简单的LED工程目录结构

```
- LED/
             - CMakeLists.txt	# 顶层项目 CMakeLists.txt 文件，这是 CMake 用于学习如何构建项目的主要文件，可以在这个文件中设置项目全局的 CMake 变量。
             - sdkconfig		# 项目配置文件，执行 idf.py menuconfig 时会创建或更新此文件，文件中保存了项目中所有组件（包括 ESP-IDF 本身）的配置信息。
             - README.md
             - components/ 		# 自定义组件总目录
             				- led/ 	# 自定义组件目录，其命名通常为某个驱动硬件名
             						- CMakeLists.txt	# 自定义组件的 CMakeLists.txt 文件，用来添加本组件所用的头文件路径等。
                                    - led.c				# 自定义组件源文件，用来编写相关硬件的初始化函数、驱动函数等。
                                    - include/ 			# 自定义组件头文件目录，用来存放自定义组件源文件的头文件。
                                    			- led.h		# 自定义组件头文件。
             - main/   			# 项目主函数目录   
             				- CMakeLists.txt	# 主函数目录 CMakeLists.txt 文件。
                            - main.c			# 项目主函数。

             - build/			# 编译文件存放目录，若没有创建此目录，执行 idf.py build 命令时会自动生成此文件夹。
```

## 下载

按住boot按键，然后插usb，这样可以进入下载模式，就可以直接下载了。

## 问题

### VScode 找不到头文件（头文件下有波浪线），实际编译是成功的

**解决方法：**将工程中的c_cpp_properties.json文件内容改成如下

```json
{
    "configurations": [
        {
            "name": "ESP-IDF",
            "compileCommands": "${workspaceFolder}/build/compile_commands.json",
            "compilerPath": "${config:idf.toolsPath}\\tools\\riscv32-esp-elf\\esp-12.2.0_20230208\\riscv32-esp-elf\\bin\\riscv32-esp-elf-gcc.exe",
            "includePath": [
                "${config:idf.espIdfPath}/components/**",
                "${config:idf.espIdfPathWin}/components/**",
                "${config:idf.espAdfPath}/components/**",
                "${config:idf.espAdfPathWin}/components/**",
                "${workspaceFolder}/**"
            ],
            "browse": {
                "path": [
                    "${config:idf.espIdfPath}/components",
                    "${config:idf.espIdfPathWin}/components",
                    "${config:idf.espAdfPath}/components/**",
                    "${config:idf.espAdfPathWin}/components/**",
                    "${workspaceFolder}"
                ],
                "limitSymbolsToIncludedHeaders": false
            }
        }
    ],
    "version": 4
}

```



## 参考

[【ESP32-IDF+VScode】开发笔记（一）：从点灯开始——点亮LED\_vscode esp32-CSDN博客](https://blog.csdn.net/weixin_43803230/article/details/131511563)

[【导航】ESP32 教学篇目录 【快速跳转】\_esp32 网页切换-CSDN博客](https://blog.csdn.net/m0_50064262/article/details/120250151)