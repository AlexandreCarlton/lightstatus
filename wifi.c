
#include <stdio.h>
#include <string.h>
#include <iwlib.h>
#include <stddef.h>

#include "wifi.h"
#include "config.h"


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

static int get_quality(unsigned int *signal_strength)
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
  *signal_strength = (unsigned int) (100 * stats.qual.qual / range.max_qual.qual);

  return 0;
}

void update_wifi_info(WifiInfo *info)
{

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

}

bool equal_wifi_info(WifiInfo *info1, WifiInfo *info2)
{
  return ((info1->signal_strength == info2->signal_strength)
      && (strcmp(info1->name, info2->name) == 0));
}

WifiInfo init_wifi_info(void)
{
  WifiInfo info = {
    .signal_strength = 0,
    .name = ""
  };
  return info;
}
