
#include "typedef.h"

int Rle_Decode(u8 *inbuf, int inSize, u8 *outbuf, int onuBufSize, int offset, int len, int pixel_size)
{
    u8 *src = inbuf;
    u8 sign;
    int i;
    int decSize = 0;
    int count = 0;
    int pos = 0;
    int copylen = 0;

    while (src < (inbuf + inSize)) {
        sign = *src++;
        count = sign & 0x7F;
        if ((decSize + count * pixel_size) > onuBufSize) {
            return -1;
        }

        if ((sign & 0x80) == 0x80) {
            if (decSize < offset) {
                if ((decSize + count * pixel_size) >= offset) {
                    copylen = (decSize + count * pixel_size - offset) > (len - pos) ? (len - pos) : (decSize + count * pixel_size - offset);
                    if (pixel_size == 1) {
                        memset(&outbuf[pos], *src, copylen);
                    } else if (pixel_size == 2) {
                        if (src[0] == src[1]) {
                            memset(&outbuf[pos], *src, copylen);
                        } else {
                            int t;
                            for (t = 0; t < copylen / 2; t++) {
                                memcpy(&outbuf[pos + t * 2], src, 2);
                            }
                        }
                    } else {
                        ASSERT(0);
                    }
                    pos += copylen;
                    if (pos == len) {
                        break;
                    }
                }
                decSize += count * pixel_size;
            } else {
                copylen = count * pixel_size > (len - pos) ? (len - pos) : count * pixel_size;
                if (copylen) {
                    if (pixel_size == 1) {
                        memset(&outbuf[pos], *src, copylen);
                    } else if (pixel_size == 2) {
                        if (src[0] == src[1]) {
                            memset(&outbuf[pos], *src, copylen);
                        } else {
                            int t;
                            for (t = 0; t < copylen / 2; t++) {
                                memcpy(&outbuf[pos + t * 2], src, 2);
                            }
                        }
                    } else {
                        ASSERT(0);
                    }
                    pos += copylen;
                    if (pos == len) {
                        break;
                    }
                }
                decSize += count * pixel_size;
            }
            src += pixel_size;
        } else {
            if (decSize < offset) {
                if ((decSize + count * pixel_size) >= offset) {
                    copylen = (decSize + count * pixel_size - offset) > (len - pos) ? (len - pos) : (decSize + count * pixel_size - offset);
                    memcpy(&outbuf[pos], &src[offset - decSize], copylen);
                    pos += copylen;
                    if (pos == len) {
                        break;
                    }
                }
                decSize += count * pixel_size;
                src += count * pixel_size;
            } else {
                copylen = count * pixel_size > (len - pos) ? (len - pos) : count * pixel_size;
                if (copylen) {
                    memcpy(&outbuf[pos], src, copylen);
                    pos += copylen;
                    if (pos == len) {
                        break;
                    }
                }

                decSize += count * pixel_size;
                src += count * pixel_size;
            }
        }
    }

    /* ASSERT(pos == len); */
    if (pos != len) {
        return -1;
    }

    return decSize;
}

