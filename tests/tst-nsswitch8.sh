#!/bin/bash

# check option +--is_generated

nsswitch_config_exe="$PWD/../nsswitch-config"

if ! [[ -f "$nsswitch-config_exe" ]]; then
    nsswitch_config_exe="$PWD/nsswitch-config"
    if ! [[ -f "$nsswitch_config_exe" ]]; then
        echo "Couldn't find the nsswitch-config executable on $PWD"
        exit 1
    fi
fi
nsswitch_config_root="$PWD/../../tests/test8"
if ! [[ -d "$nsswitch_config_root" ]]; then
    nsswitch_config_root="$PWD/../tests/test8"
    if ! [[ -d "$nsswitch_config_root" ]]; then
        echo "Couldn't find $nsswitch_onfig_root"
        exit 1
    fi
fi

echo $nsswitch_config_exe --output=$nsswitch_config_root/etc/nsswitch.conf.admin --is_generated
$nsswitch_config_exe --output=$nsswitch_config_root/etc/nsswitch.conf.admin --is_generated
if [ $? -eq 0 ]; then
    echo "Should return 1 and not 0"
    exit 1
fi

echo $nsswitch_config_exe --output=$nsswitch_config_root/etc/nsswitch.conf.gen --is_generated
$nsswitch_config_exe --output=$nsswitch_config_root/etc/nsswitch.conf.admin.gen --is_generated
if [ $? -eq 1 ]; then
    echo "Should return 0 and not 1"
    exit 1
fi

exit 0

