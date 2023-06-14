# Hatsune Miku Project Diva Mega Mix+ (M39's+) FreeType Lyrics Renderer Mod
<div display="grid">
<img style="width:45%" src="https://user-images.githubusercontent.com/31397301/189489519-19a77a4f-d947-4201-96fa-ae1f086ff787.png" />
<img style="width:45%" src="https://user-images.githubusercontent.com/31397301/189489555-3f567519-b33d-4244-806f-524eb562a1c7.png" />
<img style="width:45%" src="https://user-images.githubusercontent.com/31397301/189489391-a887004b-00d9-4fd6-923a-b59113445b83.png" />
<img style="width:45%" src="https://user-images.githubusercontent.com/31397301/189489760-18595187-99ea-43e9-ad8a-77ecd0cf595d.png" />
</div>
<hr>

    本模块将利用 FreeType + ImGui 接管游戏对 PV 歌词的内部渲染，以达成更优的视觉体验
    兼容版本 ： (All of them!...maybe)

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
- 兼容超长(76字)以上歌词
- 加载外挂 `*.srt` 歌词

## MOD 安装
- 配置 [DivaModLoader](https://github.com/blueskythlikesclouds/DivaModLoader)
- 在 [Release](https://github.com/mos9527/MMFreeLyrics/releases) 下载本 MOD
- 解压至游戏文件夹 `mods\MMFreeLyrics` 目录
- 字体安装
    - 在模组目录中创建文件夹 `fonts`，将自己想用的字体文件(`.ttf`,`.otf`) 放入其中
        - 注：文件名最好使用英文。中文名称能够被使用，但字体名不会被正常显示。
- 外挂字幕
    - 在 MOD 目录新建 `lyrics` 文件夹
    - 将准备好的字幕文件放入 lyrics
    - 更名为 [PVID].srt
        - PVID 可在游戏内歌词窗口得知
    - 确定歌词窗口 `Attempt to load (and use) external SubRip Lyrics (applies on song start)` 打勾
    - 字幕会在每次 PV / 节奏游戏 开始前加载并自动构建字库
- 正确配置的模组目录应有如下结构
    
        HATSUNE MIKU PROJECT DIVA MEGA MIX PLUS\MODS\MMFREELYRICS
        |   ...
        │   config.toml*
        │   MMFreeLyrics.dll*
        └───lyrics                
        |      PV231-English.srt
        |       ...
        └───fonts*
                font_1.ttf
                font_2.otf
                ...
       

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

https://github.com/BroGamer4256/discord

https://github.com/BroGamer4256/camera
