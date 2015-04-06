/**
 * Asier Gutierrez <asierguti@gmail.com>
 */

#ifndef _MOUNT_MONITOR_H_
#define _MOUNT_MONITOR_H_

/* Device minor number */
#define MOUNTMON_MINOR 0

#define MOUNTMON_WAIT 'w' /* we sent data to user, waiting for reply */
#define MOUNTMON_ERROR 'e' /* error state */
#define MOUNTMON_ALLOW 'a' /* allow exec (reply from user) */
#define MOUNTMON_DENY 'd' /* deny exec (reply from user) */

/* Timeout for waiting user response */
#define MOUNTMON_TIMEOUT 1000 /* msecs */

/* max GENL_NAMSIZ (16) bytes: */
#define MOUNTMON_GENL_FAM_NAME "mountmon"
/* max GENL_NAMSIZ (16) bytes: */
#define MOUNTMON_GENL_MC_GRP_NAME "mountmon-grp"
#define MOUNTMON_GENL_VERSION 1

enum {
    MOUNTMON_CMD_UNSPEC,
    MOUNTMON_CMD_SEND_NUM,
    MOUNTMON_CMD_DUMMY,
};

enum {
    MOUNTMON_ATTR_UNSPEC,
    MOUNTMON_ATTR_NUM,
    /* last entry: */
    MOUNTMON_ATTR_COUNT
};

#endif /* _MOUNT_MONITOR_H_ */
