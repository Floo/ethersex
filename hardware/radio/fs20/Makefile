TOPDIR ?= ../../..
include $(TOPDIR)/.config

$(FS20_SUPPORT)_SRC += hardware/radio/fs20/fs20.c
$(REC868_SUPPORT)_ECMD_SRC += hardware/radio/fs20/rec868_ecmd.c
$(REC868_SUPPORT)_SRC += hardware/radio/fs20/rec868.c

$(FS20_RECEIVE_SENDER)_SRC += hardware/radio/fs20/fs20_sender_net.c
$(FS20_SUPPORT)_ECMD_SRC += hardware/radio/fs20/fs20_ecmd.c


##############################################################################
# generic fluff
include $(TOPDIR)/scripts/rules.mk
