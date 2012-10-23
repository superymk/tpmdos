/*
 * Copyright (C) 2005, 2006 IBM Corporation
 *
 * Authors:
 * Leendert van Doorn <leendert@watson.ibm.com>
 * Kylene Hall <kjhall@us.ibm.com>
 *
 * Maintained by: <tpmdd-devel@lists.sourceforge.net>
 *
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * This device driver implements the TPM interface as defined in
 * the TCG TPM Interface Spec version 1.2, revision 1.0.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/pnp.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/acpi.h>
#include <linux/freezer.h>
#include "tpm.h"

// [Superymk] Begin
#include <linux/timer.h>
// [Superymk] End

/*********************************
 * Begin modification by imamotts
 *********************************/
 
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/debugfs.h>

#include <linux/mm.h>  /* mmap related stuff */
#define TPMDOS_TIS_COMM_NAME "tpmdos_tis"

struct dentry  *file1;

struct mmap_info {
	char *data;	/* the data */
	int reference;       /* how many times it is mmapped */  	
};


/* keep track of how many times it is mmapped */

void mmap_open(struct vm_area_struct *vma)
{
	struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
	info->reference++;
}

void mmap_close(struct vm_area_struct *vma)
{
	struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
	info->reference--;
}

/* nopage is called the first time a memory area is accessed which is not in memory,
 * it does the actual mapping between kernel and user space memory
 */
int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page;
	struct mmap_info *info;

	/* the data is in vma->vm_private_data */
	info = (struct mmap_info *)vma->vm_private_data;

	/* get the page */
	page = virt_to_page(info->data);
	vmf->page = page;
	
	/* increment the reference count of this page */
	get_page(page);

	return 0;
}

struct vm_operations_struct mmap_vm_ops = {
	.open =     mmap_open,
	.close =    mmap_close,
	.fault =    mmap_fault,
};

static int my_mmap(struct file *filp, struct vm_area_struct *vma)
{
	vma->vm_ops = &mmap_vm_ops;
	vma->vm_flags |= VM_RESERVED;
	/* assign the file private data to the vm private data */
	vma->vm_private_data = filp->private_data;
	mmap_open(vma);
	return 0;
}

static int my_close(struct inode *inode, struct file *filp)
{
	struct mmap_info *info = filp->private_data;
	/* obtain new memory */
	free_page((unsigned long)info->data);
    	kfree(info);
	filp->private_data = NULL;
	return 0;
}

static struct mmap_info *info = NULL;
static int my_open(struct inode *inode, struct file *filp)
{
	//if(!info)
	{
		info = kmalloc(sizeof(struct mmap_info), GFP_KERNEL);
		
		/* obtain new memory */
    	info->data = (char *)get_zeroed_page(GFP_KERNEL);
		
		/* assign this info struct to the file */
		filp->private_data = info;
	}
	
	return 0;
}

typedef struct
{
	u64 timer_result;
	unsigned long max_jiffies;
} TPM_DRIVER_NVWRITE_TIMER, *PTPM_DRIVER_NVWRITE_TIMER;

static void mm_write(PTPM_DRIVER_NVWRITE_TIMER result)
{
	memcpy(info->data, result, sizeof(TPM_DRIVER_NVWRITE_TIMER));
}

static const struct file_operations my_fops = {
	.open = my_open,
	.release = my_close,
	.mmap = my_mmap,
};

/*********************************
 * End modification by imamotts
 *********************************/


enum tis_access {
	TPM_ACCESS_VALID = 0x80,
	TPM_ACCESS_ACTIVE_LOCALITY = 0x20,
	TPM_ACCESS_REQUEST_PENDING = 0x04,
	TPM_ACCESS_REQUEST_USE = 0x02,
};

enum tis_status {
	TPM_STS_VALID = 0x80,
	TPM_STS_COMMAND_READY = 0x40,
	TPM_STS_GO = 0x20,
	TPM_STS_DATA_AVAIL = 0x10,
	TPM_STS_DATA_EXPECT = 0x08,
};

enum tis_int_flags {
	TPM_GLOBAL_INT_ENABLE = 0x80000000,
	TPM_INTF_BURST_COUNT_STATIC = 0x100,
	TPM_INTF_CMD_READY_INT = 0x080,
	TPM_INTF_INT_EDGE_FALLING = 0x040,
	TPM_INTF_INT_EDGE_RISING = 0x020,
	TPM_INTF_INT_LEVEL_LOW = 0x010,
	TPM_INTF_INT_LEVEL_HIGH = 0x008,
	TPM_INTF_LOCALITY_CHANGE_INT = 0x004,
	TPM_INTF_STS_VALID_INT = 0x002,
	TPM_INTF_DATA_AVAIL_INT = 0x001,
};

