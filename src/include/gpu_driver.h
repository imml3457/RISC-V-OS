#ifndef __GPU_DRIVER_H__
#define __GPU_DRIVER_H__

#include <virtio.h>
#include <pci.h>


struct virtio_gpu_config {
   u32  events_read;
   u32  events_clear;
   u32  num_scanouts;
   u32  reserved;
};

#define VIRTIO_GPU_FLAG_FENCE  1
struct control_header {
   u32 cont_type;
   u32 flags;
   u64 fence_id;
   u32 context_id;
   u32 padding;
};
enum control_type {
   /* 2D Commands */
   VIRTIO_GPU_CMD_GET_DISPLAY_INFO = 0x0100,
   VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
   VIRTIO_GPU_CMD_RESOURCE_UNREF,
   VIRTIO_GPU_CMD_SET_SCANOUT,
   VIRTIO_GPU_CMD_RESOURCE_FLUSH,
   VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
   VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
   VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING,
   VIRTIO_GPU_CMD_GET_CAPSET_INFO,
   VIRTIO_GPU_CMD_GET_CAPSET,
   VIRTIO_GPU_CMD_GET_EDID,
   /* cursor commands */
   VIRTIO_GPU_CMD_UPDATE_CURSOR = 0x0300,
   VIRTIO_GPU_CMD_MOVE_CURSOR,
   /* success responses */
   VIRTIO_GPU_RESP_OK_NODATA = 0x1100,
   VIRTIO_GPU_RESP_OK_DISPLAY_INFO,
   VIRTIO_GPU_RESP_OK_CAPSET_INFO,
   VIRTIO_GPU_RESP_OK_CAPSET,
   VIRTIO_GPU_RESP_OK_EDID,
   /* error responses */
   VIRTIO_GPU_RESP_ERR_UNSPEC = 0x1200,
   VIRTIO_GPU_RESP_ERR_OUT_OF_MEMORY,
   VIRTIO_GPU_RESP_ERR_INVALID_SCANOUT_ID,
   VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID,
   VIRTIO_GPU_RESP_ERR_INVALID_CONTEXT_ID,
   VIRTIO_GPU_RESP_ERR_INVALID_PARAMETER,
};

#define VIRTIO_GPU_MAX_SCANOUTS 16
struct rectangle {
   u32 x;
   u32 y;
   u32 width;
   u32 height;
};
struct display_info_response {
   struct control_header hdr;  /* VIRTIO_GPU_RESP_OK_DISPLAY_INFO */
   struct gpu_display {
       struct rectangle rect;
       u32 enabled;
       u32 flags;
   } displays[VIRTIO_GPU_MAX_SCANOUTS];
};
enum gpu_formats {
   B8G8R8A8_UNORM = 1,
   B8G8R8X8_UNORM = 2,
   A8R8G8B8_UNORM = 3,
   X8R8G8B8_UNORM = 4,
   R8G8B8A8_UNORM = 67,
   X8B8G8R8_UNORM = 68,
   A8B8G8R8_UNORM = 121,
   R8G8B8X8_UNORM = 134,
};

struct resource_create2d_request {
   struct control_header hdr; /* VIRTIO_GPU_CMD_RESOURCE_CREATE_2D */
   u32 resource_id;   /* We get to give a unique ID to each resource */
   u32 format;        /* From GpuFormats above */
   u32 width;
   u32 height;
};

struct resource_unref_request {
    struct control_header hdr; /* VIRTIO_GPU_CMD_RESOURCE_UNREF */
    u32 resource_id;
    u32 padding;
};

struct set_scanout_request {
    struct control_header hdr; /* VIRTIO_GPU_CMD_SET_SCANOUT */
    struct rectangle rect;
    u32 scanout_id;
    u32 resource_id;
};

struct virtio_gpu_resource_attach_backing {
        struct control_header hdr;
        u32 resource_id;
        u32 nr_entries;
};

struct virtio_gpu_mem_entry{
    u64 addr;
    u32 length;
    u32 padding;
};

struct virtio_gpu_transfer_to_host_2d {
        struct control_header hdr;
        struct rectangle r;
        u64 offset;
        u32 resource_id;
        u32 padding;
};

struct virtio_gpu_resource_flush{
        struct control_header hdr;
        struct rectangle r;
        u32 resource_id;
        u32 padding;
};

struct pixel{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};


struct membacked_framebuffer{
    struct pixel* pix;
    u32 width;
    u32 height;
};

extern struct membacked_framebuffer* frame_buffer;


void virt_gpu_drive_init(struct PCIdriver*, void**, int);
int virt_gpu_drive(void*, u64, void*, u64, void*, u64, u8);
int start_gpu();
int redraw_framebuffer(const struct rectangle *r);

#define GPU_READ 0
#define GPU_WRITE 1

#endif
