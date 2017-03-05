#include <ev.h>
#include <libudev.h>
#include <stdio.h>

// To compile:
// clang -std=c99 -lev -ludev battery_with_udev.c

// To see fields available:
// udevadm monitor --subsystem-match=power_supply --udev --property
// or /sys/class/power_supply/{AC,BAT0}/uevent

// See
// - http://ftp.kernel.org/pub/linux/utils/kernel/hotplug/libudev/libudev-udev-monitor.html
// - https://github.com/robertalks/udev-examples/blob/master/udev_example3.c
// for more info.

ev_io battery_watcher;
struct udev *udev;
struct udev_monitor *monitor;

static void
battery_cb(EV_P_ ev_io *w, int revents)
{
  (void) w;
  (void) revents;
  (void) loop;
  puts("Stuff happened!");

  // Receive data from the udev monitor socket, allocate a new udev device, fill in received data, and return the device.
  struct udev_device *device = udev_monitor_receive_device(monitor);
  printf("I: ACTION=%s\n", udev_device_get_action(device));
  printf("I: DEVNAME=%s\n", udev_device_get_sysname(device));
  printf("I: DEVPATH=%s\n", udev_device_get_devpath(device));

  struct udev_list_entry *list_entry = udev_device_get_properties_list_entry(device);
  while (list_entry) {
    printf("Entry=%s value=%s\n",
        udev_list_entry_get_name(list_entry),
        udev_list_entry_get_value(list_entry));
    list_entry = udev_list_entry_get_next(list_entry);
  }


  /* printf("I: STATUS=%s\n", udev_device_get_property_value(device, "STATUS")); */
  /* printf("I: =%s\n", udev_device_get_property_value(device, "POWER_SUPPLY_STATUS")); */

  udev_device_unref(device);
}

int main(int argc, char *argv[])
{
  (void) argc; (void) argv;


  udev = udev_new();
  // You can use "kernel" instead of "udev", but this is strongly discouraged.
  monitor = udev_monitor_new_from_netlink(udev, "udev");
  // Add filter, in this case we want the power supply
  udev_monitor_filter_add_match_subsystem_devtype(monitor, "power_supply", NULL);

  // Binds udev_monitor socket to event source.
  udev_monitor_enable_receiving(monitor);
  // Grab file descriptor.
  int fd = udev_monitor_get_fd(monitor);

  struct ev_loop *loop = EV_DEFAULT;

  ev_io_init(&battery_watcher, battery_cb, fd, EV_READ);
  ev_io_start(loop, &battery_watcher);

  ev_run(loop, 0);

  udev_unref(udev);

  return 0;
}