enum tis_defaults {
	TIS_MEM_BASE = 0xFED40000,
	TIS_MEM_LEN = 0x5000,
	TIS_SHORT_TIMEOUT = 750,	/* ms */
	TIS_LONG_TIMEOUT = 2000,	/* 2 sec */
};

#define	TPM_ACCESS(l)			(0x0000 | ((l) << 12))
#define	TPM_INT_ENABLE(l)		(0x0008 | ((l) << 12))
#define	TPM_INT_VECTOR(l)		(0x000C | ((l) << 12))
#define	TPM_INT_STATUS(l)		(0x0010 | ((l) << 12))
#define	TPM_INTF_CAPS(l)		(0x0014 | ((l) << 12))
#define	TPM_STS(l)			(0x0018 | ((l) << 12))
#define	TPM_DATA_FIFO(l)		(0x0024 | ((l) << 12))

#define	TPM_DID_VID(l)			(0x0F00 | ((l) << 12))
#define	TPM_RID(l)			(0x0F04 | ((l) << 12))

static LIST_HEAD(tis_chips);
static DEFINE_MUTEX(tis_lock);

#if defined(CONFIG_PNP) && defined(CONFIG_ACPI)
static int is_itpm(struct pnp_dev *dev)
{
	struct acpi_device *acpi = pnp_acpi_device(dev);
	struct acpi_hardware_id *id;

	list_for_each_entry(id, &acpi->pnp.ids, list) {
		if (!strcmp("INTC0102", id->id))
			return 1;
	}

	return 0;
}
#else
static inline int is_itpm(struct pnp_dev *dev)
{
	return 0;
}
#endif

static int check_locality(struct tpm_chip *chip, int l)
{
	if ((ioread8(chip->vendor.iobase + TPM_ACCESS(l)) &
	     (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID)) ==
	    (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID))
		return chip->vendor.locality = l;

	return -1;
}

static void release_locality(struct tpm_chip *chip, int l, int force)
{
	if (force || (ioread8(chip->vendor.iobase + TPM_ACCESS(l)) &
		      (TPM_ACCESS_REQUEST_PENDING | TPM_ACCESS_VALID)) ==
	    (TPM_ACCESS_REQUEST_PENDING | TPM_ACCESS_VALID))
		iowrite8(TPM_ACCESS_ACTIVE_LOCALITY,
			 chip->vendor.iobase + TPM_ACCESS(l));
}

static int request_locality(struct tpm_chip *chip, int l)
{
	unsigned long stop, timeout;
	long rc;

	if (check_locality(chip, l) >= 0)
		return l;

	iowrite8(TPM_ACCESS_REQUEST_USE,
		 chip->vendor.iobase + TPM_ACCESS(l));

	stop = jiffies + chip->vendor.timeout_a;

	if (chip->vendor.irq) {
again:
		timeout = stop - jiffies;
		if ((long)timeout <= 0)
			return -1;
		rc = wait_event_interruptible_timeout(chip->vendor.int_queue,
						      (check_locality
						       (chip, l) >= 0),
						      timeout);
		if (rc > 0)
			return l;
		if (rc == -ERESTARTSYS && freezing(current)) {
			clear_thread_flag(TIF_SIGPENDING);
			goto again;
		}
	} else {
		/* wait for burstcount */
		do {
			if (check_locality(chip, l) >= 0)
				return l;
			msleep(TPM_TIMEOUT);
		}
		while (time_before(jiffies, stop));
	}
	return -1;
}

static u8 tpm_tis_status(struct tpm_chip *chip)
{
	return ioread8(chip->vendor.iobase +
		       TPM_STS(chip->vendor.locality));
}

static void tpm_tis_ready(struct tpm_chip *chip)
{
	/* this causes the current command to be aborted */
	iowrite8(TPM_STS_COMMAND_READY,
		 chip->vendor.iobase + TPM_STS(chip->vendor.locality));
}

static int get_burstcount(struct tpm_chip *chip)
{
	unsigned long stop;
	int burstcnt;

	/* wait for burstcount */
	/* which timeout value, spec has 2 answers (c & d) */
	stop = jiffies + chip->vendor.timeout_d;
	do {
		burstcnt = ioread8(chip->vendor.iobase +
				   TPM_STS(chip->vendor.locality) + 1);
		burstcnt += ioread8(chip->vendor.iobase +
				    TPM_STS(chip->vendor.locality) +
				    2) << 8;
		if (burstcnt)
			return burstcnt;
		msleep(TPM_TIMEOUT);
	} while (time_before(jiffies, stop));
	return -EBUSY;
}

