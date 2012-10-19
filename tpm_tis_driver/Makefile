#
# Makefile for the kernel tpm device drivers.
#
obj-$(CONFIG_TCG_TPM) += tpm.o
ifdef CONFIG_ACPI
	obj-$(CONFIG_TCG_TPM) += tpm_bios.o
endif
obj-$(CONFIG_TCG_TIS) += tpm_tis.o
obj-$(CONFIG_TCG_NSC) += tpm_nsc.o
obj-$(CONFIG_TCG_ATMEL) += tpm_atmel.o
obj-$(CONFIG_TCG_INFINEON) += tpm_infineon.o

KVERSION = $(shell uname -r)

test:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
	
install:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
	sudo cp /home/superymk/Desktop/Linux_tpm/tpm_tis.ko /lib/modules/3.2.0-32-generic-pae/kernel/drivers/char/tpm/tpm_tis.ko


clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
