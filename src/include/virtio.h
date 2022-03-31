#ifndef __VIRTIO_H__
#define __VIRTIO_H__

#include <common.h>

#define VIRTIO_CAP_COMMON_CFG 1
#define VIRTIO_CAP_NOTIFY_CFG 2
#define VIRTIO_CAP_ISR_CFG 3
#define VIRTIO_CAP_DEVICE_CFG 4
#define VIRTIO_CAP_PCI_CFG 5


struct virtio_pci_cap {
   u8 cap_vndr;   /* Generic PCI field: PCI_CAP_ID_VNDR */
   u8 cap_next;   /* Generic PCI field: next ptr. */
   u8 cap_len;    /* Generic PCI field: capability length */
   u8 cfg_type;   /* Identifies the structure. */
   u8 bar;        /* Which BAR to find it. */
   u8 padding[3]; /* Pad to full dword. */
   u32 offset;    /* Offset within bar. */
   u32 length;    /* Length of the structure, in bytes. */
};

struct virtio_pci_common_cfg {
   u32 device_feature_select; /* read-write */
   u32 device_feature; /* read-only for driver */
   u32 driver_feature_select; /* read-write */
   u32 driver_feature; /* read-write */
   u16 msix_config; /* read-write */
   u16 num_queues; /* read-only for driver */
   u8 device_status; /* read-write */
   u8 config_generation; /* read-only for driver */
   /* About a specific virtqueue. */
   u16 queue_select; /* read-write */
   u16 queue_size; /* read-write */
   u16 queue_msix_vector; /* read-write */
   u16 queue_enable; /* read-write */
   u16 queue_notify_off; /* read-only for driver */
   u64 queue_desc; /* read-write */
   u64 queue_driver; /* read-write */
   u64 queue_device; /* read-write */
};

struct virtio_pci_notify_cap {
   struct virtio_pci_cap cap;
   u32 notify_off_multiplier; /* Multiplier for queue_notify_off. */
};

struct virtio_pci_isr_cap {
    union {
       struct {
          unsigned queue_interrupt: 1;
          unsigned device_cfg_interrupt: 1;
          unsigned reserved: 30;
       };
       unsigned int isr_cap;
    };
};


/* le32 is used here for ids for padding reasons. */
struct virtq_used_elem {
   /* Index of start of used descriptor chain. */
   u32 id;
   /* Total length of the descriptor chain which was used (written to) */
   u32 len;
};

struct virtq_used {
   #define VIRTQ_USED_F_NO_NOTIFY  1
   u16 flags;
   u16 idx;
   struct virtq_used_elem ring[ /* Queue Size */];
//   u16 avail_event; Only if VIRTIO_F_EVENT_IDX
};

struct virtq_avail {
   #define VIRTQ_AVAIL_F_NO_INTERRUPT      1
   u16 flags;
   u16 idx;
   u16 ring[ /* Queue Size */ ];
/*    u16 used_event; Only if VIRTIO_F_EVENT_IDX  */
};

struct virtq_desc {
   /* Address (guest-physical). */
   u64 addr;
   /* Length. */
   u32 len;
   /* This marks a buffer as continuing via the next field. */
   #define VIRTQ_DESC_F_NEXT   1
   /* This marks a buffer as device write-only (otherwise device read-only). */
   #define VIRTQ_DESC_F_WRITE     2
   /* This means the buffer contains a list of buffer descriptors. */
   #define VIRTQ_DESC_F_INDIRECT   4
   /* The flags as indicated above. */
   u16 flags;
   /* Next field if flags & NEXT */
   u16 next;
};

struct virtio_blk_config {
   u64 capacity;
   u32 size_max;
   u32 seg_max;
   struct virtio_blk_geometry {
      u16 cylinders;
      u8 heads;
      u8 sectors;
   } geometry;
   u32 blk_size; // the size of a sector, usually 512
   struct virtio_blk_topology {
      // # of logical blocks per physical block (log2)
      u8 physical_block_exp;
      // offset of first aligned logical block
      u8 alignment_offset;
      // suggested minimum I/O size in blocks
      u16 min_io_size;
      // optimal (suggested maximum) I/O size in blocks
      u32 opt_io_size;
   } topology;
   u8 writeback;
   u8 unused0[3];
   u32 max_discard_sectors;
   u32 max_discard_seg;
   u32 discard_sector_alignment;
   u32 max_write_zeroes_sectors;
   u32 max_write_zeroes_seg;
   u8 write_zeroes_may_unmap;
   u8 unused1[3];
};

#define VIRTIO_DEV_RESET 0
#define VIRTIO_DEV_STATUS_ACK 1
#define VIRTIO_DEV_STATUS_DRIVER 2
#define VIRTIO_DEV_DRIVER_OK 4
#define VIRTIO_DEV_STATUS_OK 8


#endif