static int recv_data(struct tpm_chip *chip, u8 *buf, size_t count)
{
	int size = 0, burstcnt;
	while (size < count &&
	       wait_for_tpm_stat(chip,
				 TPM_STS_DATA_AVAIL | TPM_STS_VALID,
				 chip->vendor.timeout_c,
				 &chip->vendor.read_queue)
	       == 0) {
		burstcnt = get_burstcount(chip);
		for (; burstcnt > 0 && size < count; burstcnt--)
			buf[size++] = ioread8(chip->vendor.iobase +
					      TPM_DATA_FIFO(chip->vendor.
							    locality));
	}
	return size;
}

static int tpm_tis_recv(struct tpm_chip *chip, u8 *buf, size_t count)
{
	int size = 0;
	int expected, status;

	if (count < TPM_HEADER_SIZE) {
		size = -EIO;
		goto out;
	}

	/* read first 10 bytes, including tag, paramsize, and result */
	if ((size =
	     recv_data(chip, buf, TPM_HEADER_SIZE)) < TPM_HEADER_SIZE) {
		dev_err(chip->dev, "Unable to read header\n");
		goto out;
	}

	expected = be32_to_cpu(*(__be32 *) (buf + 2));
	if (expected > count) {
		size = -EIO;
		goto out;
	}

	if ((size +=
	     recv_data(chip, &buf[TPM_HEADER_SIZE],
		       expected - TPM_HEADER_SIZE)) < expected) {
		dev_err(chip->dev, "Unable to read remainder of result\n");
		size = -ETIME;
		goto out;
	}

	wait_for_tpm_stat(chip, TPM_STS_VALID, chip->vendor.timeout_c,
			  &chip->vendor.int_queue);
	status = tpm_tis_status(chip);
	if (status & TPM_STS_DATA_AVAIL) {	/* retry? */
		dev_err(chip->dev, "Error left over data\n");
		size = -EIO;
		goto out;
	}

out:
	tpm_tis_ready(chip);
	release_locality(chip, chip->vendor.locality, 0);
	return size;
}

static bool itpm;
module_param(itpm, bool, 0444);
MODULE_PARM_DESC(itpm, "Force iTPM workarounds (found on some Lenovo laptops)");

/*
 * If interrupts are used (signaled by an irq set in the vendor structure)
 * tpm.c can skip polling for the data to be available as the interrupt is
 * waited for here
 */
static int tpm_tis_send_data(struct tpm_chip *chip, u8 *buf, size_t len)
{
	int rc, status, burstcnt;
	size_t count = 0;

	if (request_locality(chip, 0) < 0)
		return -EBUSY;

	status = tpm_tis_status(chip);
	if ((status & TPM_STS_COMMAND_READY) == 0) {
		tpm_tis_ready(chip);
		if (wait_for_tpm_stat
		    (chip, TPM_STS_COMMAND_READY, chip->vendor.timeout_b,
		     &chip->vendor.int_queue) < 0) {
			rc = -ETIME;
			goto out_err;
		}
	}

	while (count < len - 1) {
		burstcnt = get_burstcount(chip);
		for (; burstcnt > 0 && count < len - 1; burstcnt--) {
			iowrite8(buf[count], chip->vendor.iobase +
				 TPM_DATA_FIFO(chip->vendor.locality));
			count++;
		}

		wait_for_tpm_stat(chip, TPM_STS_VALID, chip->vendor.timeout_c,
				  &chip->vendor.int_queue);
		status = tpm_tis_status(chip);
		if (!itpm && (status & TPM_STS_DATA_EXPECT) == 0) {
			rc = -EIO;
			goto out_err;
		}
	}

	/* write last byte */
	iowrite8(buf[count],
		 chip->vendor.iobase + TPM_DATA_FIFO(chip->vendor.locality));
	wait_for_tpm_stat(chip, TPM_STS_VALID, chip->vendor.timeout_c,
			  &chip->vendor.int_queue);
	status = tpm_tis_status(chip);
	if ((status & TPM_STS_DATA_EXPECT) != 0) {
		rc = -EIO;
		goto out_err;
	}

	return 0;

out_err:
	tpm_tis_ready(chip);
	release_locality(chip, chip->vendor.locality, 0);
	return rc;
}

