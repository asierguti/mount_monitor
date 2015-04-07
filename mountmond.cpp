/**
 * Asier Gutierrez <asierguti@gmail.com>
 */

#include "mountmond.h"

namespace {
sig_atomic_t stop_signal_received = -1;
void HandleSignal(int signum) {
  const char msg[] = "Stop signal received\n";
  stop_signal_received = signum;
  write(2, static_cast<const void *>(msg), sizeof(msg));
}
} // anonymous namespace

namespace mountmon {

ModuleConnector::ModuleConnector(const Config &config)
    : config_(config), module_major_(-1), nlh_(nullptr) {
  Init();
  OpenConnection();
  EstablishConnection();
}

ModuleConnector::~ModuleConnector() {
  close(sock_fd_);
}

void ModuleConnector::Init() {
  std::string filename = config_.FileName();
  logger_ = std::shared_ptr<mount_logger>(new mount_logger(filename));
}

void ModuleConnector::OpenConnection() {

  sock_fd_ = socket(PF_NETLINK, SOCK_RAW, 17);
  if (sock_fd_ < 0) {
    throw ModuleConnectorException("Can't open a socket to the kernel");
  }
}

void ModuleConnector::EstablishConnection() {
  static struct msghdr msg;

  memset(&dest_addr_, 0, sizeof(dest_addr_));
  memset(&dest_addr_, 0, sizeof(dest_addr_));
  dest_addr_.nl_family = AF_NETLINK;
  dest_addr_.nl_pid = 0;

  //NETLINK recvmsg will take ownership of this buffer, freeing it when it's not needed anymore
  nlh_ = static_cast<struct nlmsghdr *> (malloc(NLMSG_SPACE(MAX_PAYLOAD)));
  memset(nlh_, 0, NLMSG_SPACE(MAX_PAYLOAD));
  nlh_->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
  nlh_->nlmsg_pid = getpid();
  nlh_->nlmsg_flags = 0;

  strcpy(static_cast<char *>(NLMSG_DATA(nlh_)), "Hello");

  iov_.iov_base = (void *)nlh_;
  iov_.iov_len = nlh_->nlmsg_len;
  msg.msg_name = (void *)&dest_addr_;
  msg.msg_namelen = sizeof(dest_addr_);
  msg.msg_iov = &iov_;
  msg.msg_iovlen = 1;

  std::string message = "Sending message to kernel\n";
  message.append(static_cast<char *>(NLMSG_DATA(nlh_)));

  logger_->write(message);
  sendmsg(sock_fd_, &msg, 0);

  logger_->write("Waiting for message from kernel");
}

void ModuleConnector::Run() {

  struct nlmsghdr *nlh = NULL;
  struct iovec iov;
  static struct msghdr msg_;

  //NETLINK will take ownership of this buffer, freeing it when it's not needed anymore
  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
  nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = 0;

  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  msg_.msg_iov = &iov;
  msg_.msg_iovlen = 1;

  while (1) {
    recvmsg(sock_fd_, &msg_, 0);
    logger_->write(static_cast<char *>(NLMSG_DATA(nlh)));
  }
}

} // namespace mountmon
