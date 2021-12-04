
#pragma once 
#include "fs/vfs.h"


decl_open(devfs_open);
decl_close(devfs_close);
decl_read(devfs_read);
decl_write(devfs_write);
decl_readdir(devfs_readdir);
decl_finddir(devfs_finddir);

void devfs_init();
