#ifndef _MOUNTMON_CONFIG_H_
#define _MOUNTMON_CONFIG_H_
#include <string>
#include <stdexcept>

namespace mountmon {

class ConfigException : public std::runtime_error {
public:
  ConfigException(const std::string &str) : std::runtime_error(str) {}

  ConfigException(const char *str) : std::runtime_error(str) {}
};

class Config {
public:
  Config(int argc, char **argv);
  const std::string FileName() const noexcept;
  bool Daemonize() const noexcept;

private:
  void ParseCmd(int argc, char **argv);

private:
  std::string usage_;
  std::string filename_;
  bool daemonize_;
};
} // namespace mountmon
#endif // _MOUNTMON_CONFIG_H_
