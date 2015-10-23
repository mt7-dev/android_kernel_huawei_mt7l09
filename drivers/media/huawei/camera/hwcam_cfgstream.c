/*
 *  Hisilicon K3 SOC camera driver source file
 *
 *  Copyright (C) Huawei Technology Co., Ltd.
 *
 * Author:	  h00145353
 * Email:	  alan.hefeng@huawei.com
 * Date:	  2013-11-21
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <linux/anon_inodes.h>
#include <linux/dma-buf.h>
#include <linux/debugfs.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/videodev2.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-core.h>

#include "hwcam_intf.h"

typedef struct _tag_hwcam_cfgstream
{
	struct v4l2_fh                              rq;

    hwcam_cfgstream_intf_t                      intf;
    struct kref                                 ref;
	struct list_head	                        node;

    hwcam_cfgpipeline_intf_t*                   pipeline;
    hwcam_user_intf_t*                          user;
    hwcam_cfgreq_intf_t*                        cfgreq;

    struct list_head                            bufq_idle;
    struct list_head                            bufq_busy;
    spinlock_t                                  lock_bufq;
} hwcam_cfgstream_t;

#define I2STM(i) container_of(i, hwcam_cfgstream_t, intf)

#define REF2STM(r) container_of(r, hwcam_cfgstream_t, ref)

static void
hwcam_cfgstream_release(
        struct kref* r)
{
    hwcam_cfgstream_t* stm = REF2STM(r);
    hwcam_user_intf_t* user = NULL;
    hwcam_cfgpipeline_intf_t* pipeline = NULL;

    HWCAM_CFG_INFO("instance(0x%p). \n", stm);

    swap(user, stm->user);
    swap(pipeline, stm->pipeline);

    if (user) {
        hwcam_user_intf_put(user);
    }

    if (pipeline) {
        hwcam_cfgpipeline_intf_put(pipeline);
    }

    stm->intf.vtbl = NULL;
    kzfree(stm);
}

static hwcam_cfgstream_vtbl_t s_vtbl_cfgstream;

static hwcam_cfgstream_t*
hwcam_cfgstream_create_instance(
        hwcam_cfgstream_mount_req_t* req)
{
    hwcam_cfgstream_t* stm = kzalloc(
            sizeof(hwcam_cfgstream_t), GFP_KERNEL);
    if (stm == NULL) {
        return NULL;
    }
    stm->intf.vtbl = &s_vtbl_cfgstream;
    kref_init(&stm->ref);

	v4l2_fh_init(&stm->rq, req->vdev);
    v4l2_fh_add(&stm->rq);

    stm->pipeline = req->pl;
    hwcam_cfgpipeline_intf_get(stm->pipeline);
    stm->user = req->user;
    hwcam_user_intf_get(stm->user);

    INIT_LIST_HEAD(&stm->bufq_idle);
    INIT_LIST_HEAD(&stm->bufq_busy);
    spin_lock_init(&stm->lock_bufq);

    HWCAM_CFG_INFO("instance(0x%p). \n", stm);

    return stm;
}

static void
hwcam_cfgstream_get(
        hwcam_cfgstream_intf_t* intf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    kref_get(&stm->ref);
}

static int
hwcam_cfgstream_put(
        hwcam_cfgstream_intf_t* intf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    return kref_put(&stm->ref, hwcam_cfgstream_release);
}

static int
hwcam_cfgstream_umount(
        hwcam_cfgstream_intf_t* intf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGSTREAM_REQ_UNMOUNT;

    HWCAM_CFG_INFO("instance(0x%p). \n", stm);

    hwcam_cfgdev_lock();
    list_del_init(&stm->node);
    hwcam_cfgdev_unlock();

    hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 1, &rc);
    return rc;
}

typedef struct _tag_hwcam_cfgstream_fmt_req
{
    hwcam_cfgreq_intf_t                       intf;
    hwcam_cfgstream_t*                        stm;
    struct v4l2_format*                       fmt;
} hwcam_cfgstream_fmt_req_t;

static int
hwcam_cfgstream_on_req_fmt(
        hwcam_cfgreq_intf_t* pintf,
        struct v4l2_event* ev)
{
    return 0;
}

static int
hwcam_cfgstream_on_ack_fmt(
        hwcam_cfgreq_intf_t* pintf,
        hwcam_cfgack_t* ack)
{
    return 0;
}

static hwcam_cfgreq_vtbl_t
s_vtbl_req_fmt =
{
    .on_req = hwcam_cfgstream_on_req_fmt,
    .on_ack = hwcam_cfgstream_on_ack_fmt,
};

static inline int
hwcam_cfgstream_try_fmt(
    hwcam_cfgstream_intf_t* intf,
    struct v4l2_format* fmt)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = 0;
    hwcam_cfgstream_fmt_req_t fr =
    {
        .intf = { .vtbl = &s_vtbl_req_fmt, },
        .stm = stm,
        .fmt = fmt,
    };
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;

    req->req.intf = &fr.intf;
    req->kind = HWCAM_CFGSTREAM_REQ_TRY_FMT;

    hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 0, &rc);
    return rc;
}

typedef struct _tag_hwcam_cfgstream_mount_buf_req
{
    hwcam_cfgreq_intf_t                       intf;
    struct dma_buf*                           buf;
} hwcam_cfgstream_mount_buf_req_t;

static int
hwcam_cfgstream_on_req_mount_buf(
        hwcam_cfgreq_intf_t* pintf,
        struct v4l2_event* ev)
{
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)&ev->u.data;
    hwcam_cfgstream_mount_buf_req_t* mbr =
        container_of(pintf, hwcam_cfgstream_mount_buf_req_t, intf);
    int fd = dma_buf_fd(mbr->buf, O_CLOEXEC);
    if (fd < 0) {
        HWCAM_CFG_ERR("failed to get fd for buffer! \n");
        hwcam_cfgdev_queue_ack(ev);
        return fd;
    }
    req->buf.fd = fd;
    return 0;
}

static int
hwcam_cfgstream_on_ack_mount_buf(
        hwcam_cfgreq_intf_t* pintf,
        hwcam_cfgack_t* ack)
{
    return 0;
}

static hwcam_cfgreq_vtbl_t
s_vtbl_req_mount_buf =
{
    .on_req = hwcam_cfgstream_on_req_mount_buf,
    .on_ack = hwcam_cfgstream_on_ack_mount_buf,
};

static inline int
hwcam_cfgstream_mount_unmount_buf(
        hwcam_cfgstream_intf_t* intf,
        hwcam_buf_info_t* buf,
        int mount)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = -EINVAL;
    hwcam_cfgstream_mount_buf_req_t mbr =
    {
        .intf = { .vtbl = &s_vtbl_req_mount_buf, },
        .buf = dma_buf_get(buf->fd),
    };
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;

    if (mbr.buf == NULL) {
        HWCAM_CFG_ERR("invalid file handle(%d)! \n", buf->fd);
        return -EBADF;
    }

    req->req.intf = &mbr.intf;
    req->kind = mount
        ? HWCAM_CFGSTREAM_REQ_MOUNT_BUF
        : HWCAM_CFGSTREAM_REQ_UNMOUNT_BUF;
    req->buf = *buf;

    if (hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 0, &rc) < 0) {
        dma_buf_put(mbr.buf);
    }
    return rc;
}

static int
hwcam_cfgstream_mount_buf(
        hwcam_cfgstream_intf_t* intf,
        hwcam_buf_info_t* buf)
{
    return hwcam_cfgstream_mount_unmount_buf(
            intf, buf, 1);
}

static int
hwcam_cfgstream_unmount_buf(
        hwcam_cfgstream_intf_t* intf,
        hwcam_buf_info_t* buf)
{
    return hwcam_cfgstream_mount_unmount_buf(
            intf, buf, 0);
}

typedef struct _tag_hwcam_cfgstream_mount_graphic_buf_req
{
    hwcam_cfgreq_intf_t                       intf;
    hwcam_graphic_buf_info_t*                 buf;
    union
    {
        struct file*                          objs[HWCAM_GRAPHIC_BUF_INFO_LENGTH];
        int                                   ints[HWCAM_GRAPHIC_BUF_INFO_LENGTH];
    };
} hwcam_cfgstream_mount_graphic_buf_req_t;

static int
hwcam_cfgstream_on_req_mount_graphic_buf(
        hwcam_cfgreq_intf_t* pintf,
        struct v4l2_event* ev)
{
    int i = 0;
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)&ev->u.data;
    hwcam_cfgstream_mount_graphic_buf_req_t* mbr =
        container_of(pintf, hwcam_cfgstream_mount_graphic_buf_req_t, intf);

    for (; i < mbr->buf->num_fds; i++) {
        mbr->buf->data[i] = get_unused_fd_flags(O_CLOEXEC);
        if (mbr->buf->data[i] < 0) {
            goto fail_get_unused_fd;
        }
    }
    for (; i != 0; i--) {
        fd_install(mbr->buf->data[i - 1], mbr->objs[i - 1]);
    }
    return 0;

fail_get_unused_fd:
    req->req.rc = mbr->buf->data[i];
    for (; i != 0; i--) {
        put_unused_fd(mbr->buf->data[i - 1]);
    }
    return -EINVAL;
}

static int
hwcam_cfgstream_on_ack_mount_graphic_buf(
        hwcam_cfgreq_intf_t* pintf,
        hwcam_cfgack_t* ack)
{
    return 0;
}

static hwcam_cfgreq_vtbl_t
s_vtbl_req_mount_graphic_buf =
{
    .on_req = hwcam_cfgstream_on_req_mount_graphic_buf,
    .on_ack = hwcam_cfgstream_on_ack_mount_graphic_buf,
};

static int
hwcam_cfgstream_mount_graphic_buf(
        hwcam_cfgstream_intf_t* intf,
        hwcam_graphic_buf_info_t* buf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = -EINVAL;
    hwcam_cfgstream_mount_graphic_buf_req_t mbr =
    {
        .intf = { .vtbl = &s_vtbl_req_mount_graphic_buf, },
        .buf = buf,
    };
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;
    int i = 0;

    if (HWCAM_GRAPHIC_BUF_INFO_LENGTH <= buf->num_ints + buf->num_fds) {
        return rc;
    }
    memset(mbr.objs, 0, sizeof(mbr.objs));
    for (; i < buf->num_fds; i++) {
        mbr.objs[i] = fget(buf->data[i]);
        if (mbr.objs[i] == NULL) {
            HWCAM_CFG_ERR("invalid file handle(%d)! \n", buf->data[i]);
            goto fail_to_fget;
        }
    }
    for (; i < buf->num_fds + buf->num_ints; i++) {
        mbr.ints[i] = buf->data[i];
    }

    req->req.intf = &mbr.intf;
    req->kind = HWCAM_CFGSTREAM_REQ_MOUNT_GRAPHIC_BUF;

    if (hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 0, &rc) < 0) {
        i = buf->num_fds;
        goto fail_to_fget;
    }
    return rc;

fail_to_fget:
    for (; i != 0; i--) {
        fput(mbr.objs[i - 1]);
    }
    return rc;
}

static int
hwcam_cfgstream_unmount_graphic_buf(
        hwcam_cfgstream_intf_t* intf,
        int index)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGSTREAM_REQ_UNMOUNT_GRAPHIC_BUF;
    req->unmount_graphic_buf.index = index;

    hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 0, &rc);
    return rc;
}

static int
hwcam_cfgstream_query_param(
        hwcam_cfgstream_intf_t* intf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGSTREAM_REQ_QUERY_PARAM;

    hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 0, &rc);
    return rc;
}

static int
hwcam_cfgstream_change_param(
        hwcam_cfgstream_intf_t* intf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGSTREAM_REQ_CHANGE_PARAM;

    hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 0, &rc);
    return rc;
}

static void
hwcam_cfgstream_buf_queue(
         hwcam_cfgstream_intf_t* intf,
         hwcam_vbuf_t* buf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    unsigned long flags;

    spin_lock_irqsave(&stm->lock_bufq, flags);
    list_add_tail(&buf->node, &stm->bufq_idle);
    spin_unlock_irqrestore(&stm->lock_bufq, flags);

    wake_up_all(&stm->rq.wait);
}

static int
hwcam_cfgstream_start(
        hwcam_cfgstream_intf_t* intf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGSTREAM_REQ_START;

    hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 0, &rc);
    return rc;
}

static int
hwcam_cfgstream_stop(
        hwcam_cfgstream_intf_t* intf)
{
    hwcam_cfgstream_t* stm = I2STM(intf);
    int rc = -EINVAL;
    unsigned long flags;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGSTREAM_REQUEST,
    };
    hwcam_cfgreq2stream_t* req = (hwcam_cfgreq2stream_t*)ev.u.data;

    spin_lock_irqsave(&stm->lock_bufq, flags);
    INIT_LIST_HEAD(&stm->bufq_idle);
    INIT_LIST_HEAD(&stm->bufq_busy);
    spin_unlock_irqrestore(&stm->lock_bufq, flags);

    req->req.intf = NULL;
    req->kind = HWCAM_CFGSTREAM_REQ_STOP;

    hwcam_cfgdev_send_req(stm->user, &ev, &stm->rq, 0, &rc);
    return rc;
}

static hwcam_cfgstream_vtbl_t
s_vtbl_cfgstream =
{
    .get = hwcam_cfgstream_get,
    .put = hwcam_cfgstream_put,
    .umount = hwcam_cfgstream_umount,

    .try_fmt = hwcam_cfgstream_try_fmt,

    .mount_buf = hwcam_cfgstream_mount_buf,
    .unmount_buf = hwcam_cfgstream_unmount_buf,
    .mount_graphic_buf = hwcam_cfgstream_mount_graphic_buf,
    .unmount_graphic_buf = hwcam_cfgstream_unmount_graphic_buf,
    .query_param = hwcam_cfgstream_query_param,
    .change_param = hwcam_cfgstream_change_param,

    .buf_queue = hwcam_cfgstream_buf_queue,

    .start = hwcam_cfgstream_start,
    .stop = hwcam_cfgstream_stop,
};

static long
hwcam_cfgstream_vo_dqevent(
        hwcam_cfgstream_t* stm,
        struct v4l2_event* ev,
        int nb)
{
    long rc = 0;
    hwcam_cfgreq_t* req = NULL;
    while (true) {
        rc = v4l2_event_dequeue(&stm->rq, ev, nb);
        if (rc != 0) {
            break;
        }
        req = (hwcam_cfgreq_t*)ev->u.data;
        stm->cfgreq = req->intf;
        if (!req->one_way && req->intf &&
                hwcam_cfgreq_intf_on_req(req->intf, ev)) {
            continue;
        }
        else {
            break;
        }
    }
    return rc;
}

static long
hwcam_cfgstream_vo_notify(
        hwcam_cfgstream_t* stm,
        struct v4l2_event* ev)
{
    hwcam_user_intf_notify(stm->user, ev);
    return 0;
}

static long
hwcam_cfgstream_vo_mount_buf(
        hwcam_cfgstream_t* stm,
        hwcam_buf_info_t* bi)
{
    long rc = -EINVAL;
    switch (bi->kind)
    {
    case HWCAM_BUF_KIND_STREAM_PARAM:
        break;
    default:
        HWCAM_CFG_ERR("invalid buffer kind(%d)! \n", bi->kind);
        break;
    }
    return rc;
}

static long
hwcam_cfgstream_vo_unmount_buf(
        hwcam_cfgstream_t* stm,
        hwcam_buf_info_t* bi)
{
    long rc = -EINVAL;
    switch (bi->kind)
    {
    case HWCAM_BUF_KIND_STREAM_PARAM:
        break;
    default:
        HWCAM_CFG_ERR("invalid buffer kind(%d)! \n", bi->kind);
        break;
    }
    return rc;
}

static long
hwcam_cfgstream_vo_get_graphic_buf(
        hwcam_cfgstream_t* stm,
        hwcam_graphic_buf_info_t* bi)
{
    long rc = -EINVAL;
    if (stm->cfgreq != NULL
            && stm->cfgreq->vtbl == &s_vtbl_req_mount_graphic_buf) {
        hwcam_cfgstream_mount_graphic_buf_req_t* req =
            container_of(stm->cfgreq, hwcam_cfgstream_mount_graphic_buf_req_t, intf);
        *bi = *req->buf;
        rc = 0;
        HWCAM_CFG_ERR("hwcam_cfgstream_vo_get_graphic_buf");
    }
    else {
        HWCAM_CFG_ERR("hwcam_cfgstream_vo_get_graphic_buf");
    }
    return rc;
}

static int
hwcam_cfgstream_vo_get_buf(
        hwcam_cfgstream_t* stm,
        int* index)
{
    hwcam_vbuf_t* ret = NULL;
    unsigned long flags;

    spin_lock_irqsave(&stm->lock_bufq, flags);
    if (!list_empty(&stm->bufq_idle)) {
        ret = list_first_entry(&stm->bufq_idle, hwcam_vbuf_t, node);
        list_move_tail(&ret->node, &stm->bufq_busy);
    }
    spin_unlock_irqrestore(&stm->lock_bufq, flags);
    if (ret) {
        *index = ret->buf.v4l2_buf.index;
        return 0;
    }
    else {
        *index = -1;
        return -ENOMEM;
    }
}

static int
hwcam_cfgstream_vo_put_buf(
        hwcam_cfgstream_t* stm,
        int* index)
{
    int rc = -EINVAL;
    hwcam_vbuf_t* entry = NULL;
    hwcam_vbuf_t* tmp = NULL;
    unsigned long flags;

    spin_lock_irqsave(&stm->lock_bufq, flags);
    list_for_each_entry_safe(entry, tmp, &stm->bufq_busy, node) {
        if (entry->buf.v4l2_buf.index == *index) {
            list_move(&entry->node, &stm->bufq_idle);
            rc = 0;
            break;
        }
    }
    spin_unlock_irqrestore(&stm->lock_bufq, flags);
    return rc;
}

static int
hwcam_cfgstream_vo_buf_done(
        hwcam_cfgstream_t* stm,
        int* index)
{
    int rc = -EINVAL;
    hwcam_vbuf_t* entry = NULL;
    hwcam_vbuf_t* tmp = NULL;
    unsigned long flags;

    spin_lock_irqsave(&stm->lock_bufq, flags);
    list_for_each_entry_safe(entry, tmp, &stm->bufq_busy, node) {
        if (entry->buf.v4l2_buf.index == *index) {
            list_del_init(&entry->node);
            vb2_buffer_done(&entry->buf, VB2_BUF_STATE_DONE);
            rc = 0;
            break;
        }
    }
    spin_unlock_irqrestore(&stm->lock_bufq, flags);
    return rc;
}

static long
hwcam_cfgstream_vo_get_fmt(
        hwcam_cfgstream_t* stm,
        struct v4l2_format* fmt)
{
    long rc = -EINVAL;
    if (stm->cfgreq != NULL && stm->cfgreq->vtbl == &s_vtbl_req_fmt) {
        hwcam_cfgstream_fmt_req_t* req =
            container_of(stm->cfgreq, hwcam_cfgstream_fmt_req_t, intf);
        *fmt = *req->fmt;
        rc = 0;
    }
    return rc;
}

static long
hwcam_cfgstream_vo_set_fmt(
        hwcam_cfgstream_t* stm,
        struct v4l2_format* fmt)
{
    long rc = -EINVAL;
    if (stm->cfgreq != NULL && stm->cfgreq->vtbl == &s_vtbl_req_fmt) {
        hwcam_cfgstream_fmt_req_t* req =
            container_of(stm->cfgreq, hwcam_cfgstream_fmt_req_t, intf);
        *req->fmt = *fmt;
        rc = 0;
    }
    return rc;
}

static long
hwcam_cfgstream_vo_do_ioctl(
        struct file* filep,
        unsigned int cmd,
        void* arg)
{
    long rc = -EINVAL;
    hwcam_cfgstream_t* stm = I2STM(filep->private_data);
	BUG_ON(!filep->private_data);
    switch (cmd)
    {
    case VIDIOC_DQEVENT:
        rc = hwcam_cfgstream_vo_dqevent(stm,
                arg, filep->f_flags & O_NONBLOCK);
        break;
    case HWCAM_V4L2_IOCTL_NOTIFY:
        rc = hwcam_cfgstream_vo_notify(stm, arg);
        break;
    case HWCAM_V4L2_IOCTL_MOUNT_BUF:
        rc = hwcam_cfgstream_vo_mount_buf(stm, arg);
        break;
    case HWCAM_V4L2_IOCTL_UNMOUNT_BUF:
        rc = hwcam_cfgstream_vo_unmount_buf(stm, arg);
        break;
    case HWCAM_V4L2_IOCTL_GET_GRAPHIC_BUF:
        rc = hwcam_cfgstream_vo_get_graphic_buf(stm, arg);
        break;
    case HWCAM_V4L2_IOCTL_GET_BUF:
        rc = hwcam_cfgstream_vo_get_buf(stm, arg);
        break;
    case HWCAM_V4L2_IOCTL_PUT_BUF:
        rc = hwcam_cfgstream_vo_put_buf(stm, arg);
        break;
    case HWCAM_V4L2_IOCTL_BUF_DONE:
        rc = hwcam_cfgstream_vo_buf_done(stm, arg);
        break;
    case VIDIOC_G_FMT:
        rc = hwcam_cfgstream_vo_get_fmt(stm, arg);
        break;
    case VIDIOC_S_FMT:
        rc = hwcam_cfgstream_vo_set_fmt(stm, arg);
        break;
    case VIDIOC_SUBSCRIBE_EVENT:
        rc = v4l2_event_subscribe(&stm->rq, arg, 32, NULL);
        break;
    case VIDIOC_UNSUBSCRIBE_EVENT:
        rc = v4l2_event_unsubscribe(&stm->rq, arg);
        break;
    case HWCAM_V4L2_IOCTL_REQUEST_ACK:
        rc = hwcam_cfgdev_queue_ack(arg);
        break;
    default:
        HWCAM_CFG_ERR("invalid IOCTL CMD(%d)! \n", cmd);
        break;
    }
    return rc;
}

static unsigned int
hwcam_cfgstream_vo_poll(
        struct file* filep,
        struct poll_table_struct* ptbl)
{
    unsigned int rc = 0;
    hwcam_cfgdev_lock();
    {
        void* pd = filep->private_data;
        if (pd) {
            hwcam_cfgstream_t* stm = I2STM(pd);
            unsigned long flags;

            poll_wait(filep, &stm->rq.wait, ptbl);

            spin_lock_irqsave(&stm->lock_bufq, flags);
            if (!list_empty(&stm->bufq_idle)) {
                rc |= POLLIN | POLLRDNORM;
            }
            spin_unlock_irqrestore(&stm->lock_bufq, flags);

            if (v4l2_event_pending(&stm->rq)) {
                rc |= POLLIN | POLLRDNORM;
            }
        }
    }
    hwcam_cfgdev_unlock();
	return rc;
}

static long
hwcam_cfgstream_vo_ioctl(
        struct file* filep,
        unsigned int cmd,
        unsigned long arg)
{
    int rc = 0;
    hwcam_cfgdev_lock();
    {
        rc =  video_usercopy(filep, cmd, arg,
                hwcam_cfgstream_vo_do_ioctl);
    }
    hwcam_cfgdev_unlock();
    return rc;
}

static int
hwcam_cfgstream_vo_close(
        struct inode* i,
        struct file* filep)
{
    hwcam_cfgdev_lock();
    {
        void* pd = NULL;
        swap(pd, filep->private_data);
        if (pd) {
            hwcam_cfgstream_t* stm = I2STM(pd);

            v4l2_fh_del(&stm->rq);
            v4l2_fh_exit(&stm->rq);

            hwcam_cfgstream_intf_put(&stm->intf);
        }
    }
    hwcam_cfgdev_unlock();
    HWCAM_CFG_INFO("hwcam_cfgstream_vo_close");
    return 0;
}

static struct file_operations
s_fops_cfgstream =
{
    .poll = hwcam_cfgstream_vo_poll,
    .unlocked_ioctl = hwcam_cfgstream_vo_ioctl,
    .release = hwcam_cfgstream_vo_close,
};

static int
hwcam_cfgstream_on_req_mount(
        hwcam_cfgreq_intf_t* pintf,
        struct v4l2_event* ev)
{
    int rc = 0;
    hwcam_cfgstream_t* so = NULL;
    hwcam_cfgstream_mount_req_t* ise =
        container_of(pintf, hwcam_cfgstream_mount_req_t, intf);
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)&ev->u.data;

    list_for_each_entry(so, ise->streams, node) {
        if (so->user == ise->user) {
            HWCAM_CFG_INFO("the stream had been mounted! \n");
            ise->stream = &so->intf;
            rc = -EINVAL;
            goto mount_fail_exit;
        }
    }

    req->stream.info = ise->info->info;
    req->stream.info.fd = dma_buf_fd(ise->buf, O_CLOEXEC);
    req->stream.handle = NULL;
    if (req->stream.info.fd < 0) {
        HWCAM_CFG_ERR("failed to get fd for buffer! \n");
        rc = req->stream.info.fd;
        goto mount_fail_exit;
    }

    so = hwcam_cfgstream_create_instance(ise);
    if (so == NULL) {
        HWCAM_CFG_ERR("failed to create stream! \n");
        rc = -ENOMEM;
        goto mount_fail_create_instance;
    }

    req->stream.fd = anon_inode_getfd("hwcam.cfg.stream",
            &s_fops_cfgstream, &so->intf, O_CLOEXEC);
    if (req->stream.fd < 0) {
        HWCAM_CFG_ERR("failed to mount stream! \n");
        rc = req->stream.fd;
        goto mount_fail_getfd;
    }

    ise->stream = &so->intf;
    hwcam_cfgstream_intf_get(ise->stream);

    list_add_tail(&so->node, ise->streams);

    return 0;

mount_fail_getfd:
    hwcam_cfgstream_intf_put(&so->intf);

mount_fail_create_instance:
    sys_close(req->stream.info.fd);

mount_fail_exit:
    hwcam_cfgdev_queue_ack(ev);
    return rc;
}

static int
hwcam_cfgstream_on_ack_mount(
        hwcam_cfgreq_intf_t* pintf,
        hwcam_cfgack_t* ack)
{
    hwcam_cfgstream_mount_req_t* ise =
        container_of(pintf, hwcam_cfgstream_mount_req_t, intf);
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)&ack->ev.u.data;
    ise->info->handle = req->stream.handle;
    return 0;
}

static hwcam_cfgreq_vtbl_t
s_vtbl_req_mount_stream =
{
    .on_req = hwcam_cfgstream_on_req_mount,
    .on_ack = hwcam_cfgstream_on_ack_mount,
};

hwcam_cfgreq_vtbl_t*
hwcam_cfgstream_mount_req_vtbl(void)
{
    return &s_vtbl_req_mount_stream;
}

hwcam_cfgstream_intf_t*
hwcam_cfgstream_get_by_fd(int fd)
{
    struct file* fobj = fget(fd);
    hwcam_cfgstream_intf_t* stm = NULL;

    if (fobj) {
        if (fobj->f_op == &s_fops_cfgstream) {
            stm = (hwcam_cfgstream_intf_t*)fobj->private_data;
            hwcam_cfgstream_intf_get(stm);
        }
        fput(fobj);
    }
    if (!stm) {
        HWCAM_CFG_ERR("the fd(%d) is not a valid cfgstream object! \n", fd);
    }
    return stm;
}

