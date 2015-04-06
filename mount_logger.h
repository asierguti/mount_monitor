#ifndef MOUNT_LOGGER_H
#define MOUNT_LOGGER_H

#include <string>
#include <fstream>

class mount_logger
{
public:
	explicit mount_logger(const std::string& filename);
	~mount_logger();

	void write (const std::string& info);

private:
	std::ofstream logger_file_;
};


#endif // MOUNT_LOGGER_H
