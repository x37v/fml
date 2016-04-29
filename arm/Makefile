# Sources

SRCS = src/stm32f4xx_it.c src/system_stm32f4xx.c src/startup_stm32f4xx.c
P_SRCS = src/main.cpp src/dac.cpp
P_SRCS += ../src/defines.cpp \
					../src/util.cpp \
					../src/envelope.cpp \
					../src/adsr.cpp \
					../src/fmvoice.cpp

S_SRCS = 

# USB
# SRCS += usbd_usr.c usbd_cdc_vcp.c usbd_desc.c usb_bsp.c

# Project name

PROJ_NAME = fml
OUTPATH = build

OUTPATH := $(abspath $(OUTPATH))

BASEDIR := $(abspath ./)

###################################################

# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
#override FLOAT_TYPE = hard
override FLOAT_TYPE = soft
endif
endif

###################################################

AS=$(BINPATH)arm-none-eabi-as
CC=$(BINPATH)arm-none-eabi-gcc
CXX=$(BINPATH)arm-none-eabi-g++
LD=$(BINPATH)arm-none-eabi-gcc
OBJCOPY=$(BINPATH)arm-none-eabi-objcopy
OBJDUMP=$(BINPATH)arm-none-eabi-objdump
SIZE=$(BINPATH)arm-none-eabi-size
GDB=$(BINPATH)arm-none-eabi-gdb

LINKER_SCRIPT = ./src/stm32_flash.ld

CPU = -mcpu=cortex-m4 -mthumb

CFLAGS  = $(CPU) -c -g -O0 -Wall
LDFLAGS  = $(CPU) -mlittle-endian -mthumb-interwork -nostartfiles -Wl,--gc-sections,-Map=$(OUTPATH)/$(PROJ_NAME).map,--cref --specs=nano.specs

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
else
CFLAGS += -msoft-float
endif

# Default to STM32F40_41xxx if no device is passed
ifeq ($(DEVICE_DEF), )
DEVICE_DEF = STM32F40_41xxx
endif

CFLAGS += -D$(DEVICE_DEF) -DARM_MATH_CM4

CPPFLAGS      = -fno-exceptions -fno-rtti

#vpath %.c src
vpath %.a lib


# Includes
INCLUDE_PATHS = -I$(BASEDIR)/inc -I$(BASEDIR)/lib/cmsis/stm32f4xx -I$(BASEDIR)/lib/cmsis/include
INCLUDE_PATHS += -I$(BASEDIR)/lib/Conf -I../src/

# Library paths
LIBPATHS = -L$(BASEDIR)/lib/STM32F4xx_StdPeriph_Driver

# Libraries to link
LIBS = -lstdperiph -lc -lgcc -lnosys -lm ./libarm_cortexM4l_math.a

# Extra includes
INCLUDE_PATHS += -I$(BASEDIR)/lib/STM32F4xx_StdPeriph_Driver/inc

#CFLAGS += -Map $(OUTPATH)/$(PROJ_NAME).map

OBJS = $(SRCS:src/%.c=build/%.o)
OBJS += $(S_SRCS:src/%.s=build/%.o)
OBJS += $(addsuffix .o, $(basename $(addprefix build/, $(notdir $(P_SRCS)))))


###################################################

.PHONY: lib proj

all: lib proj
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

blah:
	echo $(OBJS)

lib:
	$(MAKE) -C lib FLOAT_TYPE=$(FLOAT_TYPE) BINPATH=$(BINPATH) DEVICE_DEF=$(DEVICE_DEF) BASEDIR=$(BASEDIR)

proj: lib $(OUTPATH)/$(PROJ_NAME).elf

build/%.o: src/%.s build
	$(AS) $(CPU) -o $@ $<

build/%.o: src/%.cpp build
	$(CXX) $(CFLAGS) $(CPPFLAGS) -std=c++11 $(INCLUDE_PATHS) -o $@ $<

#there has got to be a better way
build/%.o: ../src/%.cpp build
	$(CXX) $(CFLAGS) $(CPPFLAGS) -std=c++11 $(INCLUDE_PATHS) -o $@ $<

build/%.o: src/%.c build
	$(CC) $(CFLAGS) -std=gnu99 $(INCLUDE_PATHS) -o $@ $<

build:
	mkdir -p build

$(OUTPATH)/$(PROJ_NAME).elf: $(OBJS)
	$(LD) $(LDFLAGS) -T$(LINKER_SCRIPT) $(LIBPATHS) -o $@ $^ $(LIBS) $(LD_SYS_LIBS)
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	$(OBJDUMP) -S --disassemble $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).dis

clean:
	rm -f $(OUTPATH)/*.o
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
	rm -f $(OUTPATH)/$(PROJ_NAME).dis
	rm -f $(OUTPATH)/$(PROJ_NAME).map
	# Remove this line if you don't want to clean the libs as well
	$(MAKE) clean -C lib

openocd:
	xterm -e "openocd -f stm32f4xx-openOCD.cfg" &
	
gdb: proj
	$(GDB)
