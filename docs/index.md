# yadsi (pronounced *"yahtzee"*)
yet another duckyscript interpreter. only supports a small subset of duckyscript for now.

you can use [PlatformIO](https://platformio.org) to compile and install this on your Pico.

# how to use this
this tutorial assumes you've worked with duckyscript already. if not, go look up the documentation!

upon plugging the pico into a pc, you'll see a new usb drive pop up. there's a `yadsi.conf` file which contains the config and a `scripts/` directory which contains the scripts. you can upload the scripts into the `scripts/` directory (customizable in `yadsi.conf`) and then specify the filename of the one you want to run in `yadsi.conf`. that will allow you to run the script by pressing BOOTSEL on the pico.

# config
```conf
# Filename of the script to execute, including ".txt" and excluding "scripts/".
# Example: name=helloworld_win.txt
name=

# Whether to run the script until unplugged.
# Example: loop=true
#loop=false

# Whether to remount the USB drive after execution.
# Example: remount=true
#remount=false

# The directory name for scripts, excluding trailing and leading slashes.
# Example: scripts_dir=collection/simple
#scripts_dir=scripts
```

# supported commands

|command|arguments|description|
|-|-|-|
|`STRING`|`<string to type>`|types a string|
|`STRINGLN`|`<string to type>`|types a string with a newline at the end|
|`REM`|`<anything>`|a comment|
|`DELAY`|`<milliseconds>`|waits `<milliseconds>` milliseconds|
|`WAIT_FOR_BUTTON_PRESS`||waits until BOOTSEL is pressed
|`LED_ON`||turns the onboard LED on|
|`LED_OFF`||turns the onboard LED off|
|`HOLD`|`<key>`|holds a key|
|`RELEASE`|`<key>`|releases a key|
|`<key>`|`[keys...]`|presses a key combination|