// [Superymk] Begin
static int memlocate(u8* mem, unsigned int target, size_t len)
{
	size_t i = 0;
	for(i = 0; i < len; i++)
 	{
 		unsigned int value = *(unsigned int*)((u8*)(mem+i));
 		
 		if(value == target)
 			return i;
 	}
 	
 	return 0;
}

static void QueryPerformanceCounter(u64 *performance_count)
{
    struct timeval time;

    /* Grab the current time. */
    do_gettimeofday(&time);
    *performance_count = time.tv_usec + /* Microseconds. */
                         time.tv_sec * USEC_PER_SEC; /* Seconds. */
}
// [Superymk] End

/*
 * If interrupts are used (signaled by an irq set in the vendor structure)
 * tpm.c can skip polling for the data to be available as the interrupt is
 * waited for here
 */
static int tpm_tis_send(struct tpm_chip *chip, u8 *buf, size_t len)
{
	int rc;
	u32 ordinal;

	// [Superymk] Begin 
	TPM_DRIVER_NVWRITE_TIMER result;
	unsigned long max_jiffies = 0;
	u64 start_timer = 0, end_timer = 0;
	int res = memlocate(buf, 0xbabecafe, len);
	
	//printk(KERN_INFO "[TPMDoS Driver] MAGIC_HEADER found:%d, size:%u\n", result, len);
	
	QueryPerformanceCounter(&start_timer);
	// [Superymk] End
	
	rc = tpm_tis_send_data(chip, buf, len);
	if (rc < 0)
		return rc;

	/* go and do it */
	iowrite8(TPM_STS_GO,
		 chip->vendor.iobase + TPM_STS(chip->vendor.locality));

	if (chip->vendor.irq) {
		ordinal = be32_to_cpu(*((__be32 *) (buf + 6)));
		//[Superymk] Begin
		max_jiffies = tpm_calc_ordinal_duration(chip, ordinal);
		//[Superymk] End
		if (wait_for_tpm_stat
		    (chip, TPM_STS_DATA_AVAIL | TPM_STS_VALID,
		     max_jiffies,
		     &chip->vendor.read_queue) < 0) {
			rc = -ETIME;
			goto out_err;
		}
	}
	
	// [Superymk] Begin
	QueryPerformanceCounter(&end_timer);
	result.timer_result = end_timer - start_timer;
	result.max_jiffies = max_jiffies;
	
	if(res)
		mm_write(&result);
	// [Superymk] End
	
	return len;
out_err:
	tpm_tis_ready(chip);
	release_locality(chip, chip->vendor.locality, 0);
	
	// [Superymk] Begin
	QueryPerformanceCounter(&end_timer);
	result.timer_result = end_timer - start_timer;
	result.max_jiffies = max_jiffies;
	
	if(res)
		mm_write(&result);
	// [Superymk] End
	
	return rc;
}

/*
 * Early probing for iTPM with STS_DATA_EXPECT flaw.
 * Try sending command without itpm flag set and if that
 * fails, repeat with itpm flag set.
 */
static int probe_itpm(struct tpm_chip *chip)
{
	int rc = 0;
	u8 cmd_getticks[] = {
		0x00, 0xc1, 0x00, 0x00, 0x00, 0x0a,
		0x00, 0x00, 0x00, 0xf1
	};
	size_t len = sizeof(cmd_getticks);
	bool rem_itpm = itpm;
	u16 vendor = ioread16(chip->vendor.iobase + TPM_DID_VID(0));

	/* probe only iTPMS */
	if (vendor != TPM_VID_INTEL)
		return 0;

	itpm = 0;

	rc = tpm_tis_send_data(chip, cmd_getticks, len);
	if (rc == 0)
		goto out;

	tpm_tis_ready(chip);
	release_locality(chip, chip->vendor.locality, 0);

	itpm = 1;

	rc = tpm_tis_send_data(chip, cmd_getticks, len);
	if (rc == 0) {
		dev_info(chip->dev, "Detected an iTPM.\n");
		rc = 1;
	} else
		rc = -EFAULT;

out:
	itpm = rem_itpm;
	tpm_tis_ready(chip);
	release_locality(chip, chip->vendor.locality, 0);

	return rc;
}

static const struct file_operations tis_ops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.open = tpm_open,
	.read = tpm_read,
	.write = tpm_write,
	.release = tpm_release,
};

static DEVICE_ATTR(pubek, S_IRUGO, tpm_show_pubek, NULL);
static DEVICE_ATTR(pcrs, S_IRUGO, tpm_show_pcrs, NULL);
static DEVICE_ATTR(enabled, S_IRUGO, tpm_show_enabled, NULL);
static DEVICE_ATTR(active, S_IRUGO, tpm_show_active, NULL);
static DEVICE_ATTR(owned, S_IRUGO, tpm_show_owned, NULL);
static DEVICE_ATTR(temp_deactivated, S_IRUGO, tpm_show_temp_deactivated,
		   NULL);
