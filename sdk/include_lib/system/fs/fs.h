#ifndef __FS_H__
#define __FS_H__

#include "generic/typedef.h"
#include "generic/list.h"
#include "system/malloc.h"

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#include "sdfile.h"

#define F_ATTR_RO       0x01
#define F_ATTR_ARC      0x02
#define F_ATTR_DIR      0x04
#define F_ATTR_VOL      0x08


#endif  /* __FS_H__ */


