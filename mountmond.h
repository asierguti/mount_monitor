/**
 * Asier Gutierrez <asierguti@gmail.com>
 */

#ifndef _MOUNTMOND_H_
#define _MOUNTMOND_H_

#include <stdexcept>
#include <linux/limits.h>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NETLINK_USER 17

#define MAX_PAYLOAD 1024 /* maximum payload size*/

#include "mountmon_config.h"
#include "mount_monitor.h"
#include "mount_logger.h"

namespace mountmon {

class ModuleConnectorException : public std::runtime_error {
public:
  ModuleConnectorException(const std::string &str) : std::runtime_error(str) {}

  ModuleConnectorException(const char *str) : std::runtime_error(str) {}
};

class ModuleConnector {
public:
  explicit ModuleConnector(const Config &config);
  ~ModuleConnector();

  void Run();

private:

  void Init();
  // Set signal handler(s)
  // void InitSignals() const noexcept;

  // Find module major number
  void FindMajor();

  // void PopulateDenyList();

  void OpenConnection();
  void EstablishConnection();

private:
  typedef std::unordered_set<std::string> AppsList;

private:
  Config config_;
  unsigned module_major_;
  char buf_[PATH_MAX];
  const std::string config_file_;
  AppsList deny_list_;

  struct sockaddr_nl src_addr_, dest_addr_;
  struct nlmsghdr *nlh_;
  struct iovec iov_;
  int sock_fd_;
  struct msghdr msg_;

  std::shared_ptr <mount_logger> logger_;
};

} // namespace mountmon
#endif // _MOUNTMOND_H_
