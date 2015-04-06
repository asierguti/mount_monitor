#include "mount_logger.h"

mount_logger::mount_logger (const std::string& filename){
	  logger_file_.open (filename);
}

mount_logger::~mount_logger ()
{
	logger_file_.close();
}

void mount_logger::write (const std::string& info)
{
	
	logger_file_ << info << std::endl;
}
