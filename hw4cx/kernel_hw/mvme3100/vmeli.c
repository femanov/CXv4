#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/vme.h>


#define VMEI_BUS_MAX	1

static const char driver_name[] = "vmeli";

static int bus[VMEI_BUS_MAX];
static unsigned int bus_num;
MODULE_PARM_DESC(bus, "Enumeration of VMEbus to which the driver is connected");
module_param_array(bus, int, &bus_num, 0000);

static int major = 0;
module_param(major, int, 0000);
MODULE_PARM_DESC(major, "Major device number (default is dynamic)");

static int debug_irq = 0;
module_param(debug_irq, int, 0644);
MODULE_PARM_DESC(debug_irq, "Enable debug output at every VME interrupt");


// static const char cdev_name[] = "bus/vme/irq/vmeli%d";
static const char cdev_name[] = "vmeli%d";

#define N_IRQL   8

enum
{
    Q_SIZE = 256, // In fact 255=256-1, "-1" because of head/tail logic
};

/*
 * Structure to handle image related parameters.
 */
struct image_desc {
//	int level; /*!!! Is it REALLY needed? */
//	int level_mask;
	int opened;
//	int ready;
	spinlock_t lock;
	wait_queue_head_t wait;

	struct device *sysfs_device;
	struct vme_dev *vme_device;

	/* VECT-BUFFERING */
	char         q_isin[256];
	unsigned int q_fifo[Q_SIZE];
	int          q_head;
	int          q_tail;
};

static struct image_desc image[N_IRQL];

static struct cdev vmeli_cdev;		/* Character device */
static struct class *vmeli_sysfs_class;	/* Sysfs class */
static struct vme_dev *vme_device;		/* Pointer to user device */
// static struct dentry *vmeli_debugfs_dir;
// static struct dentry *vmeli_debugfs_file;



static void vmeli_isr(int level, int vector, void *data)
{
	struct image_desc *img = (struct image_desc *)data;
	unsigned long flags;
	unsigned int  vect;

	if (debug_irq) {
		pr_info("vme_interrupt: level=%d, vector=%d\n", level, vector);
	}

	spin_lock_irqsave(&img->lock, flags);

#if 1
	/* Store vect in this IRQ-level's FIFO */
	vect = vector & 0xFF;
	// Isn't this vector already present?
	if (img->q_isin[vect] == 0)
	{
		if (((img->q_head + 1) & 0xFF) == img->q_tail) {
			// An overflow?! This can't happen!
			// And we can't do anything anyway...
		}
		else {
			// Store vector in a queue cell
			img->q_fifo[img->q_head] = vect;
			// Promote queue head index
			img->q_head = (img->q_head + 1) & 0xFF;
			// Mark this vector as "is in the queue"
			img->q_isin[vect] = 1;
		}
	}
#else
	if (!img->opened) {
		spin_unlock_irqrestore(&img->lock, flags);
		return;
	}
	img->level_mask |= 1<<level;
	img->ready = 1;
#endif

	spin_unlock_irqrestore(&img->lock, flags);

	wake_up_interruptible(&img->wait);
}


static ssize_t vmeli_cdev_read(struct file *file, char *buf,
                               size_t count, loff_t *ppos)
{
	unsigned int minor = MINOR(file_inode(file)->i_rdev);
	unsigned long flags;
//	int level_mask;
	ssize_t       ret;
	unsigned int  vect;

	pr_debug("vmeli read, minor %d\n", minor);

	if (buf == NULL) {
		return -EINVAL;
	}

	if (count < sizeof(vect)) {
		return -EINVAL;
	}
	
	spin_lock_irqsave(&image[minor].lock, flags);

#if 1
	if (image[minor].q_tail == image[minor].q_head)
		ret = -EWOULDBLOCK;
	else {
		// Read vector from queue
		vect = image[minor].q_fifo[image[minor].q_tail];
		// Promote queue tail index
		image[minor].q_tail = (image[minor].q_tail + 1) & 0xFF;
		// Drop this vector's "is in the queue" flag
		image[minor].q_isin[vect] = 0;
		// Store vector in callers' buffer
		if (copy_to_user(buf, &vect, sizeof(vect)))
			ret = -EFAULT;
		else
			ret = sizeof(vect);
	}
#else
	if (image[minor].ready) {
		level_mask = image[minor].level_mask;
		image[minor].level_mask = 0;
		image[minor].ready = 0;
	} else {
		level_mask = 0;
	}
#endif

	spin_unlock_irqrestore(&image[minor].lock, flags);

//	if (copy_to_user(buf, &level_mask, sizeof(level_mask))) {
//		return -EFAULT;
//	}

	return ret;
}

