# GEGL Spheres - render a sphere or egg shape using GEGL
![image](https://github.com/LinuxBeaver/GEGL-Spheres---Render-Spheres-in-Gimp/assets/78667207/c9440ebe-fb3c-48e0-b4ff-14b26e120434)

## WARNING ABOUT BREAKING GEGL EFFECTS WITH THE NEWER INNER GLOW BINARY
GEGL Sphere depends on my GEGL Inner Glow plugin (lb:innerglow). Which can be
downloaded with GEGL Effects linked or in this repo with innerglow.dll/.so
Releases. GEGL Effects download below.

https://github.com/LinuxBeaver/GEGL-Effects---Layer-Effects-in-Gimp-using-GEGL/releases

Both Inner Glow’s are the same thing and you only need one. But recently
lb:innerglow and lb:bevel; the depedencies of my GEGL Layer Effect’s engine
had their namespace changed, as they were previously called gegl:bevel and gegl:innerglow.
Now they dropped gegl: for lb:. If you have an old version of GEGL Effects (before
late may 2023) it will not work with this newer lb:innerglow, thus breaking it. It is recommended
you update GEGL Effects to the latest version and get the inner glow binary that way. I’m
saying this because I don’t want to have to worry about people complaining that
GEGL Effects broke after they downloaded sphere.

If you use this inner glow binary and have my GEGL Effects plugin there is a
chance GEGL Effects will break until you update it to the latest version.
If you don’t understand this just install the plugin and later test GEGL Effects’s
Inner Glow ability. If GEGL Effects Inner Glow does nothing that means it broke.
You can easily fix it just by going to GEGL Effects Github and updating to the latest
version of GEGL Effects.


## OS specific location to put GEGL Filter binaries 

Windows
C:\Users\USERNAME\AppData\Local\gegl-0.4\plug-ins
 
 Linux 
 /home/(USERNAME)/.local/share/gegl-0.4/plug-ins
 
 Linux (Flatpak)
 /home/(USERNAME)/.var/app/org.gimp.GIMP/data/gegl-0.4/plug-ins


## Compiling and Installing
Linux

To compile and install you will need the GEGL header files (libgegl-dev on Debian based distributions or gegl on Arch Linux) and meson (meson on most distributions).

meson setup --buildtype=release build
ninja -C build


If you have an older version of gegl you may need to copy to ~/.local/share/gegl-0.3/plug-ins instead (on Ubuntu 18.04 for example).
Windows

The easiest way to compile this project on Windows is by using msys2. Download and install it from here: https://www.msys2.org/

Open a msys2 terminal with C:\msys64\mingw64.exe. Run the following to install required build dependencies:

pacman --noconfirm -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-meson mingw-w64-x86_64-gegl

Then build the same way you would on Linux:

meson setup --buildtype=release build
ninja -C build

## More previews of this based plugin

![image](https://github.com/LinuxBeaver/GEGL-Spheres---Render-Spheres-in-Gimp/assets/78667207/c41a40cd-9d23-4fd1-944d-38ad8763b153)
![image](https://github.com/LinuxBeaver/GEGL-Spheres---Render-Spheres-in-Gimp/assets/78667207/103a3801-a7bf-4249-b259-b2ba17698a1b)
![image](https://github.com/LinuxBeaver/GEGL-Spheres---Render-Spheres-in-Gimp/assets/78667207/39974278-b025-4833-9eb3-5e4fbc5de200)

