#pragma once

#define CONFIG_NAME "/yadsi.conf"
#define CONFIG_CONTENTS "# Filename of the script to execute, including \".txt\" and excluding \"scripts/\".\n\
# Example: name=helloworld_win.txt\n\
name=\n\
\n\
# Whether to run the script until unplugged.\n\
# Example: loop=true\n\
#loop=false\n\
\n\
# Whether to remount the USB drive after execution.\n\
# Example: remount=true\n\
#remount=false\n\
\n\
# The directory name for scripts, excluding trailing and leading slashes.\n\
# Example: scripts_dir=collection/simple\n\
#scripts_dir=scripts"

#define SCRIPTS_NAME "/scripts"