MODULE_SUBDIR = monitor_module
INCLUDE = -I$(MODULE_SUBDIR) -I.
CXXFLAGS = -g -std=gnu++11 -Wall -Wextra -Werror -pedantic $(INCLUDE)

all: mountmond mountmon_module

mountmon_module:
	cd $(MODULE_SUBDIR) && $(MAKE)

# mountmon_wrapper.o: mountmon_wrapper.c
# 	$(CC) -c $(CFLAGS) mountmon_wrapper.c -o $@	

mountmon_config.o: mountmon_config.cpp
	$(CXX) -c $(CXXFLAGS) mountmon_config.cpp -o $@

mountmond.o: mountmond.cpp
	$(CXX) -c $(CXXFLAGS) mountmond.cpp -o $@

mountmond: mount_logger.o mountmon_config.o mountmond.o main.cpp
	$(CXX) $(CXXFLAGS) mount_logger.o mountmon_config.o mountmond.o main.cpp -o $@

clean:
	rm -f *.o mountmond
	cd $(MODULE_SUBDIR) && $(MAKE) clean
