#include "mountmon_config.h"
#include <unistd.h>
#include <getopt.h>

namespace mountmon {
Config::Config(int argc, char **argv)
    : usage_(std::string("Usage: ") + argv[0]), filename_("out.log"),
      daemonize_(true) {
  usage_.append(": [-n] [-f where to write the log]");
  usage_.append("\n-n\tdo not perform daemonization");
  usage_.append("\n-f\nfile where to write the log");
  ParseCmd(argc, argv);
}

void Config::ParseCmd(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "nf:")) != -1) {
    switch (opt) {
    case 'n':
      daemonize_ = false;
      break;

    case 'f':
      filename_ = optarg;
      break;

    default: // '?'
      throw ConfigException(usage_);
    }
  }
}

const std::string Config::FileName() const noexcept {
  return filename_;
}

bool Config::Daemonize() const noexcept { return daemonize_; }

} // namespace mountmon
