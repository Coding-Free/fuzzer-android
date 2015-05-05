#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include "utils.h"
#include "ioctls.h"

static const struct ioctl videodev2_ioctls[] = {
  IOCTL(VIDIOC_QUERYCAP, 0, 0),
  IOCTL(VIDIOC_RESERVED, 0, 0),
  IOCTL(VIDIOC_ENUM_FMT, 0, 0),
  IOCTL(VIDIOC_G_FMT, 0, 0),
  IOCTL(VIDIOC_S_FMT, 0, 0),
  IOCTL(VIDIOC_REQBUFS, 0, 0),
  IOCTL(VIDIOC_QUERYBUF, 0, 0),
  IOCTL(VIDIOC_G_FBUF, 0, 0),
  IOCTL(VIDIOC_S_FBUF, 0, 0),
  IOCTL(VIDIOC_OVERLAY, 0, 0),
  IOCTL(VIDIOC_QBUF, 0, 0),
#ifdef VIDIOC_EXPBUF
  IOCTL(VIDIOC_EXPBUF, 0, 0),
#endif
  IOCTL(VIDIOC_DQBUF, 0, 0),
  IOCTL(VIDIOC_STREAMON, 0, 0),
  IOCTL(VIDIOC_STREAMOFF, 0, 0),
  IOCTL(VIDIOC_G_PARM, 0, 0),
  IOCTL(VIDIOC_S_PARM, 0, 0),
  IOCTL(VIDIOC_G_STD, 0, 0),
  IOCTL(VIDIOC_S_STD, 0, 0),
  IOCTL(VIDIOC_ENUMSTD, 0, 0),
  IOCTL(VIDIOC_ENUMINPUT, 0, 0),
  IOCTL(VIDIOC_G_CTRL, 0, 0),
  IOCTL(VIDIOC_S_CTRL, 0, 0),
  IOCTL(VIDIOC_G_TUNER, 0, 0),
  IOCTL(VIDIOC_S_TUNER, 0, 0),
  IOCTL(VIDIOC_G_AUDIO, 0, 0),
  IOCTL(VIDIOC_S_AUDIO, 0, 0),
  IOCTL(VIDIOC_QUERYCTRL, 0, 0),
  IOCTL(VIDIOC_QUERYMENU, 0, 0),
  IOCTL(VIDIOC_G_INPUT, 0, 0),
  IOCTL(VIDIOC_S_INPUT, 0, 0),
  IOCTL(VIDIOC_G_OUTPUT, 0, 0),
  IOCTL(VIDIOC_S_OUTPUT, 0, 0),
  IOCTL(VIDIOC_ENUMOUTPUT, 0, 0),
  IOCTL(VIDIOC_G_AUDOUT, 0, 0),
  IOCTL(VIDIOC_S_AUDOUT, 0, 0),
  IOCTL(VIDIOC_G_MODULATOR, 0, 0),
  IOCTL(VIDIOC_S_MODULATOR, 0, 0),
  IOCTL(VIDIOC_G_FREQUENCY, 0, 0),
  IOCTL(VIDIOC_S_FREQUENCY, 0, 0),
  IOCTL(VIDIOC_CROPCAP, 0, 0),
  IOCTL(VIDIOC_G_CROP, 0, 0),
  IOCTL(VIDIOC_S_CROP, 0, 0),
  IOCTL(VIDIOC_G_JPEGCOMP, 0, 0),
  IOCTL(VIDIOC_S_JPEGCOMP, 0, 0),
  IOCTL(VIDIOC_QUERYSTD, 0, 0),
  IOCTL(VIDIOC_TRY_FMT, 0, 0),
  IOCTL(VIDIOC_ENUMAUDIO, 0, 0),
  IOCTL(VIDIOC_ENUMAUDOUT, 0, 0),
  IOCTL(VIDIOC_G_PRIORITY, 0, 0),
  IOCTL(VIDIOC_S_PRIORITY, 0, 0),
  IOCTL(VIDIOC_G_SLICED_VBI_CAP, 0, 0),
  IOCTL(VIDIOC_LOG_STATUS, 0, 0),
  IOCTL(VIDIOC_G_EXT_CTRLS, 0, 0),
  IOCTL(VIDIOC_S_EXT_CTRLS, 0, 0),
  IOCTL(VIDIOC_TRY_EXT_CTRLS, 0, 0),
#ifdef LOLLIPOP
  IOCTL(VIDIOC_ENUM_FRAMESIZES, 0, 0),
  IOCTL(VIDIOC_ENUM_FRAMEINTERVALS, 0, 0),
  IOCTL(VIDIOC_G_ENC_INDEX, 0, 0),
  IOCTL(VIDIOC_ENCODER_CMD, 0, 0),
  IOCTL(VIDIOC_TRY_ENCODER_CMD, 0, 0),
  IOCTL(VIDIOC_DBG_S_REGISTER, 0, 0),
  IOCTL(VIDIOC_DBG_G_REGISTER, 0, 0),
#endif
#ifdef VIDIOC_DBG_G_CHIP_IDENT
  IOCTL(VIDIOC_DBG_G_CHIP_IDENT, 0, 0),
#endif
#ifdef VIDIOC_S_HW_FREQ_SEEK
  IOCTL(VIDIOC_S_HW_FREQ_SEEK, 0, 0),
#endif
#ifdef VIDIOC_ENUM_DV_PRESETS
  IOCTL(VIDIOC_ENUM_DV_PRESETS, 0, 0),
#endif
#ifdef VIDIOC_S_DV_PRESET
  IOCTL(VIDIOC_S_DV_PRESET, 0, 0),
#endif
#ifdef VIDIOC_G_DV_PRESET
  IOCTL(VIDIOC_G_DV_PRESET, 0, 0),
#endif
#ifdef VIDIOC_QUERY_DV_PRESET
  IOCTL(VIDIOC_QUERY_DV_PRESET, 0, 0),
#endif
#ifdef VIDIOC_S_DV_TIMINGS
  IOCTL(VIDIOC_S_DV_TIMINGS, 0, 0),
#endif
#ifdef VIDIOC_G_DV_TIMINGS
  IOCTL(VIDIOC_G_DV_TIMINGS, 0, 0),
#endif
#ifdef VIDIOC_DQEVENT
  IOCTL(VIDIOC_DQEVENT, 0, 0),
#endif
#ifdef VIDIOC_SUBSCRIBE_EVENT
  IOCTL(VIDIOC_SUBSCRIBE_EVENT, 0, 0),
#endif
#ifdef VIDIOC_UNSUBSCRIBE_EVENT
  IOCTL(VIDIOC_UNSUBSCRIBE_EVENT, 0, 0),
#endif
#ifdef VIDIOC_CREATE_BUFS
  IOCTL(VIDIOC_CREATE_BUFS, 0, 0),
#endif
#ifdef VIDIOC_PREPARE_BUF
  IOCTL(VIDIOC_PREPARE_BUF, 0, 0),
#endif
#ifdef VIDIOC_G_SELECTION
  IOCTL(VIDIOC_G_SELECTION, 0, 0),
#endif
#ifdef VIDIOC_S_SELECTION
  IOCTL(VIDIOC_S_SELECTION, 0, 0),
#endif
#ifdef VIDIOC_DECODER_CMD
  IOCTL(VIDIOC_DECODER_CMD, 0, 0),
#endif
#ifdef VIDIOC_TRY_DECODER_CMD
  IOCTL(VIDIOC_TRY_DECODER_CMD, 0, 0),
#endif
#ifdef VIDIOC_ENUM_DV_TIMINGS
  IOCTL(VIDIOC_ENUM_DV_TIMINGS, 0, 0),
#endif
#ifdef VIDIOC_QUERY_DV_TIMINGS
  IOCTL(VIDIOC_QUERY_DV_TIMINGS, 0, 0),
#endif
#ifdef VIDIOC_DV_TIMINGS_CAP
  IOCTL(VIDIOC_DV_TIMINGS_CAP, 0, 0),
#endif
#ifdef VIDIOC_ENUM_FREQ_BANDS
  IOCTL(VIDIOC_ENUM_FREQ_BANDS, 0, 0),
#endif
};

static const char *const videodev2_devs[] = {
	"video4linux",
};

static const struct ioctl_group videodev2_grp = {
	.devtype = DEV_MISC,
	.devs = videodev2_devs,
	.devs_cnt = ARRAY_SIZE(videodev2_devs),
	.sanitise = pick_random_ioctl,
	.ioctls = videodev2_ioctls,
	.ioctls_cnt = ARRAY_SIZE(videodev2_ioctls),
};

REG_IOCTL_GROUP(videodev2_grp)