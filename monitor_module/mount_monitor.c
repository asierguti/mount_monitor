 /**
  * Asier Gutierrez <asierguti@gmail.com>
  */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <net/sock.h>       /* must include this before inet_common.h */
#include <net/inet_common.h>
#include <net/genetlink.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/binfmts.h>
#include <linux/cdev.h>
#include <linux/limits.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/spinlock.h>
#include "probe.h"
#include "mount_monitor.h"

#define KNETLINK_UNIT 17
#define NL_MOUNTGRP 2

static int mountmon_major = 0;
static int mountmon_minor = MOUNTMON_MINOR;

DECLARE_WAIT_QUEUE_HEAD(write_wq);
static char action_state = MOUNTMON_ERROR; 
static atomic_t open_count = ATOMIC_INIT(-1);

DECLARE_WAIT_QUEUE_HEAD(read_wq);
static rwlock_t cmd_lock;
static char current_cmd[PATH_MAX];
static size_t cmd_len = 0;

static atomic_t _pid = ATOMIC_INIT(-1);

static int _dummy(struct sk_buff *skb, struct genl_info *info);

static struct sock * knetlink_sk = NULL;

static struct security_operations *ops = NULL;

/* In modern kernels register_security() function is not exported
 * so we shall need to probe for current security_operations structure
 * and replace its bprm_check_security() pointer.
 * Of course we need to save the old pointer.
 */

static int(*old_sb_mount)(char *dev_name, struct nameidata * nd,char *type, unsigned long flags, void *data) = NULL;

static int _dummy(struct sk_buff *skb, struct genl_info *info)
{
  return 0;
}

static int mount_hook(char *dev_name, struct nameidata * nd,char *type, unsigned long flags, void *data)
{
  pr_debug("Writing cmd to buf\n");

  size_t len;

  int res;
  if (-1 == atomic_read(&open_count))
  {
    pr_debug("No one is connected to module, skipping checks");
    goto orig;
  }

  pr_debug("Writing cmd to buf\n");
  write_lock(&cmd_lock);
  len = strlen(dev_name);

  strncpy(current_cmd, dev_name, len);
  cmd_len = len;
  action_state = MOUNTMON_WAIT;


  write_unlock(&cmd_lock);
  pr_debug("cmd written. Waking up read\n");
  wake_up_interruptible(&read_wq);

  // pr_info("%s", current_cmd);

  struct sk_buff *skb_out;

  switch(action_state)
  {
  case MOUNTMON_WAIT:
    action_state = MOUNTMON_ERROR;
  case MOUNTMON_ERROR:
    break;
  case MOUNTMON_ALLOW:
    pr_debug("User allowed to mount: %s\n", dev_name);
    goto orig;
  case MOUNTMON_DENY:
    pr_debug("User denied to mount: %s\n", dev_name);
    return -EACCES;
  default:
    pr_err("Invalid response from user: %c\n", action_state);
    break;
  }

  struct nlmsghdr *nlh;

  skb_out = nlmsg_new(len,0);

  if(!skb_out)
  {
      pr_err(KERN_ERR "Failed to allocate new skb\n");
      goto orig;
  } 
  nlh=nlmsg_put(skb_out,0,0,NLMSG_DONE,cmd_len,0);  

  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
  strncpy(nlmsg_data(nlh),current_cmd, cmd_len);

  res=nlmsg_unicast(knetlink_sk,skb_out,atomic_read(&_pid));

  if(res<0)
  {
      pr_err(KERN_INFO "Error while sending bak to user\n");
      atomic_dec_and_test(&open_count);
  }

orig:
  return old_sb_mount(dev_name, nd, type, flags, data);
}

void mountmon_input(struct sk_buff *skb)
{
  struct nlmsghdr *nlh;
  int pid;
  struct sk_buff *skb_out;
  int msg_size;
  char *msg="Hello from kernel";
  int res;

  msg_size=strlen(msg);

  nlh=(struct nlmsghdr*)skb->data;
  atomic_set(&_pid, nlh->nlmsg_pid);
  atomic_inc(&open_count);
}

struct netlink_kernel_cfg cfg = {
    .input = mountmon_input,
};

static int __init mountmon_init(void)
{
  int ret = 0;

  rwlock_init(&cmd_lock);

	ops = probe_security_ops();
	if (!ops)
  {
    pr_err("Failed to find security_ops struct\n");
    ret = -1;
    goto cleanup;
  }
  pr_debug("Successfully probed security_ops struct\n");

  old_sb_mount = ops->sb_mount;

  smp_mb();

  ops->sb_mount = mount_hook;

  if ( knetlink_sk != NULL ) {
    pr_err("knetlink_init: sock already present\n");
    ret = -1;
    goto cleanup;
  }
  
  knetlink_sk = netlink_kernel_create(&init_net, 17, &cfg);
  if ( knetlink_sk == NULL ) {
    pr_err("knetlink_init: sock fail\n");
    ret = -1;
    goto cleanup;
  }

cleanup:
  return ret;
}

static void mountmon_exit(void)
{
  dev_t dev = MKDEV(mountmon_major, mountmon_minor);
  unregister_chrdev_region(dev, 1);

  if(ops)
  {
    ops->sb_mount = old_sb_mount;
  }

  sock_release( knetlink_sk->sk_socket );

  pr_debug("Exiting\n");
}

module_init(mountmon_init);
module_exit(mountmon_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Asier Gutierrez <asierguti@gmail.com>");