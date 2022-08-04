# MMFreeLyrics 
## 初音未来 Project DIVA MEGA 39's PC (Mega Mix+) FreeType 歌词渲染模块

    本模块将利用 FreeType + ImGui 接管游戏对 PV 歌词的内部渲染，以达成更优的视觉体验
    兼容版本 ： 1.0.2 

## 功能
- 游戏内样式修改 (附带 ImGui)
    - 字体热加载 （TTF/OTF/...)
    - 字体大小
    - 字体描边
    - 字体投影
    - 字体颜色
    - 窗口样式
    - ...
- 配置文件支持
- 兼容歌词 MOD
- 兼容超长(78字)以上歌词
- 加载外挂 `*.srt` 歌词

## MOD 演示
[[MOD 演示] 初音未来 Project DIVA MEGA 39's FreeType 歌词渲染](https://www.bilibili.com/video/BV1ha411N7qQ)

## MOD 安装
- 使用 [DivaModManager](https://github.com/TekkaGB/DivaModManager) 安装
    - 或直接使用 [DivaModLoader](https://github.com/blueskythlikesclouds/DivaModLoader)
    - 可参考 [初音未来 Project DIVA MEGA39’s＋帧数解锁和歌曲汉化MOD安装流程](https://www.bilibili.com/read/cv16871246)，并在游戏根目录下的 `config.toml` 中 `priority` 项添加 `MMFreeLyrics`
- 在 [Release](https://github.com/mos9527/MMFreeLyrics/releases) 下载本 MOD
- 解压至游戏文件夹 `mods\MMFreeLyrics` 目录
- 配置见 *MOD 使用*
- 正确配置的模组目录应有如下结构
    
        HATSUNE MIKU PROJECT DIVA MEGA MIX PLUS\MODS\MMFREELYRICS
        │   brotlicommon.dll
        │   brotlidec.dll
        │   bz2.dll
        │   bz2d.dll
        │   charset.txt
        │   config.toml
        │   freetype.dll
        │   freetyped.dll
        │   libpng16.dll
        │   libpng16d.dll
        │   MMFreeLyrics.dll
        │   zlib1.dll
        │   zlibd1.dll
        │
        └───fonts
                font_1.ttf
                font_2.otf
                ...
- 启动游戏

## MOD 使用
### 歌词 MOD 兼容性
- 如果还使用了歌词翻译 MOD 且遇到缺字问题：
    - 将翻译模组文件夹内的 `pv_db.txt` 内容复制于本模组目录下 `charset.txt` 内
    - 尝试调整 `Fonts` 中 `Fallback` 字体
    - 如果可能，请将该 `pv_db.txt` 提交 Issues
### 字体安装
- 在模组目录中创建文件夹 `fonts`，将自己想用的字体文件(`.ttf`,`.otf`) 放入其中
    - 注：文件名最好使用英文。中文名称能够被使用，但字体名不会被正常显示。
### 外挂字幕
- 在 MOD 目录新建 `lyrics` 文件夹
- 将准备好的字幕文件放入 lyrics
- 更名为 [PVID].srt
    - PVID 可在游戏内歌词窗口得知
- 确定歌词窗口 `Attempt to load (and use) external SubRip Lyrics (applies on song start)` 打勾
- 字幕会在每次 PV / 节奏游戏 开始前加载并自动构建字库
### 热键
|热键|功能|
|-|-|
Ctrl+F|打开游戏内字体窗口
Ctrl+G|打开游戏内歌词窗口
Ctrl+S|保存配置
Ctrl+L|载入配置

----

## References
https://github.com/ocornut/imgui

https://github.com/ajkhoury/SigMaker-x64

https://github.com/BroGamer4256/score-mm

https://github.com/ActualMandM/DivaDllMods

https://github.com/blueskythlikesclouds/DivaModLoader

https://github.com/nastys/MEGAHAKUS

https://github.com/saurabhshri/simple-yet-powerful-srt-subtitle-parser-cpp
