## MMFreeLyrics 
### An alternative lyric renderer for Hatsune Miku: Project Diva Mega Mix+
----
## Description

A Freetype based lyric renderer for MM+, which uses ImGui for styling
and configuration.

Since ImGui has no built-in text stroking support,the one used here is
a modded version that uses FT_Stroker to implement it.

First time modding a `C++` game...I have to say,It wasn't really pleasant ;(

## Screenshots
![](https://user-images.githubusercontent.com/31397301/180940279-31d58deb-e384-42cd-848b-48c7c3c74abf.png)
![](https://user-images.githubusercontent.com/31397301/180940518-bab58460-5249-4d2e-b09a-d344d1249ddd.png)
![](https://user-images.githubusercontent.com/31397301/180940285-d56ac403-31b0-45c1-a290-384ea331e7cf.png)

## Building
- Setup Freetype
    Freetype is managed via `vcpkg`

    You may use vcpkg with `vcpkg install freetype --triplet=x64-windows` + `vcpkg integrate install` to install it

## TODO
----
- Automatically build charset on startup
- Config file support

## References
----
https://github.com/ocornut/imgui

https://github.com/ajkhoury/SigMaker-x64

https://github.com/BroGamer4256/score-mm

https://github.com/ActualMandM/DivaDllMods

https://github.com/blueskythlikesclouds/DivaModLoader