static DEVICE_ATTR(caps, S_IRUGO, tpm_show_caps_1_2, NULL);
static DEVICE_ATTR(cancel, S_IWUSR | S_IWGRP, NULL, tpm_store_cancel);
static DEVICE_ATTR(durations, S_IRUGO, tpm_show_durations, NULL);
static DEVICE_ATTR(timeouts, S_IRUGO, tpm_show_timeouts, NULL);

static struct attribute *tis_attrs[] = {
	&dev_attr_pubek.attr,
	&dev_attr_pcrs.attr,
	&dev_attr_enabled.attr,
	&dev_attr_active.attr,
	&dev_attr_owned.attr,
	&dev_attr_temp_deactivated.attr,
	&dev_attr_caps.attr,
	&dev_attr_cancel.attr,
	&dev_attr_durations.attr,
	&dev_attr_timeouts.attr, NULL,
};

static struct attribute_group tis_attr_grp = {
	.attrs = tis_attrs
};

static struct tpm_vendor_specific tpm_tis = {
	.status = tpm_tis_status,
	.recv = tpm_tis_recv,
	.send = tpm_tis_send,
	.cancel = tpm_tis_ready,
	.req_complete_mask = TPM_STS_DATA_AVAIL | TPM_STS_VALID,
	.req_complete_val = TPM_STS_DATA_AVAIL | TPM_STS_VALID,
	.req_canceled = TPM_STS_COMMAND_READY,
	.attr_group = &tis_attr_grp,
	.miscdev = {
		    .fops = &tis_ops,},
};

static irqreturn_t tis_int_probe(int irq, void *dev_id)
{
	struct tpm_chip *chip = dev_id;
	u32 interrupt;

	interrupt = ioread32(chip->vendor.iobase +
			     TPM_INT_STATUS(chip->vendor.locality));

	if (interrupt == 0)
		return IRQ_NONE;

	chip->vendor.probed_irq = irq;

	/* Clear interrupts handled with TPM_EOI */
	iowrite32(interrupt,
		  chip->vendor.iobase +
		  TPM_INT_STATUS(chip->vendor.locality));
	return IRQ_HANDLED;
}

static irqreturn_t tis_int_handler(int dummy, void *dev_id)
{
	struct tpm_chip *chip = dev_id;
	u32 interrupt;
	int i;

	interrupt = ioread32(chip->vendor.iobase +
			     TPM_INT_STATUS(chip->vendor.locality));

	if (interrupt == 0)
		return IRQ_NONE;

	if (interrupt & TPM_INTF_DATA_AVAIL_INT)
		wake_up_interruptible(&chip->vendor.read_queue);
	if (interrupt & TPM_INTF_LOCALITY_CHANGE_INT)
		for (i = 0; i < 5; i++)
			if (check_locality(chip, i) >= 0)
				break;
	if (interrupt &
	    (TPM_INTF_LOCALITY_CHANGE_INT | TPM_INTF_STS_VALID_INT |
	     TPM_INTF_CMD_READY_INT))
		wake_up_interruptible(&chip->vendor.int_queue);

	/* Clear interrupts handled with TPM_EOI */
	iowrite32(interrupt,
		  chip->vendor.iobase +
		  TPM_INT_STATUS(chip->vendor.locality));
	ioread32(chip->vendor.iobase + TPM_INT_STATUS(chip->vendor.locality));
	return IRQ_HANDLED;
}

static bool interrupts = 1;
module_param(interrupts, bool, 0444);
MODULE_PARM_DESC(interrupts, "Enable interrupts");

