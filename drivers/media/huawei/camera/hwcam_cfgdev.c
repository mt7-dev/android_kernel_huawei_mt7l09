/*
 *  Hisilicon K3 SOC camera driver source file
 *
 *  Copyright (C) Huawei Technology Co., Ltd.
 *
 * Author:	  h00145353
 * Email:	  alan.hefeng@huawei.com
 * Date:	  2013-10-29
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


#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/hisi_ion.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/videodev2.h>
#include <media/media-device.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ioctl.h>

#include "hwcam_intf.h"
#include "cam_log.h"

typedef struct _tag_hwcam_cfgdev_vo
{
    struct v4l2_device                          v4l2;
    struct video_device*                        vdev;
    struct media_device*                        mdev;
    hwcam_cfgreq_intf_t*                        cfgreq;

    struct dentry*                              debug_root;
    hwcam_user_intf_t *                         notify_user;
    struct ion_client*                          ion;
	struct v4l2_fh                              rq;
    struct list_head                            pipelines;
} hwcam_cfgdev_vo_t;

static hwcam_cfgdev_vo_t s_cfgdev;
DEFINE_MUTEX(s_cfgdev_lock);

void
hwcam_cfgdev_lock(void)
{
    mutex_lock(&s_cfgdev_lock);
}

void
hwcam_cfgdev_unlock(void)
{
    mutex_unlock(&s_cfgdev_lock);
}

static atomic_t s_sequence = ATOMIC_INIT(0);

static void hwcam_cfgdev_work(struct work_struct* w);
static DEFINE_SPINLOCK(s_ack_queue_lock);
static struct list_head s_ack_queue = LIST_HEAD_INIT(s_ack_queue);
static DECLARE_WAIT_QUEUE_HEAD(s_wait_ack);
static DECLARE_DELAYED_WORK(s_cfgdev_work, hwcam_cfgdev_work);

static void
hwcam_cfgdev_work(
        struct work_struct* w)
{
    mutex_lock(&s_cfgdev_lock);
    mutex_unlock(&s_cfgdev_lock);
}

static void
hwcam_cfgdev_release_ack(
        hwcam_cfgack_t* ack)
{
    kzfree(ack);
}

int
hwcam_cfgdev_queue_ack(
        struct v4l2_event* ev)
{
    unsigned long flags = 0;
    hwcam_cfgack_t* ack = NULL;
    hwcam_cfgreq_t* req = (hwcam_cfgreq_t*)ev->u.data;
    if (req->one_way) {
        HWCAM_CFG_ERR("need NOT acknowledge an one way request(%ld)! \n",
                req->seq);
        return -EINVAL;
    }
    ack = kzalloc(sizeof(hwcam_cfgack_t), GFP_KERNEL);
    if (ack == NULL) {
        HWCAM_CFG_ERR("out of memory for ack! \n");
        return -ENOMEM;
    }
    ack->ev = *ev;
    ack->release = hwcam_cfgdev_release_ack;

    spin_lock_irqsave(&s_ack_queue_lock, flags);
    list_add_tail(&ack->node, &s_ack_queue);
    wake_up_all(&s_wait_ack);
    spin_unlock_irqrestore(&s_ack_queue_lock, flags);
    return 0;
}

bool
hwcam_cfgdev_check_ack(
        hwcam_cfgreq_t* req,
        hwcam_cfgack_t** ppack)
{
    bool ret = false;
    hwcam_cfgack_t* ack = NULL;
    hwcam_cfgack_t* tmp = NULL;
    hwcam_cfgreq_t* back = NULL;
    unsigned long flags = 0;

    spin_lock_irqsave(&s_ack_queue_lock, flags);
    list_for_each_entry_safe(ack, tmp, &s_ack_queue, node) {
        back = (hwcam_cfgreq_t*)ack->ev.u.data;
        if (req->user == back->user
                && req->intf == back->intf
                && req->seq == back->seq) {
            ret = true;
            *ppack = ack;
            list_del(&ack->node);
            break;
        }
    }
    spin_unlock_irqrestore(&s_ack_queue_lock, flags);
    return ret;
}

enum
{
    HWCAM_WAIT4ACK_TIME                         =   10000,   // 5000ms
};

static int
hwcam_cfgdev_wait_ack(
        hwcam_user_intf_t* user,
        hwcam_cfgreq_t* req,
        int timeout,
        int* ret)
{
    int rc = msecs_to_jiffies(timeout);
    hwcam_cfgack_t* ack = NULL;

    if (ret) { *ret = -EINVAL; }
    while (rc > 0) {
        hwcam_user_intf_wait_begin(user);
        rc = wait_event_timeout(s_wait_ack,
                hwcam_cfgdev_check_ack(req, &ack),
                rc);
        hwcam_user_intf_wait_end(user);
        if (ack != NULL) {
            break;
        }
    }
    if ((req->intf != NULL) && (ack != NULL))  {
        hwcam_cfgreq_intf_on_ack(req->intf, ack);
    }
    if (ack == NULL) {
        HWCAM_CFG_ERR("the user(0x%p) is out of time for ACK(%ld)! \n",
                user, req->seq);
        rc = -EBUSY;
    }
    else {
        if (ret) { *ret = req->rc; }
        ack->release(ack);
        rc = 0;
    }
    return rc;
}

int
hwcam_cfgdev_send_req(
        hwcam_user_intf_t* user,
        struct v4l2_event* ev,
        struct v4l2_fh* target,
        int one_way,
        int* ret)
{
    int rc = 0;
    bool queued = false;
    hwcam_cfgreq_t* req = (hwcam_cfgreq_t*)ev->u.data;
    req->user = user;
    req->seq = atomic_add_return(1, &s_sequence);
    req->one_way = one_way ? 1 : 0;

    mutex_lock(&s_cfgdev_lock);
    if (target->vdev) {
        v4l2_event_queue_fh(target, ev);
        queued = true;
    }
    else {
        HWCAM_CFG_ERR("the target is invalid!");
        rc = -ENOENT;
    }
    mutex_unlock(&s_cfgdev_lock);

    if (queued && !req->one_way) {
        return hwcam_cfgdev_wait_ack(
                user, req, HWCAM_WAIT4ACK_TIME, ret);
    }
    return rc;
}

hwcam_data_table_t*
hwcam_cfgdev_import_data_table(
        char const* name,
        hwcam_buf_info_t const* bi,
        struct ion_handle** handle)
{
    hwcam_data_table_t* tbl = NULL;
    mutex_lock(&s_cfgdev_lock);
    if (s_cfgdev.ion) {
        struct ion_handle* hdl =
            ion_import_dma_buf(s_cfgdev.ion, bi->fd);
        if (hdl) {
            tbl = ion_map_kernel(s_cfgdev.ion, hdl);
            if (tbl) {
                if (!strncmp(tbl->name, name,
                            HWCAM_DATA_TABLE_NAME_SIZE)) {
                    *handle = hdl;
                }
                else {
                    HWCAM_CFG_ERR("invalid data table type(%d)!", bi->fd);
                    ion_unmap_kernel(s_cfgdev.ion, hdl);
                }
            }
            else {
                HWCAM_CFG_ERR("failed to map ion buffer(%d)!", bi->fd);
                ion_free(s_cfgdev.ion, hdl);
            }
        }
        else {
            HWCAM_CFG_ERR("failed to import ion buffer(%d)!", bi->fd);
        }
    }
    mutex_unlock(&s_cfgdev_lock);
    return tbl;
}

void
hwcam_cfgdev_release_data_table(
        struct ion_handle* handle)
{
    mutex_lock(&s_cfgdev_lock);
    if (s_cfgdev.ion && handle) {
        ion_unmap_kernel(s_cfgdev.ion, handle);
        ion_free(s_cfgdev.ion, handle);
    }
    mutex_unlock(&s_cfgdev_lock);
}

void*
hwcam_cfgdev_import_graphic_buffer(hwcam_buf_info_t const* bi)
{
    return NULL;
}

void
hwcam_cfgdev_release_graphic_buffer(void)
{
}

int
hwcam_cfgdev_mount_pipeline(
        hwcam_user_intf_t* user,
        hwcam_dev_intf_t* cam,
        int moduleID,
        hwcam_cfgpipeline_intf_t** pl)
{
    hwcam_cfgpipeline_mount_req_t ipe =
    {
        .intf = { .vtbl = hwcam_cfgpipeline_mount_req_vtbl(), },
        .vdev = s_cfgdev.vdev,
        .pipelines = &s_cfgdev.pipelines,
        .cam = cam,
        .moduleID = moduleID,
        .pipeline = NULL,
        .rc = 0,
    };
    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_CFGDEV_REQUEST,
    };
    int rc = -EINVAL;
    hwcam_cfgreq2dev_t* req = (hwcam_cfgreq2dev_t*)ev.u.data;

    req->req.intf = &ipe.intf;
    req->kind = HWCAM_CFGDEV_REQ_MOUNT_PIPELINE;

    hwcam_cfgdev_send_req(user, &ev, &s_cfgdev.rq, 0, &rc);

    if (NULL == ipe.pipeline || 0 != ipe.rc)
        goto mount_end;

    if (NULL == s_cfgdev.notify_user) {
        s_cfgdev.notify_user = user;
        hwcam_user_intf_get(s_cfgdev.notify_user);
    } else {
        hwcam_user_intf_put(s_cfgdev.notify_user);
        s_cfgdev.notify_user = user;
        hwcam_user_intf_get(s_cfgdev.notify_user);
    }
mount_end:
    *pl = ipe.pipeline;
    return ipe.rc;
}

static void
hwcam_cfgdev_subdev_release(
        struct video_device *vdev)
{
    struct v4l2_subdev *sd = video_get_drvdata(vdev);
    if(sd == NULL)
        return;
    sd->devnode = NULL;
    video_device_release(vdev);

    HWCAM_CFG_INFO("TODO. \n");
}

static unsigned int
hwcam_subdev_poll(
        struct file* file,
        poll_table* wait)
{
	struct video_device* vdev = video_devdata(file);
	struct v4l2_subdev* sd = vdev_to_v4l2_subdev(vdev);
	struct v4l2_fh* fh = file->private_data;

	if(sd==NULL){
        return POLLERR;
	}

	if (!(sd->flags & V4L2_SUBDEV_FL_HAS_EVENTS)) {
        return POLLERR;
    }

	poll_wait(file, &fh->wait, wait);

	if (v4l2_event_pending(fh)) {
		return POLLIN;
    }

	return 0;
}

struct v4l2_file_operations
hwcam_v4l2_subdev_fops =
{
};

int
hwcam_cfgdev_register_subdev(
        struct v4l2_subdev* sd)
{
	int rc = 0;
	struct video_device* vdev = NULL;
    struct v4l2_device* v4l2 = &s_cfgdev.v4l2;

	if (!sd || !sd->name[0]) {
		rc = -EINVAL;
        goto out;
    }

	rc = v4l2_device_register_subdev(v4l2, sd);
	if (rc < 0) {
        goto out;
    }

    if (!(sd->flags & V4L2_SUBDEV_FL_HAS_DEVNODE)) {
        goto out;
    }

    vdev = video_device_alloc();
    if (!vdev) {
        rc = -ENOMEM;
        goto video_alloc_fail;
    }

	video_set_drvdata(vdev, sd);
	strlcpy(vdev->name, sd->name, sizeof(vdev->name));
	vdev->v4l2_dev = v4l2;
	vdev->fops = &hwcam_v4l2_subdev_fops;
	vdev->release = hwcam_cfgdev_subdev_release;
	rc = __video_register_device(
            vdev, VFL_TYPE_SUBDEV, -1, 1, sd->owner);
	if (rc < 0) {
        goto video_register_fail;
	}
	cam_debug("register video devices %s sucessful",sd->name);
	cam_debug("video dev name %s %s",vdev->dev.kobj.name,vdev->name);
	sd->entity.info.v4l.major = VIDEO_MAJOR;
	sd->entity.info.v4l.minor = vdev->minor;
	sd->entity.name = video_device_node_name(vdev);
	sd->devnode = vdev;
	goto out;

video_register_fail:
    video_device_release(vdev);

video_alloc_fail:
    if (sd->devnode) {
        v4l2_device_unregister_subdev(sd);
    }

out:
    return rc;
}

int
hwcam_cfgdev_unregister_subdev(
        struct v4l2_subdev* sd)
{
	if (!sd) {
		return -EINVAL;
    }
	v4l2_device_unregister_subdev(sd);
	return 0;
}

static unsigned int
hwcam_cfgdev_vo_poll(
        struct file* filep,
        struct poll_table_struct* ptbl)
{
	unsigned int rc = 0;
	poll_wait(filep, &s_cfgdev.rq.wait, ptbl);
	if (v4l2_event_pending(&s_cfgdev.rq)) {
		rc = POLLIN | POLLRDNORM;
    }
	return rc;
}

static long
hwcam_cfgdev_vo_dqevent(
        struct v4l2_event* ev,
        int nb)
{
    long rc = 0;
    hwcam_cfgreq_t* req = NULL;
    while (true) {
        rc = v4l2_event_dequeue(&s_cfgdev.rq, ev, nb);
        if (rc != 0) {
            break;
        }
        req = (hwcam_cfgreq_t*)ev->u.data;
        s_cfgdev.cfgreq = req->intf;
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
hwcam_cfgdev_vo_do_ioctl(
        struct file* filep,
        unsigned int cmd,
        void* arg)
{
    long rc = -EINVAL;
    switch (cmd)
    {
    case VIDIOC_DQEVENT:
        rc = hwcam_cfgdev_vo_dqevent(arg,
                filep->f_flags & O_NONBLOCK);
        break;
    case VIDIOC_SUBSCRIBE_EVENT:
        rc = v4l2_event_subscribe(&s_cfgdev.rq,
                (struct v4l2_event_subscription*)arg, 8,NULL);
        break;
    case VIDIOC_UNSUBSCRIBE_EVENT:
        rc = v4l2_event_unsubscribe(&s_cfgdev.rq,
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

static long
hwcam_cfgdev_vo_ioctl(
        struct file* filep,
        unsigned int cmd,
        unsigned long arg)
{
    return video_usercopy(filep,
            cmd, arg, hwcam_cfgdev_vo_do_ioctl);
}

static int
hwcam_cfgdev_vo_close(
        struct file* filep)
{
    void* fpd = NULL;
    struct ion_client* ion = NULL;

    struct v4l2_event ev =
    {
        .type = HWCAM_V4L2_EVENT_TYPE,
        .id = HWCAM_SERVER_CRASH,
    };
    if (NULL != s_cfgdev.notify_user) {
        hwcam_user_intf_notify(s_cfgdev.notify_user,&ev);
        hwcam_user_intf_put(s_cfgdev.notify_user);
        s_cfgdev.notify_user = NULL;
    }

    swap(filep->private_data, fpd);
    swap(s_cfgdev.ion, ion);

    if (fpd) {
        v4l2_fh_del(&s_cfgdev.rq);
        v4l2_fh_exit(&s_cfgdev.rq);
    }

    if (ion) {
        ion_client_destroy(ion);
    }
    return 0;
}

static int
hwcam_cfgdev_vo_open(
        struct file* filep)
{
    if (s_cfgdev.ion) {
        HWCAM_CFG_INFO("only one server can attach to cfgdev! \n");
        return -EBUSY;
    }

    s_cfgdev.ion = hisi_ion_client_create("hwcam-cfgdev");
    if (!s_cfgdev.ion) {
        HWCAM_CFG_ERR("failed to create ion client! \n");
        return -ENOMEM;
    }

	v4l2_fh_init(&s_cfgdev.rq, s_cfgdev.vdev);
    v4l2_fh_add(&s_cfgdev.rq);
    filep->private_data = &s_cfgdev;

    HWCAM_CFG_INFO("the server(%d) attached. \n", current->pid);
	return 0;
}

static struct v4l2_file_operations
s_fops_cfgdev =
{
	.owner = THIS_MODULE,
	.open = hwcam_cfgdev_vo_open,
	.poll = hwcam_cfgdev_vo_poll,
	.unlocked_ioctl = hwcam_cfgdev_vo_ioctl,
	.release = hwcam_cfgdev_vo_close,
};

static void
hwcam_cfgdev_vo_subdev_notify(
        struct v4l2_subdev* sd,
        unsigned int notification,
        void* arg)
{
    HWCAM_CFG_INFO("TODO. \n");
}

static int
hwcam_cfgdev_vo_probe(
        struct platform_device* pdev)
{
	int rc = 0;
    struct video_device* vdev = NULL;
    struct media_device* mdev = NULL;
    struct v4l2_device* v4l2 = &s_cfgdev.v4l2;

    vdev = video_device_alloc();
    if (!vdev) {
        rc = -ENOMEM;
        goto probe_end;
    }

    mdev = kzalloc(sizeof(struct media_device), GFP_KERNEL);
    if (!mdev) {
        rc = -ENOMEM;
        goto media_alloc_fail;
    }

	strlcpy(mdev->model, HWCAM_MODEL_CFG, sizeof(mdev->model));
	mdev->dev = &(pdev->dev);
	rc = media_device_register(mdev);
	if (rc < 0) {
		goto media_register_fail;
    }

    rc = media_entity_init(&vdev->entity, 0, NULL, 0);
	if (rc < 0) {
		goto entity_init_fail;
    }

	v4l2->mdev = mdev;
	v4l2->notify = hwcam_cfgdev_vo_subdev_notify;
	rc = v4l2_device_register(&(pdev->dev), v4l2);
	if (rc < 0) {
		goto v4l2_register_fail;
    }

	vdev->v4l2_dev = v4l2;
	strlcpy(vdev->name, "hwcam-cfgdev", sizeof(vdev->name));
	vdev->entity.type = MEDIA_ENT_T_DEVNODE_V4L;
	vdev->entity.group_id = HWCAM_VNODE_GROUP_ID;
	vdev->release = video_device_release_empty;
	vdev->fops = &s_fops_cfgdev;
	vdev->minor = -1;
	vdev->vfl_type = VFL_TYPE_GRABBER;
	rc = video_register_device(vdev, VFL_TYPE_GRABBER, -1);
	if (rc < 0) {
		goto video_register_fail;
    }
	cam_debug("video dev name %s %s",vdev->dev.kobj.name,vdev->name);
	vdev->entity.name = video_device_node_name(vdev);
    vdev->lock = &s_cfgdev_lock;
	video_set_drvdata(vdev, &s_cfgdev);

    s_cfgdev.vdev = vdev;
    s_cfgdev.mdev = mdev;
    s_cfgdev.notify_user = NULL;
    INIT_LIST_HEAD(&s_cfgdev.pipelines);

    s_cfgdev.debug_root = debugfs_create_dir("hwcam", NULL);

	goto probe_end;

video_register_fail:
	v4l2_device_unregister(v4l2);

v4l2_register_fail:
	media_entity_cleanup(&vdev->entity);

entity_init_fail:
	media_device_unregister(mdev);

media_register_fail:
    kzfree(mdev);

media_alloc_fail:
    video_device_release(vdev);

probe_end:
	HWCAM_CFG_INFO("exit");
	return rc;
}

static const struct of_device_id
s_cfgdev_devtbl_match[] =
{
    {
        .compatible = "huawei,camcfgdev"
    },
}

MODULE_DEVICE_TABLE(of, s_cfgdev_devtbl_match);

static struct platform_driver
s_cfgdev_driver =
{
    .probe = hwcam_cfgdev_vo_probe,
    .driver =
    {
        .name = "huawei,camcfgdev",
        .owner = THIS_MODULE,
        .of_match_table = s_cfgdev_devtbl_match,
    },
};

static int __init
hwcam_cfgdev_vo_init(void)
{
    hwcam_v4l2_subdev_fops = v4l2_subdev_fops;
    hwcam_v4l2_subdev_fops.poll = hwcam_subdev_poll;
    return platform_driver_register(&s_cfgdev_driver);
}

static void __exit
hwcam_cfgdev_vo_exit(void)
{
    platform_driver_unregister(&s_cfgdev_driver);
}

module_init(hwcam_cfgdev_vo_init);
module_exit(hwcam_cfgdev_vo_exit);
MODULE_DESCRIPTION("Huawei V4L2 Camera");
MODULE_LICENSE("GPL v2");

