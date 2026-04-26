#ifndef __RESAMPLE_API_H__
#define __RESAMPLE_API_H__

typedef struct _RS_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} RS_IO_CONTEXT;

typedef struct _RS_PARA_STRUCT_ {
    unsigned short insample;
    unsigned short outsample;
    unsigned char  quality;//3~8,数值越大质量越好
} RS_PARA_STRUCT;

typedef struct _SPEED_PITCH_PARA_STRUCT_ {
    unsigned short insample;
    unsigned short pitchrate;        //128<=>1
    unsigned char  quality;
    unsigned char speedin;
    unsigned char speedout;
    unsigned char pitchflag;
} SPEED_PITCH_PARA_STRUCT;

typedef struct  _RS_STUCT_API_ {
    unsigned int(*need_buf)();
    int (*open)(unsigned int *ptr, RS_PARA_STRUCT *rs_para, RS_IO_CONTEXT *rs_io);//succ:0, fail:1
    int (*run)(unsigned int *ptr, short *inbuf, int len);//ret:消耗的数据长度
} RS_STUCT_API;

RS_STUCT_API *get_resample_context();

typedef struct  _SPEEDPITCH_STUCT_API_ {
    unsigned int(*need_buf)(int srv);
    int(*open)(unsigned int *ptr, SPEED_PITCH_PARA_STRUCT *speedpitch_para, RS_IO_CONTEXT *rs_io);
    int(*run)(unsigned int *ptr, short *inbuf, int len);
} SPEEDPITCH_STUCT_API;

SPEEDPITCH_STUCT_API *get_sppitch_context();

#endif
