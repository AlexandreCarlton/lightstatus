
#include <stdio.h>
#include <string.h>
#include <iwlib.h>
#include <stddef.h>

#include "wifi.h"
#include "config.h"
#include "icons.h"

#define ICON_ARRAY_LENGTH(arr) ((sizeof(arr)) / (sizeof(char *)))


static int skfd;

static int get_essid(char *name, size_t length)
{
  struct iwreq request;
  request.u.essid.pointer = (caddr_t) name;
  request.u.essid.length = (unsigned short) (length + 1);
  request.u.essid.flags = 0 ;

  int ret_ext = iw_get_ext(skfd, wifi_interface, SIOCGIWESSID, &request);

  if (ret_ext < 0) {
    return ret_ext;
  } else if (request.u.essid.flags == 0) {
    // TODO Find better error code.
    return -1;
  }

  return 0;
}

static int get_quality(uint_fast8_t *signal_strength)
{

  iwrange range; //memset?
  iwstats stats;
  int ret_range;
  int ret_stats;
  if ((ret_range = iw_get_range_info(skfd, wifi_interface, &range)) < 0) {
    //fprintf(stderr, "Qual: range info failed.\n");
    *signal_strength = 0;
    return ret_range;
  } else if ((ret_stats = iw_get_stats(skfd, wifi_interface, &stats, &range, 1))) {
    //fprintf(stderr, "Qual: stats failed.\n");
    *signal_strength = 0;
    return ret_stats;
  } else if (stats.qual.updated & IW_QUAL_QUAL_INVALID) {
    *signal_strength = 0;
    //fprintf(stderr, "Qual: invalid quality.\n");
    return -1;
  }
  *signal_strength = (uint_fast8_t) (100 * stats.qual.qual / range.max_qual.qual);

  return 0;
}

WifiInfo wifi_init(void)
{
  WifiInfo info = {
    .signal_strength = 0,
    .name = ""
  };
  return info;
}

bool wifi_update(WifiInfo * const info)
{
  WifiInfo old_info;
  old_info.signal_strength = info->signal_strength;
  strncpy(old_info.name, info->name, MAX_ESSID_LENGTH);

  if ((skfd = iw_sockets_open()) < 0) {
    //fprintf(stderr, "Could not open iw socket.\n");
  }

  if (get_quality(&info->signal_strength) != 0) {
    info->signal_strength = 0;
  }

  if (get_essid(info->name, MAX_ESSID_LENGTH) != 0) {
    memset(info->name, 0, sizeof(info->name));
  }

  iw_sockets_close(skfd);

   return (info->signal_strength != old_info.signal_strength)
     || (strncmp(info->name, old_info.name, MAX_ESSID_LENGTH) != 0);
}

bool wifi_should_display(WifiInfo const * const info)
{
  return info->signal_strength > 0;
}

void wifi_print(FILE *file, WifiInfo const * const info)
{
  const char *wifi_connected_icon = wifi_connected_icons[0];
  unsigned long len_icons = sizeof(wifi_connected_icons) / sizeof(char *);
  for (unsigned long i = 1; i <= len_icons; i++)
  {
    if (info->signal_strength <= i * 100 / len_icons) {
      wifi_connected_icon = wifi_connected_icons[i - 1];
      break;
    }
  }
  if (info->signal_strength > 0) {
    fprintf(file, "%s %s", wifi_connected_icon, info->name);
  } else {
    fprintf(file, "%s %s", wifi_disconnected_icon, "N/A");
  }

}

void wifi_free(WifiInfo * const info)
{
  (void) info;
}