static int tpm_tis_init(struct device *dev, resource_size_t start,
			resource_size_t len, unsigned int irq)
{
	u32 vendor, intfcaps, intmask;
	int rc, i, irq_s, irq_e, probe;
	struct tpm_chip *chip;

	if (!(chip = tpm_register_hardware(dev, &tpm_tis)))
		return -ENODEV;

	chip->vendor.iobase = ioremap(start, len);
	if (!chip->vendor.iobase) {
		rc = -EIO;
		goto out_err;
	}

	/* Default timeouts */
	chip->vendor.timeout_a = msecs_to_jiffies(TIS_SHORT_TIMEOUT);
	chip->vendor.timeout_b = msecs_to_jiffies(TIS_LONG_TIMEOUT);
	chip->vendor.timeout_c = msecs_to_jiffies(TIS_SHORT_TIMEOUT);
	chip->vendor.timeout_d = msecs_to_jiffies(TIS_SHORT_TIMEOUT);

	if (request_locality(chip, 0) != 0) {
		rc = -ENODEV;
		goto out_err;
	}

	vendor = ioread32(chip->vendor.iobase + TPM_DID_VID(0));

	dev_info(dev,
		 "1.2 TPM (device-id 0x%X, rev-id %d)\n",
		 vendor >> 16, ioread8(chip->vendor.iobase + TPM_RID(0)));

	if (!itpm) {
		probe = probe_itpm(chip);
		if (probe < 0) {
			rc = -ENODEV;
			goto out_err;
		}
		itpm = (probe == 0) ? 0 : 1;
	}

	if (itpm)
		dev_info(dev, "Intel iTPM workaround enabled\n");


	/* Figure out the capabilities */
	intfcaps =
	    ioread32(chip->vendor.iobase +
		     TPM_INTF_CAPS(chip->vendor.locality));
	dev_dbg(dev, "TPM interface capabilities (0x%x):\n",
		intfcaps);
	if (intfcaps & TPM_INTF_BURST_COUNT_STATIC)
		dev_dbg(dev, "\tBurst Count Static\n");
	if (intfcaps & TPM_INTF_CMD_READY_INT)
		dev_dbg(dev, "\tCommand Ready Int Support\n");
	if (intfcaps & TPM_INTF_INT_EDGE_FALLING)
		dev_dbg(dev, "\tInterrupt Edge Falling\n");
	if (intfcaps & TPM_INTF_INT_EDGE_RISING)
		dev_dbg(dev, "\tInterrupt Edge Rising\n");
	if (intfcaps & TPM_INTF_INT_LEVEL_LOW)
		dev_dbg(dev, "\tInterrupt Level Low\n");
	if (intfcaps & TPM_INTF_INT_LEVEL_HIGH)
		dev_dbg(dev, "\tInterrupt Level High\n");
	if (intfcaps & TPM_INTF_LOCALITY_CHANGE_INT)
		dev_dbg(dev, "\tLocality Change Int Support\n");
	if (intfcaps & TPM_INTF_STS_VALID_INT)
		dev_dbg(dev, "\tSts Valid Int Support\n");
	if (intfcaps & TPM_INTF_DATA_AVAIL_INT)
		dev_dbg(dev, "\tData Avail Int Support\n");

	/* get the timeouts before testing for irqs */
	if (tpm_get_timeouts(chip)) {
		dev_err(dev, "Could not get TPM timeouts and durations\n");
		rc = -ENODEV;
		goto out_err;
	}

	if (tpm_do_selftest(chip)) {
		dev_err(dev, "TPM self test failed\n");
		rc = -ENODEV;
		goto out_err;
	}

	/* INTERRUPT Setup */
	init_waitqueue_head(&chip->vendor.read_queue);
	init_waitqueue_head(&chip->vendor.int_queue);

	intmask =
	    ioread32(chip->vendor.iobase +
		     TPM_INT_ENABLE(chip->vendor.locality));

	intmask |= TPM_INTF_CMD_READY_INT
	    | TPM_INTF_LOCALITY_CHANGE_INT | TPM_INTF_DATA_AVAIL_INT
	    | TPM_INTF_STS_VALID_INT;

	iowrite32(intmask,
		  chip->vendor.iobase +
		  TPM_INT_ENABLE(chip->vendor.locality));
	if (interrupts)
		chip->vendor.irq = irq;
	if (interrupts && !chip->vendor.irq) {
		irq_s =
		    ioread8(chip->vendor.iobase +
			    TPM_INT_VECTOR(chip->vendor.locality));
		if (irq_s) {
			irq_e = irq_s;
		} else {
			irq_s = 3;
			irq_e = 15;
		}

		for (i = irq_s; i <= irq_e && chip->vendor.irq == 0; i++) {
			iowrite8(i, chip->vendor.iobase +
				 TPM_INT_VECTOR(chip->vendor.locality));
			if (request_irq
			    (i, tis_int_probe, IRQF_SHARED,
			     chip->vendor.miscdev.name, chip) != 0) {
				dev_info(chip->dev,
					 "Unable to request irq: %d for probe\n",
					 i);
				continue;
			}

			/* Clear all existing */
			iowrite32(ioread32
				  (chip->vendor.iobase +
				   TPM_INT_STATUS(chip->vendor.locality)),
				  chip->vendor.iobase +
				  TPM_INT_STATUS(chip->vendor.locality));

			/* Turn on */
			iowrite32(intmask | TPM_GLOBAL_INT_ENABLE,
				  chip->vendor.iobase +
				  TPM_INT_ENABLE(chip->vendor.locality));

			chip->vendor.probed_irq = 0;

			/* Generate Interrupts */
			tpm_gen_interrupt(chip);

			chip->vendor.irq = chip->vendor.probed_irq;

			/* free_irq will call into tis_int_probe;
			   clear all irqs we haven't seen while doing
			   tpm_gen_interrupt */
			iowrite32(ioread32
				  (chip->vendor.iobase +
				   TPM_INT_STATUS(chip->vendor.locality)),
				  chip->vendor.iobase +
				  TPM_INT_STATUS(chip->vendor.locality));

			/* Turn off */
			iowrite32(intmask,
				  chip->vendor.iobase +
				  TPM_INT_ENABLE(chip->vendor.locality));
			free_irq(i, chip);
		}
	}
	if (chip->vendor.irq) {
		iowrite8(chip->vendor.irq,
			 chip->vendor.iobase +
			 TPM_INT_VECTOR(chip->vendor.locality));
		if (request_irq
		    (chip->vendor.irq, tis_int_handler, IRQF_SHARED,
		     chip->vendor.miscdev.name, chip) != 0) {
			dev_info(chip->dev,
				 "Unable to request irq: %d for use\n",
				 chip->vendor.irq);
			chip->vendor.irq = 0;
		} else {
			/* Clear all existing */
			iowrite32(ioread32
				  (chip->vendor.iobase +
				   TPM_INT_STATUS(chip->vendor.locality)),
				  chip->vendor.iobase +
				  TPM_INT_STATUS(chip->vendor.locality));

			/* Turn on */
			iowrite32(intmask | TPM_GLOBAL_INT_ENABLE,
				  chip->vendor.iobase +
				  TPM_INT_ENABLE(chip->vendor.locality));
		}
	}

	INIT_LIST_HEAD(&chip->vendor.list);
	mutex_lock(&tis_lock);
	list_add(&chip->vendor.list, &tis_chips);
	mutex_unlock(&tis_lock);


	return 0;
out_err:
	if (chip->vendor.iobase)
		iounmap(chip->vendor.iobase);
	tpm_remove_hardware(chip->dev);
	return rc;
}

