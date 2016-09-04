################################################################################
#
# upgrade_tool
#
################################################################################

UPGRADE_TOOL_VERSION = 0.1
UPGRADE_TOOL_SITE = "${BR2_EXTERNAL}/package/upgrade_tool/src"
UPGRADE_TOOL_SITE_METHOD = local
UPGRADE_TOOL_DEPENDENCIES = uboot zlib

define UPGRADE_TOOL_BUILD_CMDS
		# copy source files from uboot folder
		UBOOT_DIR=$(shell find $(BUILD_DIR) -maxdepth 1 -type d -name uboot-*)
		mkdir -p $(@D)/uboot
		cp $(UBOOT_DIR)/include/image_table.h $(@D)/uboot
		cp $(UBOOT_DIR)/common/image_table.c $(@D)/uboot

    $(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

define UPGRADE_TOOL_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/upgrade_tool $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