// static ssize_t vmeli_cdev_write(struct file *file, const char *buf, 
// 			 size_t count, loff_t *ppos)
// {
//     return 0;
// }

static int vmeli_cdev_open(struct inode *inode, struct file *file)
{
	unsigned int minor = MINOR(file_inode(file)->i_rdev);
	int err;
	int irq_vector, irq_level;
	struct vme_dev *vdev;

	pr_debug("vmeli open, minor %d\n", minor);

	if ((minor <= 0) || (minor >= N_IRQL)) {
		err = -EBADF;
		goto err_params;
	}

	if (image[minor].opened) {
		err = -EBUSY;
		goto err_irq_busy;
	}

	irq_level = minor;
	vdev = image[minor].vme_device;
	
	// /* Check for VME interrupt is free */
	// mutex_lock(&bridge->irq_mtx);
	// for (irq_vector=0; irq_level<=255; irq_level++) {
	// 	if (vdev->bridge->irq[irq_level - 1].callback[irq_vector].func) {
	// 		mutex_unlock(&bridge->irq_mtx);
	// 		err = -EBUSY;
	// 		goto err_irq_busy;
	// 	}
	// }
	// mutex_unlock(&bridge->irq_mtx);

	/* Setup VME interrupts */
	for (irq_vector=0; irq_vector<=255; irq_vector++) {
		err = vme_irq_request(vdev, irq_level, irq_vector, vmeli_isr, &image[minor]);
		if (err) {
			dev_err(&vdev->dev, "Requested IRQ: level=%d, vector=%d FAILED\n", irq_level, irq_vector);
			goto err_free_irq;
		}
		dev_dbg(&vdev->dev, "Requested IRQ: level=%d, vector=%d\n", irq_level, irq_vector);
	}

	image[minor].opened++;
	init_waitqueue_head(&image[minor].wait);
	// MOD_INC_USE_COUNT;

	return 0;

err_free_irq:
	while (--irq_vector >= 0) {
		vme_irq_free(vdev, irq_level, irq_vector);
	}

err_irq_busy:
err_params:
	return err;
}

static int vmeli_cdev_release(struct inode *inode, struct file *file)
{
	unsigned int minor = MINOR(file_inode(file)->i_rdev);
	int err;
	int irq_vector, irq_level;
	struct vme_dev *vdev;

	pr_debug("vmeli release, minor %d\n", minor);

	if ((minor <= 0) || (minor >= N_IRQL)) {
		err = -EBADF;
		goto err_params;
	}
	
	irq_level = minor;
	vdev = image[minor].vme_device;

	for (irq_vector=0; irq_vector<=255; irq_vector++) {
		vme_irq_free(vdev, irq_level, irq_vector);
	}

	// MOD_DEC_USE_COUNT;
	image[minor].opened--;

	return 0;

err_params:
	return err;
}


static unsigned int vmeli_cdev_poll(struct file *file, poll_table *wait)
{
	unsigned int minor = MINOR(file_inode(file)->i_rdev);
	unsigned long flags;
	unsigned int  ret;

	// pr_debug("vmeli poll, minor %d\n", minor);

	poll_wait(file, &image[minor].wait, wait);

#if 1
	spin_lock_irqsave(&image[minor].lock, flags);
	ret = (image[minor].q_tail == image[minor].q_head)? 0 : POLLIN;
	spin_unlock_irqrestore(&image[minor].lock, flags);
	return ret;
#else
	// pr_debug("poll wait completed, vector %x\n", image[minor].vector);

	if (image[minor].ready) {
		return POLLIN;
	} else {
		return 0;
	}
#endif
}

