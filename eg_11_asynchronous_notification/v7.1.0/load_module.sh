#!/bin/sh
module="async_notify"
device="async_notify"
mode="666"
group=0

function load() {
    insmod ./$module.ko $* || exit 1
}

function unload() {
    rmmod $module || exit 1
}

arg=${1:-"load"}
case $arg in
    load)
        load ;;
    unload)
        unload ;;
    reload)
        ( unload )
        load
        ;;
    *)
        echo "Usage: $0 {load | unload | reload}"
        echo "Default is load"
        exit 1
        ;;
esac