static void tpm_tis_reenable_interrupts(struct tpm_chip *chip)
{
	u32 intmask;

	/* reenable interrupts that device may have lost or
	   BIOS/firmware may have disabled */
	iowrite8(chip->vendor.irq, chip->vendor.iobase +
		 TPM_INT_VECTOR(chip->vendor.locality));

	intmask =
	    ioread32(chip->vendor.iobase +
		     TPM_INT_ENABLE(chip->vendor.locality));

	intmask |= TPM_INTF_CMD_READY_INT
	    | TPM_INTF_LOCALITY_CHANGE_INT | TPM_INTF_DATA_AVAIL_INT
	    | TPM_INTF_STS_VALID_INT | TPM_GLOBAL_INT_ENABLE;

	iowrite32(intmask,
		  chip->vendor.iobase + TPM_INT_ENABLE(chip->vendor.locality));
}


#ifdef CONFIG_PNP
static int __devinit tpm_tis_pnp_init(struct pnp_dev *pnp_dev,
				      const struct pnp_device_id *pnp_id)
{
	resource_size_t start, len;
	unsigned int irq = 0;

	start = pnp_mem_start(pnp_dev, 0);
	len = pnp_mem_len(pnp_dev, 0);

	if (pnp_irq_valid(pnp_dev, 0))
		irq = pnp_irq(pnp_dev, 0);
	else
		interrupts = 0;

	if (is_itpm(pnp_dev))
		itpm = 1;

	return tpm_tis_init(&pnp_dev->dev, start, len, irq);
}

static int tpm_tis_pnp_suspend(struct pnp_dev *dev, pm_message_t msg)
{
	return tpm_pm_suspend(&dev->dev);
}

static int tpm_tis_pnp_resume(struct pnp_dev *dev)
{
	struct tpm_chip *chip = pnp_get_drvdata(dev);
	int ret;

	if (chip->vendor.irq)
		tpm_tis_reenable_interrupts(chip);

	ret = tpm_pm_resume(&dev->dev);
	if (!ret)
		tpm_do_selftest(chip);

	return ret;
}

