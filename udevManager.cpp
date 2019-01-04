#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <libudev.h>

#include <fcntl.h>
#include <unistd.h>
// #include "sys/ioctl.h"
#include <linux/input.h>

#include "udevManager.h"

UdevManager::UdevManager()
{
    udev_fd = -1;
}

UdevManager::~UdevManager()
{
    if (udev)
    {
        udev_unref(udev);
    }
    if (mon)
    {
        udev_monitor_unref(mon);
    }
    for (device::iterator iter = keyboards.begin(); iter != keyboards.end(); iter++)
    {
        if (iter->second >= 0)
            close(iter->second);
    }
    for (device::iterator iter = mice.begin(); iter != mice.end(); iter++)
    {
        if (iter->second >= 0)
            close(iter->second);
    }
}

void UdevManager::setup()
{
    setupUdev();
    setupKeyboard();
    setupMouse();
    printDevice();
}

void UdevManager::readEvent()
{
    readUdevEvent();
    readKeyboardEvent();
    readMouseEvent();
}

void UdevManager::addKeyboard(const char *node)
{
    if (node == NULL)
        return;

    removeKeyboard(node);

    int fd = open(node, O_RDONLY | O_NONBLOCK);
    if (fd >= 0)
    {
        keyboards[node] = fd;
    }
}

void UdevManager::removeKeyboard(const char *node)
{
    if (node == NULL)
        return;

    device::iterator iter = keyboards.find(node);
    if (iter != keyboards.end())
    {
        if (iter->second >= 0)
        {
            close(iter->second);
        }
        keyboards.erase(node);
    }
}

void UdevManager::addMouse(const char *node)
{
    if (node == NULL)
        return;

    removeMouse(node);

    int fd = open(node, O_RDONLY | O_NONBLOCK);
    if (fd >= 0)
    {
        mice[node] = fd;
    }
}

void UdevManager::removeMouse(const char *node)
{
    if (node == NULL)
        return;
    device::iterator iter = mice.find(node);
    if (iter != mice.end())
    {
        if (iter->second >= 0)
        {
            close(iter->second);
        }
        mice.erase(node);
    }
}
void UdevManager::printDevice()
{
    printf("Keyboard: [%d], Mouse: [%d]\n", keyboards.size(), mice.size());
}

void UdevManager::setupUdev()
{
    udev = udev_new();
    if (!udev)
        return;

    mon = udev_monitor_new_from_netlink(udev, "udev");
    // just listen for input devices
    udev_monitor_filter_add_match_subsystem_devtype(mon, "input", NULL);
    udev_monitor_enable_receiving(mon);
    // get the file descriptor for the mon (used w/ select);
    udev_fd = udev_monitor_get_fd(mon);
    if (udev_fd < 0)
    {
        return;
    }
}

