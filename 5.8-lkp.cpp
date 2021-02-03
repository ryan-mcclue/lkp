// SPDX-License-Identifier: zlib-acknowledgement
#include <cstdint>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
// exuberant ctags for utilising projects (gives definitions)
// 1. external libraries 
// TODO(Ryan): Only working for struct definitions and not functions?
// Documentation is sparse, should I build from source so I can inspect sources for documentation?
// How to read linux-docs
// 2. system calls
// 3. c standard library

// cscope for modifying (gives callers)

// libdrm wraps DRM ioctls?
#include <xf86drm.h>
#include <xf86drmMode.h>

inline void DEBUGGER_BREAK(void) { return; }
void break_and_log_errno(char const *msg)
{
  char errno_msg[256] = {0};
  // TODO(Ryan): With better gnu++20 support, use std::format
  sprintf(errno_msg, "%s %m", msg);
  // IMPORTANT(Ryan): Instruct debugger to break at this function call
  // and then simply step out to inspect errno_msg in expressions window
  DEBUGGER_BREAK();
}

typedef uint64_t u64;
typedef uint32_t u32;

int
main(int argc, char* argv[])
{ // NOTE(Ryan): For every device the linux kernel detects, it will export obtained information about the device
  // via sysfs (so, a file is present in /sys/dev/char or sys/dev/block depending on how the driver sends data)
  // It will also send a uevent to the udevd systemd service/daemon whose subsequent action can be configured.
  // In most cases will put in /dev.
  
  // TODO(Ryan): Investigate using libudev to avoid hard-coding.
  int drm_fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
  if (drm_fd == -1) {
    break_and_log_errno("Unable to open DRM device");
  }

  // TODO(Ryan): Even worth considering as every driver should support?
  u64 has_dumb = 0;
  if (drmGetCap(drm_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
    break_and_log_errno("DRM driver does not support dumb buffers");
  }

  drmModeRes *drm_info = drmModeGetResources(drm_fd);
  if (drm_info == NULL) {
    break_and_log_errno("Unable to get resources for DRM card");
  }

  drmModeConnector *drm_connector = NULL;
  for (u32 i = 0; i < drm_info->count_connectors; ++i) {
    drm_connector = drmModeGetConnector(drm_fd, drm_info->connectors[i]);
    if (drm_connector == NULL) {
      break_and_log_errno("Cannot retrieve DRM connector");
    }

    // TODO(Ryan): Support multiple monitors
    if (drm_connector->connection == DRM_MODE_CONNECTED) {
      if (drm_connector->count_modes == 0) {
        break_and_log_errno("No valid mode for DRM connector");
      }

      u32 mode_width = drm_connector->modes[0].hdisplay;
      u32 mode_height = drm_connector->modes[0].vdisplay;
  
      for (u32 i = 0; i < drm_connector->count_encoders; ++i) {
        drmModeEncoder *encoder = drmModeGetEncoder(drm_fd, drm_connector->encoder_id);
        if (encoder != NULL) {
          break_and_log_errno("Cannot retrieve encoder");
        }
        for (u32 j = 0; j < drm_info->count_crtcs; ++j) {
          // check if crtc works with encoder
          if (!(encoder->possible_crtcs & (1 << j))) {
            continue;
          }
          u32 crtc = drm_info->crtcs[j];
          break;
          drmFreeEncoder(encoder);
        }
      }

    }
    drmModeFreeConnector(drm_connector);

    // create frame buffer for crtc
    // TODO(Ryan): Use libgdm to create buffer for opengl/mesa context. 
    struct drm_mode_create_dumb creq = {0};
    creq.width = mode_width;
    creq.height = mode_height;
    creq.bpp = 32;
    int something = drmIoctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (something < 0) {
      break_and_log_errno("Cannot create dumb buffer");
    }
    u32 stride = creq.pitch;
    u32 size = creq.size;
    u32 handle = creq.handle;

    something = drmModeAddFB(drm_fd, mode_width, mode_height, 24, 32, stride, handle, &dev->fb?);
    if (something != 0) {
      break_and_log_errno("Cannot create dumb buffer");
    }

    struct drm_mode_map_mreq mreq = {0};
    mreq.handle = handle;
    something = drmIoctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (something != 0) {
      break_and_log_errno("Cannot map dumb buffer");
    }

    map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd, mreq.offset); 
    if (map == MAP_FAILED) {
      break_and_log_errno("Cannot mmap dumb buffer");
    }
    memset(map, 0, size);

    /*
    struct drm_mode_destroy_dumb dreq = {0};
    dreq.handle = handle;
    drmIoctl(drm_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    */
    
    // perform actual modesetting
    drmModeCrtc *ctrc_obj = drmModeGetCrtc(drm_fd, crtc);
    int ret = drmModeSetCrtc(drm_fd, crtc, dev->fb?, 0, 0, drm_connector, 1, mode?);
    if (ret != 0) {
      break_and_log_errno("Cannot set crtc for connector");
    }

    // drawing
    map[offset] = a << 24 | r << 16 | g << 8 | b;
    cleanup();
  }

 /* 
  // want to get available connectors that is 1. turned on, i.e. monitor
  // 2. has a CRTC that can control it
  // ultimately we want to tell the DRM device to show our framebuffer on the given CRTC with
  // selected connector


  for (u32 i = 0; i < conn->count_modes; ++i) {
    drmModeModeInfo *resolution = &conn->modes[i];
    if (resolution->type & DRM_MODE_TYPE_PREFERRED) {
      chosen_resolution = resolution;
      break;
    }
  }
  // connectors are pipelines
  // CRTCs (cathode ray tube controllers) are internal resources of gpu which dictate how many connectors we can control individually
  // can have more connectors than CRTCs


*/  

  close(drm_fd);

  return 0;
}


