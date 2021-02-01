// SPDX-License-Identifier: zlib-acknowledgement
#include <cstdint>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// libdrm wraps DRM ioctls?
#include <xf86drm.h>
#include <xf86drmMode.h>

inline void DEBUGGER_BREAK(void) { return; }
void break_and_log_errno(void)
{
  char errno_msg[256] = {0};
  // TODO(Ryan): With better gnu++20 support, use std::format
  sprintf(errno_msg, "%m");
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
    break_and_log_errno();
  }

 /* 
  u64 has_dumb = 0;
  // this means the driver for our card supports simple memory-mapped buffers
  // without the need for us to write driver dependent code
  if (drmGetCap(drm_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
    close(drm_fd);
    return -EOPNOTSUPP;
  }

  // want to get available connectors that is 1. turned on, i.e. monitor
  // 2. has a CRTC that can control it
  // ultimately we want to tell the DRM device to show our framebuffer on the given CRTC with
  // selected connector
  drmModeRes *resource_info = drmModeGetResources(drm_fd);
  if (resource_info == NULL) {
    return -errno;
  }

  drmModeConnector *conn = NULL;
  for (u32 i = 0; i < resource_info->count_connectors; ++i) {
     conn = drmModeGetConnector(drm_fd, resource_info->connectors[i]);

    if (conn == NULL) {
      // cannot retrieve DRM connector
    }

    if (con->connection == DRM_MODE_CONNECTED) {
      break;
    }
    drmModeFreeConnector(conn);
  }

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

  return 0;
}