static struct file_operations vmeli_fops = {
	.read		= vmeli_cdev_read,
	// .write		= vmeli_cdev_write,
	.open		= vmeli_cdev_open,
	.release	= vmeli_cdev_release,
	.poll		= vmeli_cdev_poll,
};


static int vmeli_match(struct vme_dev *vdev)
{
	int i;

	int cur_bus = vme_bus_num(vdev);
	int cur_slot = vme_slot_num(vdev);

// // what is cur_slot?
	// pr_debug("%s: vmeli_match: idx=%d, bus=%d, slot=%d\n",
	// 		 driver_name, vdev->num, cur_bus, cur_slot);

	for (i = 0; i < bus_num; i++)
		if ((cur_bus == bus[i]) && (cur_slot == vdev->num))
			return 1;

	return 0;
}


static int vmeli_probe(struct vme_dev *vdev)
{
	int err;
	unsigned int irq_level;
	unsigned int minor;
	dev_t devid;

	int   vect;

	int idx = vdev->num;

	dev_dbg(&vdev->dev, "probing idx=%d", idx);

	/* Save pointer to the bridge device */
	if (vme_device) {
		dev_err(&vdev->dev, "Driver can only be loaded for 1 device\n");
		err = -EINVAL;
		goto err_dev;
	}
	vme_device = vdev;

	/* Initialise descriptors */
	for (irq_level = 0; irq_level < N_IRQL; irq_level++) {
		minor = irq_level;

#if 1
		image[minor].opened = 0;
		image[minor].vme_device = vme_device;
		image[minor].q_head = 0;
		image[minor].q_tail = 0;
		for (vect = 0;  vect < 256;  vect++) image[minor].q_isin[vect] = 0;
#else
		image[minor].level = irq_level;
		image[minor].level_mask = 0;
		image[minor].opened = 0;
		image[minor].ready = 0;
		image[minor].vme_device = vme_device;
#endif

		spin_lock_init(&image[minor].lock);
		init_waitqueue_head(&image[minor].wait);

		image[minor].sysfs_device = NULL;	/* Sysfs device */
	}

	/* Assign major and minor numbers for the driver */
	if (major) {
		devid = MKDEV(major, 0);
		err = register_chrdev_region(devid, N_IRQL, driver_name);
	} else {
		err = alloc_chrdev_region(&devid, 0, N_IRQL, driver_name);
		major = MAJOR(devid);
	}
	if (err) {
		dev_err(&vdev->dev, "Error getting Major Number for driver.\n");
		goto err_alloc_chrdev;
	}

	/* Register the driver as a char device */
	cdev_init(&vmeli_cdev, &vmeli_fops);
	err = cdev_add(&vmeli_cdev, MKDEV(major, 0), N_IRQL);
	if (err) {
		dev_err(&vdev->dev, "Error while adding cdev.\n");
		goto err_cdev_add;
	}

	/* Create sysfs entries - on udev systems this creates the dev files */
	vmeli_sysfs_class = class_create(THIS_MODULE, driver_name);
	if (IS_ERR(vmeli_sysfs_class)) {
		dev_err(&vdev->dev, "Error creating sysfs class.\n");
		err = PTR_ERR(vmeli_sysfs_class);
		goto err_sysfs_class;
	}

	/* Add sysfs entries */
	for (irq_level = 1; irq_level < N_IRQL; irq_level++) {
		minor = irq_level;

		image[minor].sysfs_device = 
			device_create(vmeli_sysfs_class, NULL,
				      MKDEV(major, minor), NULL,
				      cdev_name, irq_vector);
		if (IS_ERR(image[minor].sysfs_device)) {
			dev_info(&vdev->dev, "Error creating sysfs device\n");
			err = PTR_ERR(image[minor].sysfs_device);
			goto err_sysfs_create;
		}
	}

	//TODO: Maby debugfs?
	// /* Register debugfs */
	// vme_user_debugfs_dir = debugfs_create_dir(driver_name, NULL);
	// if (!vme_user_debugfs_dir) {
	// 	dev_err(&vdev->dev, "Could not create debugfs directory: %s\n", driver_name);
	// } else {
	// 	vme_user_debugfs_file = debugfs_create_file("wnds_desc", 0644,
	// 			vme_user_debugfs_dir, NULL, &vme_user_debugfs_ops);
	// 	if (!vme_user_debugfs_file) {
	// 		dev_err(&vdev->dev, "Could not create debugfs entry: wnds_desc\n");
	// 	}
	// }

	return 0;


err_sysfs_create:
	while (--irq_level > 0) {
		minor = irq_level;
		device_destroy(vmeli_sysfs_class, MKDEV(major, minor));
	}

	class_destroy(vmeli_sysfs_class);	
err_sysfs_class:

	cdev_del(&vmeli_cdev);
err_cdev_add:

	unregister_chrdev_region(MKDEV(major, 0), N_IRQL);
err_alloc_chrdev:

err_dev:
	return err;
}