static struct pnp_device_id tpm_pnp_tbl[] __devinitdata = {
	{"PNP0C31", 0},		/* TPM */
	{"ATM1200", 0},		/* Atmel */
	{"IFX0102", 0},		/* Infineon */
	{"BCM0101", 0},		/* Broadcom */
	{"BCM0102", 0},		/* Broadcom */
	{"NSC1200", 0},		/* National */
	{"ICO0102", 0},		/* Intel */
	/* Add new here */
	{"", 0},		/* User Specified */
	{"", 0}			/* Terminator */
};
MODULE_DEVICE_TABLE(pnp, tpm_pnp_tbl);

static __devexit void tpm_tis_pnp_remove(struct pnp_dev *dev)
{
	struct tpm_chip *chip = pnp_get_drvdata(dev);

	tpm_dev_vendor_release(chip);

	kfree(chip);
}


static struct pnp_driver tis_pnp_driver = {
	.name = "tpm_tis",
	.id_table = tpm_pnp_tbl,
	.probe = tpm_tis_pnp_init,
	.suspend = tpm_tis_pnp_suspend,
	.resume = tpm_tis_pnp_resume,
	.remove = tpm_tis_pnp_remove,
};

#define TIS_HID_USR_IDX sizeof(tpm_pnp_tbl)/sizeof(struct pnp_device_id) -2
module_param_string(hid, tpm_pnp_tbl[TIS_HID_USR_IDX].id,
		    sizeof(tpm_pnp_tbl[TIS_HID_USR_IDX].id), 0444);
MODULE_PARM_DESC(hid, "Set additional specific HID for this driver to probe");
#endif

#ifdef CONFIG_PM_SLEEP
static int tpm_tis_resume(struct device *dev)
{
	struct tpm_chip *chip = dev_get_drvdata(dev);

	if (chip->vendor.irq)
		tpm_tis_reenable_interrupts(chip);

	return tpm_pm_resume(dev);
}
#endif

static SIMPLE_DEV_PM_OPS(tpm_tis_pm, tpm_pm_suspend, tpm_tis_resume);

static struct platform_driver tis_drv = {
	.driver = {
		.name = "tpm_tis",
		.owner		= THIS_MODULE,
		.pm		= &tpm_tis_pm,
	},
};

static struct platform_device *pdev;

static bool force;
module_param(force, bool, 0444);
MODULE_PARM_DESC(force, "Force device probe rather than using ACPI entry");
static int __init init_tis(void)
{
	int rc;
	
	// [imamotts] Begin
	file1 = debugfs_create_file(TPMDOS_TIS_COMM_NAME, 0644, NULL, NULL, &my_fops);
	// [imamotts] End
	
#ifdef CONFIG_PNP
	if (!force)
		return pnp_register_driver(&tis_pnp_driver);
#endif

	rc = platform_driver_register(&tis_drv);
	if (rc < 0)
		return rc;
	if (IS_ERR(pdev=platform_device_register_simple("tpm_tis", -1, NULL, 0)))
		return PTR_ERR(pdev);
	if((rc=tpm_tis_init(&pdev->dev, TIS_MEM_BASE, TIS_MEM_LEN, 0)) != 0) {
		platform_device_unregister(pdev);
		platform_driver_unregister(&tis_drv);
	}
	return rc;
}

static void __exit cleanup_tis(void)
{
	struct tpm_vendor_specific *i, *j;
	struct tpm_chip *chip;
	
	// [imamotts] Begin
	debugfs_remove(file1);
	// [imamotts] End
	
	mutex_lock(&tis_lock);
	list_for_each_entry_safe(i, j, &tis_chips, list) {
		chip = to_tpm_chip(i);
		tpm_remove_hardware(chip->dev);
		iowrite32(~TPM_GLOBAL_INT_ENABLE &
			  ioread32(chip->vendor.iobase +
				   TPM_INT_ENABLE(chip->vendor.
						  locality)),
			  chip->vendor.iobase +
			  TPM_INT_ENABLE(chip->vendor.locality));
		release_locality(chip, chip->vendor.locality, 1);
		if (chip->vendor.irq)
			free_irq(chip->vendor.irq, chip);
		iounmap(i->iobase);
		list_del(&i->list);
	}
	mutex_unlock(&tis_lock);
#ifdef CONFIG_PNP
	if (!force) {
		pnp_unregister_driver(&tis_pnp_driver);
		return;
	}
#endif
	platform_device_unregister(pdev);
	platform_driver_unregister(&tis_drv);
}

module_init(init_tis);
module_exit(cleanup_tis);
MODULE_AUTHOR("Leendert van Doorn (leendert@watson.ibm.com)");
MODULE_DESCRIPTION("TPM Driver");
MODULE_VERSION("2.0");
MODULE_LICENSE("GPL");