void UdevManager::setupKeyboard()
{
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *entry;
    struct udev_device *dev;

    /* Create a list of the devices in the 'input' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_add_match_property(enumerate, "ID_INPUT_KEYBOARD", "1");
    udev_enumerate_add_match_property(enumerate, "ID_INPUT_KEY", "1");
    // udev_enumerate_add_match_property(enumerate, "ID_INPUT_MOUSE", "1");

    udev_enumerate_scan_devices(enumerate);

    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(entry, devices)
    {
        /* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
        const char *name = udev_list_entry_get_name(entry);

        dev = udev_device_new_from_syspath(udev, name);

        const char *sysname = udev_device_get_sysname(dev);
        const char *devnode = udev_device_get_devnode(dev);
        const char *devpath = udev_device_get_devpath(dev);
        const char *devtype = udev_device_get_devtype(dev);
        dev_t devnum = udev_device_get_devnum(dev);
        const char *driver = udev_device_get_driver(dev);
        const char *action = udev_device_get_action(dev);

        printf("Keyboard device: %s\n", name);
        printf(" - sysname : %s\n", sysname);
        printf(" - devnode : %s\n", devnode);
        printf(" - devpath: %s\n", devpath);
        printf(" - devtype: %s\n", devtype);
        printf(" - devnum: %d\n", devnum);
        printf(" - driver: %s\n", driver);
        printf(" - action: %s\n", action);

        if (devnode)
            addKeyboard(devnode);

        udev_device_unref(dev);
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);
}
void UdevManager::setupMouse()
{
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *entry;
    struct udev_device *dev;

    /* Create a list of the devices in the 'input' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_add_match_property(enumerate, "ID_INPUT_MOUSE", "1");

    udev_enumerate_scan_devices(enumerate);

    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(entry, devices)
    {
        /* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
        const char *name = udev_list_entry_get_name(entry);

        dev = udev_device_new_from_syspath(udev, name);

        const char *sysname = udev_device_get_sysname(dev);
        const char *devnode = udev_device_get_devnode(dev);
        const char *devpath = udev_device_get_devpath(dev);
        const char *devtype = udev_device_get_devtype(dev);
        dev_t devnum = udev_device_get_devnum(dev);
        const char *driver = udev_device_get_driver(dev);
        const char *action = udev_device_get_action(dev);

        printf("Keyboard device: %s\n", name);
        printf(" - sysname : %s\n", sysname);
        printf(" - devnode : %s\n", devnode);
        printf(" - devpath: %s\n", devpath);
        printf(" - devtype: %s\n", devtype);
        printf(" - devnum: %d\n", devnum);
        printf(" - driver: %s\n", driver);
        printf(" - action: %s\n", action);

        if (devnode)
            addMouse(devnode);

        udev_device_unref(dev);
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);
}

void UdevManager::readUdevEvent()
{
    fd_set fds;
    struct timeval tv;
    int ret;
    struct udev_device *dev;

    FD_ZERO(&fds);
    FD_SET(udev_fd, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    ret = select(udev_fd + 1, &fds, NULL, NULL, &tv);

    /* Check if our file descriptor has received data. */
    if (ret > 0 && FD_ISSET(udev_fd, &fds))
    {
        /* Make the call to receive the device.
		   select() ensured that this will not block. */
        dev = udev_monitor_receive_device(mon);
        if (dev)
        {
            const char *devnode = udev_device_get_devnode(dev);
            const char *subsystem = udev_device_get_subsystem(dev);
            const char *type = udev_device_get_devtype(dev);
            const char *action = udev_device_get_action(dev);
            const char *prop_keyboard = udev_device_get_property_value(dev, "ID_INPUT_KEYBOARD");
            const char *prop_mouse = udev_device_get_property_value(dev, "ID_INPUT_MOUSE");

            printf("udev_monitor recv\n");
            printf(" - devnode: %s\n", devnode);
            printf(" - subsystem: %s\n", subsystem);
            printf(" - type: %s\n", type);
            printf(" - action: %s\n", action);
            printf(" - prop_keyboard: %s\n", prop_keyboard);
            printf(" - prop_mouse: %s\n", prop_mouse);

            if (devnode)
            {
                if (strcmp(action, "add") == 0)
                {
                    if (prop_keyboard)
                        addKeyboard(devnode);
                    if (prop_mouse)
                        addMouse(devnode);
                }
                else if (strcmp(action, "remove") == 0)
                {
                    if (prop_keyboard)
                        removeKeyboard(devnode);
                    if (prop_mouse)
                        removeMouse(devnode);
                }
                printDevice();
            }
            udev_device_unref(dev);
        }
    }
}

void UdevManager::readKeyboardEvent()
{
    struct input_event ev;

    for (device::iterator iter = keyboards.begin(); iter != keyboards.end(); iter++)
    {
        int nBytesRead = read(iter->second, &ev, sizeof(struct input_event));
        while (nBytesRead >= 0)
        {
            if (ev.type == EV_KEY)
            {
                printf("dev[%s] type[%d] code[%d] value[%d]\n", iter->first.c_str(), ev.type, ev.code, ev.value);
            }
            nBytesRead = read(iter->second, &ev, sizeof(struct input_event));
        }
    }
}

void UdevManager::readMouseEvent()
{
    struct input_event ev;

    for (device::iterator iter = mice.begin(); iter != mice.end(); iter++)
    {
        int nBytesRead = read(iter->second, &ev, sizeof(struct input_event));
        while (nBytesRead >= 0)
        {
            if (ev.type == EV_KEY || ev.type == EV_REL || ev.type == EV_ABS)
            {
                printf("dev[%s] type[%d] code[%d] value[%d]\n", iter->first.c_str(), ev.type, ev.code, ev.value);
            }

            nBytesRead = read(iter->second, &ev, sizeof(struct input_event));
        }
    }
}