// TODO: How I will unload driver if clients connected and free IRQs?

static int vmeli_remove(struct vme_dev *dev)
{
	int irq_vector;
	
	dev_info(&dev->dev, "removing");

	/* Remove debugfs entries*/
	// debugfs_remove_recursive(vmeli_debugfs_dir);

	/* Remove sysfs entries */
	for (irq_level = 1; irq_level < N_IRQL; irq_level++) {
		unsigned int minor = irq_level;
		device_destroy(vmeli_sysfs_class, MKDEV(major, minor));
	}
	class_destroy(vmeli_sysfs_class);

	/* Unregister device driver */
	cdev_del(&vmeli_cdev);

	/* Unregiser the major and minor device numbers */
	unregister_chrdev_region(MKDEV(major, 0), N_IRQL);

	return 0;
}

static struct vme_driver vmeli_driver = {
	.name = driver_name,
	.match = vmeli_match,
	.probe = vmeli_probe,
	.remove = vmeli_remove,
};

static int __init vmeli_init(void)
{
	int retval = 0;

	pr_info("VME Interrupts User Space Access Driver with 1-device-per-IRQ-line\n");

	if (bus_num == 0) {
		pr_err("No cards, skipping registration\n");
		retval = -ENODEV;
		goto err_nocard;
	}

	/* Let's start by supporting one bus, we can support more than one
	 * in future revisions if that ever becomes necessary.
	 */
	if (bus_num > VMEI_BUS_MAX) {
		pr_err("Driver only able to handle %d buses\n",
		       VMEI_BUS_MAX);
		bus_num = VMEI_BUS_MAX;
	}

	/*
	 * Here we just register the maximum number of devices we can and
	 * leave vme_user_match() to allow only 1 to go through to probe().
	 * This way, if we later want to allow multiple user access devices,
	 * we just change the code in vme_user_match().
	 */
	retval = vme_register_driver(&vmeli_driver, VME_MAX_SLOTS);
	if (retval)
		goto err_reg;

	return retval;

err_reg:
err_nocard:
	return retval;
}

static void __exit vmeli_exit(void)
{
	vme_unregister_driver(&vmeli_driver);
}

MODULE_AUTHOR("Evgeniy Kotov <e.s.kotov@inp.nsk.su>")
MODULE_AUTHOR("Dmitry Bolkhovityanov <d.yu.bolkhovityanov@inp.nsk.su>"); // vmeli version
MODULE_DESCRIPTION("VME IRQs to user space translator with 1-device-per-IRQ-line");
MODULE_LICENSE("GPL v2");

module_init(vmeli_init);
module_exit(vmeli_exit);
