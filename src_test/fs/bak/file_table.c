/*
 *  linux/fs/file_table.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include "common.h"
#include <linux/fs.h>

struct file file_table[NR_FILE];
