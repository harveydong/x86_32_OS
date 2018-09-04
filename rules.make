
.PHONY:dummy

first_rule: sub_dirs
	$(MAKE) all_targets

SUB_DIRS:=$(subdir-y)
ALL_SUB_DIRS:=$(sort $(subdir-y) $(subdir-))

%.s:%.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -S $< -o $@
%.i:%.c
	$(CPP) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) $< -o > $@
%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<
%.o:%.s
	$(AS) $(AFLAGS) $(EXTRA_CFLAGS) -o $@ $<

all_targets: $(O_TARGET) $(L_TARGET)

$(O_TARGET):$(obj-y)
	rm -rf $@
	$(LD) $(EXTRA_LDFLAGS) -r -o $@ $(filter $(obj-y),$^)

$(L_TARGET):$(obj-y)
	rm -rf $@
	$(AR) $(EXTRA_ARFLAGS) rcs $@ $(obj-y)

ifdef ALL_SUB_DIRS
	$(MAKE) $(patsubst %,_sfdep_%,$(ALL_SUB_DIRS)) _FASTDEP_ALL_SUB_DIRS="$(ALL_SUB_DIRS)"
endif

ifdef _FASTDEP_ALL_SUB_DIRS
$(patsubst %,_sfdep_%,$(_FASTDEP_ALL_SUB_DIRS));
	$(MAKE) -C $(patsubst _sfdep_%,%,$@) fastdep
endif

subdir-list=$(sort $(patsubst %,_subdir_%,$(SUB_DIRS)))
sub_dirs: dummy $(subdir-list)

$(subdir-list):dummy
	$(MAKE) -C $(patsubst _subdir_%,%,$@)

dummy:

