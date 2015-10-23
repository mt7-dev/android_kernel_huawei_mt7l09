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
#include <linux/videodev2.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ioctl.h>

#include "hwcam_intf.h"

typedef struct _tag_hwcam_cfgpipeline
{
	struct v4l2_fh                              rq;

    hwcam_cfgpipeline_intf_t                    intf;
    struct kref                                 ref;
	struct list_head	                        node;

    hwcam_dev_intf_t*                           cam;
    hwcam_cfgreq_intf_t*                        cfgreq;

	struct list_head	                        streams;
} hwcam_cfgpipeline_t;

#define I2PL(i) container_of(i, hwcam_cfgpipeline_t, intf)

#define REF2PL(r) container_of(r, hwcam_cfgpipeline_t, ref)

static void
hwcam_cfgpipeline_release(
        struct kref* r)
{
    hwcam_cfgpipeline_t* pl = REF2PL(r);

    HWCAM_CFG_INFO("instance(0x%p)", pl);

    if (!list_empty(&pl->streams)) {
        HWCAM_CFG_ERR("the streams list is not empty!");
    }

    kzfree(pl);
}

static hwcam_cfgpipeline_vtbl_t s_vtbl_cfgpipeline;

static hwcam_cfgpipeline_t*
hwcam_cfgpipeline_create_instance(
        hwcam_cfgpipeline_mount_req_t* req)
{
    hwcam_cfgpipeline_t* pl = kzalloc(
            sizeof(hwcam_cfgpipeline_t), GFP_KERNEL);
    if (pl == NULL) {
        return NULL;
    }
    pl->intf.vtbl = &s_vtbl_cfgpipeline;
    kref_init(&pl->ref);

	v4l2_fh_init(&pl->rq, req->vdev);
    v4l2_fh_add(&pl->rq);

	INIT_LIST_HEAD(&pl->node);

	INIT_LIST_HEAD(&pl->streams);
    pl->cam = req->cam;

    HWCAM_CFG_INFO("instance(0x%p)", pl);

    return pl;
}

static void
hwcam_cfgpipeline_get(
        hwcam_cfgpipeline_intf_t* intf)
{
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    kref_get(&pl->ref);
}

static int
hwcam_cfgpipeline_put(
        hwcam_cfgpipeline_intf_t* intf)
{
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    return kref_put(&pl->ref, hwcam_cfgpipeline_release);
}

static int
hwcam_cfgpipeline_umount(
        hwcam_cfgpipeline_intf_t* intf)
{
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGPIPELINE_REQUEST,
    };
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGPIPELINE_REQ_UNMOUNT;

    hwcam_cfgdev_lock();
    if (!list_empty(&pl->node)) {
        list_del_init(&pl->node);
        hwcam_cfgpipeline_intf_put(&pl->intf);
    }
    hwcam_cfgdev_unlock();

    hwcam_cfgdev_send_req(NULL, &ev, &pl->rq, 1, &rc);
    return rc;
}

static hwcam_cfgstream_intf_t*
hwcam_cfgpipeline_mount_stream(
        hwcam_cfgpipeline_intf_t* intf,
        hwcam_user_intf_t* user,
        hwcam_stream_info_t* info)
{
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    int rc = -EINVAL;
    hwcam_cfgstream_mount_req_t ise =
    {
        .intf = { .vtbl = hwcam_cfgstream_mount_req_vtbl(), },
        .vdev = pl->rq.vdev,
        .streams = &pl->streams,
        .pl = intf,
        .user = user,
        .info = info,
        .buf = dma_buf_get(info->info.fd),
        .stream = NULL,
    };
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGPIPELINE_REQUEST,
    };
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)ev.u.data;

    if (ise.buf == NULL) {
        HWCAM_CFG_ERR("invalid file handle(%d)! \n", info->info.fd);
        return NULL;
    }

    req->req.intf = &ise.intf;
    req->kind = HWCAM_CFGPIPELINE_REQ_MOUNT_STREAM;

    if (hwcam_cfgdev_send_req(user, &ev, &pl->rq, 0, &rc) || rc) {
        dma_buf_put(ise.buf);
    }
    return ise.stream;
}

typedef struct _tag_hwcam_cfgpipeline_mount_buf_req
{
    hwcam_cfgreq_intf_t                       intf;
    struct dma_buf*                           buf;
} hwcam_cfgpipeline_mount_buf_req_t;

static int
hwcam_cfgpipeline_on_req_mount_buf(
        hwcam_cfgreq_intf_t* pintf,
        struct v4l2_event* ev)
{
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)&ev->u.data;
    hwcam_cfgpipeline_mount_buf_req_t* mbr =
        container_of(pintf, hwcam_cfgpipeline_mount_buf_req_t, intf);
    int fd = dma_buf_fd(mbr->buf, O_CLOEXEC);
    if (fd < 0) {
        hwcam_cfgdev_queue_ack(ev);
        return fd;
    }
    req->buf.fd = fd;
    return 0;
}

static int
hwcam_cfgpipeline_on_ack_mount_buf(
        hwcam_cfgreq_intf_t* pintf,
        hwcam_cfgack_t* ack)
{
    return 0;
}

static hwcam_cfgreq_vtbl_t
s_vtbl_req_mount_buf =
{
    .on_req = hwcam_cfgpipeline_on_req_mount_buf,
    .on_ack = hwcam_cfgpipeline_on_ack_mount_buf,
};

static int
hwcam_cfgpipeline_mount_unmount_buf(
        hwcam_cfgpipeline_intf_t* intf,
        hwcam_user_intf_t* user,
        hwcam_buf_info_t* buf,
        int mount)
{
    int rc = 0;
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    hwcam_cfgpipeline_mount_buf_req_t mbr =
    {
        .intf = { .vtbl = &s_vtbl_req_mount_buf, },
        .buf = dma_buf_get(buf->fd),
    };
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGPIPELINE_REQUEST,
    };
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)ev.u.data;

    if (mbr.buf == NULL) {
        HWCAM_CFG_ERR("invalid file handle(%d)! \n", buf->fd);
        return -EBADF;
    }

    req->req.intf = &mbr.intf;
    req->kind = mount
        ? HWCAM_CFGPIPELINE_REQ_MOUNT_BUF
        : HWCAM_CFGPIPELINE_REQ_UNMOUNT_BUF;
    req->buf = *buf;

    if (hwcam_cfgdev_send_req(user, &ev, &pl->rq, 0, &rc) || rc) {
        dma_buf_put(mbr.buf);
    }
    return rc;
}

static int
hwcam_cfgpipeline_mount_buf(
        hwcam_cfgpipeline_intf_t* intf,
        hwcam_user_intf_t* user,
        hwcam_buf_info_t* buf)
{
    return hwcam_cfgpipeline_mount_unmount_buf(
            intf, user, buf, 1);
}

static int
hwcam_cfgpipeline_unmount_buf(
        hwcam_cfgpipeline_intf_t* intf,
        hwcam_user_intf_t* user,
        hwcam_buf_info_t* buf)
{
    return hwcam_cfgpipeline_mount_unmount_buf(
            intf, user, buf, 0);
}

typedef struct _tag_hwcam_cfgpipeline_fmt_req
{
    hwcam_cfgreq_intf_t                         intf;
    hwcam_cfgpipeline_t*                        pl;
    struct v4l2_fmtdesc*                        fd;
} hwcam_cfgpipeline_fmt_req_t;

static int
hwcam_cfgpipeline_on_req_fmt(
        hwcam_cfgreq_intf_t* pintf,
        struct v4l2_event* ev)
{
    return 0;
}

static int
hwcam_cfgpipeline_on_ack_fmt(
        hwcam_cfgreq_intf_t* pintf,
        hwcam_cfgack_t* ack)
{
    return 0;
}

static hwcam_cfgreq_vtbl_t
s_vtbl_req_fmt =
{
    .on_req = hwcam_cfgpipeline_on_req_fmt,
    .on_ack = hwcam_cfgpipeline_on_ack_fmt,
};

static int
hwcam_cfgpipeline_enum_fmt(
        hwcam_cfgpipeline_intf_t* intf,
        hwcam_user_intf_t* user,
        struct v4l2_fmtdesc* fd)
{
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    int rc = -EINVAL;
    hwcam_cfgpipeline_fmt_req_t fr =
    {
        .intf = { .vtbl = &s_vtbl_req_fmt, },
        .pl = pl,
        .fd = fd,
    };
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGPIPELINE_REQUEST,
    };
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)ev.u.data;

    req->req.intf = &fr.intf;
    req->kind = HWCAM_CFGPIPELINE_REQ_ENUM_FMT;

    hwcam_cfgdev_send_req(user, &ev, &pl->rq, 0, &rc);
    return rc;
}

static int
hwcam_cfgpipeline_query_cap(
        hwcam_cfgpipeline_intf_t* intf,
        hwcam_user_intf_t* user)
{
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGPIPELINE_REQUEST,
    };
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGPIPELINE_REQ_QUERY_CAPABILITY;

    hwcam_cfgdev_send_req(user, &ev, &pl->rq, 0, &rc);
    return rc;
}

static int
hwcam_cfgpipeline_query_param(
        hwcam_cfgpipeline_intf_t* intf,
        hwcam_user_intf_t* user)
{
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGPIPELINE_REQUEST,
    };
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGPIPELINE_REQ_QUERY_PARAM;

    hwcam_cfgdev_send_req(user, &ev, &pl->rq, 0, &rc);
    return rc;
}

static int
hwcam_cfgpipeline_change_param(
        hwcam_cfgpipeline_intf_t* intf,
        hwcam_user_intf_t* user)
{
    hwcam_cfgpipeline_t* pl = I2PL(intf);
    int rc = -EINVAL;
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGPIPELINE_REQUEST,
    };
    hwcam_cfgreq2pipeline_t* req = (hwcam_cfgreq2pipeline_t*)ev.u.data;

    req->req.intf = NULL;
    req->kind = HWCAM_CFGPIPELINE_REQ_CHANGE_PARAM;

    hwcam_cfgdev_send_req(user, &ev, &pl->rq, 0, &rc);
    return rc;
}

static hwcam_cfgpipeline_vtbl_t
s_vtbl_cfgpipeline =
{
    .get = hwcam_cfgpipeline_get,
    .put = hwcam_cfgpipeline_put,
    .umount = hwcam_cfgpipeline_umount,

    .mount_buf = hwcam_cfgpipeline_mount_buf,
    .unmount_buf = hwcam_cfgpipeline_unmount_buf,

    .enum_fmt = hwcam_cfgpipeline_enum_fmt,
    .query_cap = hwcam_cfgpipeline_query_cap,

    .query_param = hwcam_cfgpipeline_query_param,
    .change_param = hwcam_cfgpipeline_change_param,
    .mount_stream = hwcam_cfgpipeline_mount_stream,
};

static long
hwcam_cfgpipeline_vo_dqevent(
        hwcam_cfgpipeline_t* pl,
        struct v4l2_event* ev,
        int nb)
{
    long rc = 0;
    hwcam_cfgreq_t* req = NULL;
    while (true) {
        rc = v4l2_event_dequeue(&pl->rq, ev, nb);
        if (rc != 0) {
            break;
        }
        req = (hwcam_cfgreq_t*)ev->u.data;
        pl->cfgreq = req->intf;
        if (!req->one_way && req->intf
                && hwcam_cfgreq_intf_on_req(req->intf, ev)) {
            continue;
        }
        else {
            break;
        }
    }
    return rc;
}

static long
hwcam_cfgpipeline_vo_notify(
        hwcam_cfgpipeline_t* pl,
        struct v4l2_event* ev)
{
    hwcam_dev_intf_notify(pl->cam, ev);
    return 0;
}

static long
hwcam_cfgpipeline_vo_mount_buf(
        hwcam_cfgpipeline_t* cam,
        hwcam_buf_info_t* bi)
{
    long rc = -EINVAL;

    switch (bi->kind)
    {
    case HWCAM_BUF_KIND_PIPELINE_CAPABILITY:
        break;
    case HWCAM_BUF_KIND_PIPELINE_PARAM:
        break;
    default:
        HWCAM_CFG_ERR("invalid buffer kind(%d)! \n", bi->kind);
        break;
    }
    return rc;
}

static long
hwcam_cfgpipeline_vo_unmount_buf(
        hwcam_cfgpipeline_t* cam,
        hwcam_buf_info_t* bi)
{
    long rc = -EINVAL;

    switch (bi->kind)
    {
    case HWCAM_BUF_KIND_PIPELINE_CAPABILITY:
        break;
    case HWCAM_BUF_KIND_PIPELINE_PARAM:
        break;
    default:
        HWCAM_CFG_ERR("invalid buffer kind(%d)! \n", bi->kind);
        break;
    }
    return rc;
}

static long
hwcam_cfgpipeline_vo_do_ioctl(
        struct file* filep,
        unsigned int cmd,
        void* arg)
{
	int rc = -EINVAL;
    hwcam_cfgpipeline_t* pl = I2PL(filep->private_data);
	BUG_ON(!pl);
    switch (cmd)
    {
    case VIDIOC_DQEVENT:
        rc = hwcam_cfgpipeline_vo_dqevent(pl,
                arg, filep->f_flags & O_NONBLOCK);
        break;
    case HWCAM_V4L2_IOCTL_NOTIFY:
        rc = hwcam_cfgpipeline_vo_notify(pl,
                (struct v4l2_event*)arg);
        break;
    case HWCAM_V4L2_IOCTL_MOUNT_BUF:
        rc = hwcam_cfgpipeline_vo_mount_buf(pl,
                (hwcam_buf_info_t*)arg);
        break;
    case HWCAM_V4L2_IOCTL_UNMOUNT_BUF:
        rc = hwcam_cfgpipeline_vo_unmount_buf(pl,
                (hwcam_buf_info_t*)arg);
        break;
    case VIDIOC_SUBSCRIBE_EVENT:
        rc = v4l2_event_subscribe(&pl->rq,
                (struct v4l2_event_subscription*)arg, 8, NULL);
        break;
    case VIDIOC_UNSUBSCRIBE_EVENT:
        rc = v4l2_event_unsubscribe(&pl->rq,
                (struct v4l2_event_subscription*)arg);
        break;
    case HWCAM_V4L2_IOCTL_REQUEST_ACK:
        rc = hwcam_cfgdev_queue_ack((struct v4l2_event*)arg);
        break;
    default:
        HWCAM_CFG_ERR("invalid IOCTL CMD(%d)! \n", cmd);
        break;
    }
    return rc;
}

static unsigned int
hwcam_cfgpipeline_vo_poll(
        struct file* filep,
        struct poll_table_struct* ptbl)
{
	unsigned int rc = 0;
    hwcam_cfgdev_lock();
    {
        void* pd = filep->private_data;
        if (pd) {
            hwcam_cfgpipeline_t* pl = I2PL(pd);
            poll_wait(filep, &pl->rq.wait, ptbl);
            if (v4l2_event_pending(&pl->rq)) {
                rc = POLLIN | POLLRDNORM;
            }
        }
    }
    hwcam_cfgdev_unlock();
	return rc;
}

