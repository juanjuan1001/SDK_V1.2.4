#include "ui_resfile.h"
#include "fs/sdfile.h"

FILE *res_file = NULL;

int open_resfile(const char *name)
{
    int len;
    RES_HEAD_T head ALIGNED(4);

    printf("open_resfile: %s\n", name);
    if (res_file) {
        printf("resfile already opened!\n");
        fclose(res_file);
        res_file = NULL;
    }


    res_file = sdfile_open(name, "r");
    if (!res_file) {
        printf("open_resfile fail!\n");
        return -EINVAL;
    }

    len = sdfile_read(res_file, &head, sizeof(head));
    if (len != sizeof(head)) {
        sdfile_close(res_file);
        res_file = NULL;
        return -EFAULT;
    }

    if (head.magic[0] != 'R' || head.magic[1] != 'U' ||
        head.magic[2] != '2' || ((head.magic[3] != '1') && (head.magic[3] != '2'))) {
        puts("-------------resfile_err\n");
        return -EINVAL;
    }

    return 0;
}



