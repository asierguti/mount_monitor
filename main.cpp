#include <iostream>
#include <cstdlib>
#include "mountmond.h"

int main(int argc, char **argv) {
  try {
    mountmon::ModuleConnector connector(mountmon::Config(argc, argv));
    connector.Run();
  } catch (const mountmon::ConfigException &e) {
    std::cerr << "Config error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const mountmon::ModuleConnectorException &e) {
    std::cerr << "Connector error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown error" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
