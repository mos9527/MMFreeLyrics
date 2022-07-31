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

## MOD 演示
[初音未来 Project DIVA MEGA 39's PC FreeType 歌词渲染 MOD - Bilibili](...)

## MOD 安装
- 安装 [DivaModManager](https://github.com/TekkaGB/DivaModManager)
- 在 [Release](https://github.com/mos9527/MMFreeLyrics/releases) 下载本 MOD
- 解压至游戏文件夹 `mods` 目录
- 在本模组目录中创建文件夹 `fonts`，将自己想用的字体文件(`.ttf`,`.otf`) 放入其中
    - 注：文件名最好使用英文。中文名称能够被使用，但字体名不会被正常显示。
- 如果还使用了歌词翻译 MOD
    - 将翻译模组文件夹内的 `pv_db.txt` 复制于本模组目录
    - 更名为 `charset.txt`
    - 否则中文子集等可能无法正常显示
- 利用 `DivaModManager` 启动游戏
----

## References
https://github.com/ocornut/imgui

https://github.com/ajkhoury/SigMaker-x64

https://github.com/BroGamer4256/score-mm

https://github.com/ActualMandM/DivaDllMods

https://github.com/blueskythlikesclouds/DivaModLoader

https://github.com/nastys/MEGAHAKUS