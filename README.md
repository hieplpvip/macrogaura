# macrogaura - RGB keyboard control for Asus ROG laptops on macOS

Supports RGB keyboards with IDs [0b05:1854](https://linux-hardware.org/index.php?id=usb:0b05-1854) (GL553, GL753), [0b05:1869](https://linux-hardware.org/index.php?id=usb:0b05-1869) (GL503, FX503, GL703) and [0b05:1866](https://linux-hardware.org/index.php?id=usb:0b05-1866) (GL504, GL703, GX501, GM501).

## Usage

```
Usage:
   macrogaura COMMAND ARGUMENTS

COMMAND should be one of:
   single_static
   single_breathing
   single_colorcycle
   multi_static
   multi_breathing
   red
   green
   blue
   yellow
   cyan
   magenta
   white
   black
   rainbow
```

## Building

Clone the GitHub repo and enter the top-level directory.  Then:

```
sudo xcodebuild
```

## Credits
- [Will Roberts](https://github.com/wroberts) for [rogauracore](https://github.com/wroberts/rogauracore) (based on which this program was developed)
- [Le Bao Hiep](https://github.com/hieplpvip) for porting rogauracore to macOS
