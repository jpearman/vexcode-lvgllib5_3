# VEXcode makefile 2019_03_26_01

# show compiler output
VERBOSE = 0

# include toolchain options
include vex/mkenv.mk

# location of the project source cpp and c files
SRC_C += $(wildcard src/*.c)
SRC_C += $(wildcard lvgl/lv_core/*.c) 
SRC_C += $(wildcard lvgl/lv_draw/*.c) 
SRC_C += $(wildcard lvgl/lv_fonts/*.c) 
SRC_C += $(wildcard lvgl/lv_hal/*.c) 
SRC_C += $(wildcard lvgl/lv_misc/*.c) 
SRC_C += $(wildcard lvgl/lv_objx/*.c) 
SRC_C += $(wildcard lvgl/lv_themes/*.c) 

OBJA = $(addprefix $(BUILD)/, $(addsuffix .o, $(basename $(SRC_C))) )
OBJ  = $(BUILD)/demo/demo.o

# location of include files that c and cpp files depend on
SRC_H  = $(wildcard include/*.h)

# additional dependancies
SRC_A  = makefile

# project header file locations
INC_F  = include . lvgl

# headers needed to use library
LV_SRC_H += $(wildcard lvgl/*.h) 
LV_SRC_H += $(wildcard lvgl/lv_core/*.h) 
LV_SRC_H += $(wildcard lvgl/lv_draw/*.h) 
LV_SRC_H += $(wildcard lvgl/lv_fonts/*.h) 
LV_SRC_H += $(wildcard lvgl/lv_hal/*.h) 
LV_SRC_H += $(wildcard lvgl/lv_misc/*.h) 
LV_SRC_H += $(wildcard lvgl/lv_objx/*.h) 
LV_SRC_H += $(wildcard lvgl/lv_themes/*.h) 

LV_DST_H = $(addprefix $(BUILD)/include/, $(LV_SRC_H) )
LV_DST_H += $(BUILD)/include/lv_conf.h $(BUILD)/include/v5lvgl.h

$(BUILD)/include/%: %
	$(Q)$(MKDIR)
	$(Q) $(call copyfile,$^, $@)

vpath %.h lvgl/ include/

# override default library name
PROJECTLIB = libv5lvgl

# build targets
all: $(BUILD)/$(PROJECT).bin $(BUILD)/$(PROJECTLIB).a inc

# copy lvgl header files
.PHONY: inc
inc: $(LV_DST_H)
	$(ECHO) "Copy headers to build folder"

# include build rules
include vex/mkrules.mk
