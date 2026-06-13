#! /bin/sh
module="scull"

function load() {
    insmod ./$module.ko $* || exit 1
}

function unload() {
    rmmod $module || exit 1
}

function reload() {
    rmmod $module || exit 1
    insmod ./$module.ko $* || exit 1
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