static long
hwcam_cfgpipeline_vo_ioctl(
        struct file* filep,
        unsigned int cmd,
        unsigned long arg)
{
    int rc = 0;
    hwcam_cfgdev_lock();
    {
        rc = video_usercopy(filep, cmd, arg,
                hwcam_cfgpipeline_vo_do_ioctl);
    }
    hwcam_cfgdev_unlock();
    return rc;
}

static int
hwcam_cfgpipeline_vo_close(
        struct inode* i,
        struct file* filep)
{
    hwcam_cfgdev_lock();
    {
        void* pd = NULL;
        swap(pd, filep->private_data);
        if (pd) {
            hwcam_cfgpipeline_t* pl = I2PL(pd);

            if (!list_empty(&pl->node)) {
                list_del_init(&pl->node);
                hwcam_cfgpipeline_intf_put(&pl->intf);
            }

            v4l2_fh_del(&pl->rq);
            v4l2_fh_exit(&pl->rq);

            hwcam_cfgpipeline_intf_put(&pl->intf);
        }
    }
    HWCAM_CFG_INFO("hwcam_cfgpipeline_vo_close");
    hwcam_cfgdev_unlock();
    return 0;
}

static struct file_operations
s_fops_cfgpipeline =
{
    .poll = hwcam_cfgpipeline_vo_poll,
    .unlocked_ioctl = hwcam_cfgpipeline_vo_ioctl,
    .release = hwcam_cfgpipeline_vo_close,
};

static int
hwcam_cfgpipeline_on_req_mount(
        hwcam_cfgreq_intf_t* pintf,
        struct v4l2_event* ev)
{
    int fd = 0;
    hwcam_cfgpipeline_t *plo = NULL;
    hwcam_cfgpipeline_mount_req_t* ipe = NULL;
    hwcam_cfgreq2dev_t* req = NULL;

    ipe = container_of(pintf, hwcam_cfgpipeline_mount_req_t, intf);
    req = (hwcam_cfgreq2dev_t*)&ev->u.data;

    list_for_each_entry(plo, ipe->pipelines, node) {
        if (plo->cam == ipe->cam) {
            HWCAM_CFG_INFO("the pipeline had been mounted[0x%p]!", plo);
            ipe->rc = -EEXIST;
            ipe->pipeline = &plo->intf;
            hwcam_cfgpipeline_intf_get(ipe->pipeline);
            hwcam_cfgdev_queue_ack(ev);
            return -EINVAL;
        }
    }

    plo = hwcam_cfgpipeline_create_instance(ipe);
    if (plo == NULL) {
        HWCAM_CFG_ERR("failed to create pipeline!");
        return -ENOMEM;
    }

    fd = anon_inode_getfd("hwcam.cfg.pipeline",
            &s_fops_cfgpipeline, &plo->intf, O_CLOEXEC);
    if (fd < 0) {
        hwcam_cfgpipeline_intf_put(&plo->intf);
        HWCAM_CFG_ERR("failed to mount pipeline!");
        return fd;
    }
    req->pipeline.fd = fd;
    req->pipeline.moduleID = ipe->moduleID;
    ipe->pipeline = &plo->intf;
    hwcam_cfgpipeline_intf_get(ipe->pipeline);

    list_add_tail(&plo->node, ipe->pipelines);
    hwcam_cfgpipeline_intf_get(ipe->pipeline);

    return 0;
}

static int
hwcam_cfgpipeline_on_ack_mount(
        hwcam_cfgreq_intf_t* pintf,
        hwcam_cfgack_t* ack)
{
    return 0;
}

static hwcam_cfgreq_vtbl_t
s_vtbl_req_mount_pipeline =
{
    .on_req = hwcam_cfgpipeline_on_req_mount,
    .on_ack = hwcam_cfgpipeline_on_ack_mount,
};

hwcam_cfgreq_vtbl_t*
hwcam_cfgpipeline_mount_req_vtbl(void)
{
    return &s_vtbl_req_mount_pipeline;
}

