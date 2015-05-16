TARGET:=$(notdir $(lastword $(CURDIR)))



TOOLCHAIN_PATH:=/opt/cross/microcontroller/arm/4.8.4-release-candidate-2015-04-09_184647/bin
#TOOLCHAIN_PATH:=/opt/cross/microcontroller/arm/4.9.2-release-candidate-2015-04-09_110954/bin
#TOOLCHAIN_PATH:=/opt/cross/microcontroller/arm/5-20150405-release-candidate-2015-04-09_094306/bin



TOOLCHAIN_PREFIX:=arm-none-eabi

#
# With OPTLVL=g the project compiled but failed to link in with gcc 4.8.3
#
OPTLVL:=0 # Optimization level, can be [0, 1, 2, 3, s, g, fast].

AA_SRCDIR:=AA_src

# make sets CURRDIR to the path name of the current directory.
PROJECT_NAME:=$(notdir $(lastword $(CURDIR)))

LINKER_SCRIPT=project_stm32_flash.ld

#
# $VENDOR_STM_SOURCE_DIR is the directory created by unzip-ing stm32f4discovery_fw.zip
# md5sum:
# 6063f18dff8b5f1ddfafa77d1ab72ad9  stm32f4discovery_fw.zip
#
# With the introduction of the Cube software ST has kept this legacy 
# code avialable. md5sum (or diff) indicates it is the same file renamed.
# md5sum:  
# 6063f18dff8b5f1ddfafa77d1ab72ad9  stsw-stm32068.zip
VENDOR_STM_SOURCE_DIR:=/opt/cross/microcontroller/arm/STM32F4-Discovery_FW_V1.1.0


# We are (2014-Dec-16) using an Olimex board, not the discovery board.
# 
# 
#
#
INCLUDE=-I$(CURDIR)

INCLUDE+=-I$(VENDOR_STM_SOURCE_DIR)/Libraries/CMSIS/ST/STM32F4xx/Include/
INCLUDE+=-I$(VENDOR_STM_SOURCE_DIR)/Libraries/CMSIS/STM32F4xx/Include/
INCLUDE+=-I$(VENDOR_STM_SOURCE_DIR)/Libraries/CMSIS/Include/
INCLUDE+=-I$(VENDOR_STM_SOURCE_DIR)/Libraries/STM32F4xx_StdPeriph_Driver/inc/

INCLUDE+=-I/usr/local/include/eigen3/

# INCLUDE+=-I$(VENDOR_STM_SOURCE_DIR)/Utilities/STM32F4-Discovery/
INCLUDE+=-I./$(AA_SRCDIR)

# vpath is used so object files are written to the current directory instead
# of the same directory as their source files
#
# This directory contains stm32f4xx_rcc.c stm32f4xx_sdio.c and 
# many other files matching stm32f4xx_*.c and misc.c
vpath %.c $(VENDOR_STM_SOURCE_DIR)/Libraries/STM32F4xx_StdPeriph_Driver/src/
#
# This directory contains  startup_stm32f4xx.s
# vpath %.s $(VENDOR_STM_SOURCE_DIR)/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7
vpath %.s $(VENDOR_STM_SOURCE_DIR)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/gcc_ride7/
#

# ASRC=startup_stm32f401xx.s
ASRC=startup_stm32f4xx.s

# Project Source Files
SRC=stm32f4xx_it.c
SRC+=system_stm32f4xx.c
SRC+=timebase_32768Hz_it.c
SRC+=microsecond_delay_it.c
# SRC+=embedded_machar.c
# SRC+=rtc_management.c
SRC+=build_date.c
SRC+=matrix_keypad_it.c
SRC+=rtc_management_it.c

# 
# main() is in main.cpp 
# If mixing C and C++, int main() needs to be compiled as C++ code.
#

# Standard Peripheral Source Files
SRC+=stm32f4xx_rcc.c
SRC+=misc.c
SRC+=stm32f4xx_gpio.c
SRC+=stm32f4xx_exti.c
SRC+=stm32f4xx_syscfg.c
SRC+=stm32f4xx_tim.c
SRC+=stm32f4xx_rtc.c
SRC+=stm32f4xx_pwr.c

CPPSRC=main.cpp
CPPSRC+=microsecond_delay.cpp
CPPSRC+=timebase_32768Hz.cpp
CPPSRC+=stm32_e407_utilities.cpp
CPPSRC+=Print.cpp
CPPSRC+=char_lcd_stm32f4.cpp
CPPSRC+=quadrature_decoder.cpp
CPPSRC+=sexagesimal.cpp
CPPSRC+=navigation_star.cpp
CPPSRC+=starlist_access.cpp
CPPSRC+=ngc_list_access.cpp
CPPSRC+=messier.cpp
CPPSRC+=controller.cpp
CPPSRC+=telescope_model.cpp
CPPSRC+=rtc_management.cpp
CPPSRC+=solar_system.cpp
CPPSRC+=horizontal_parallax.cpp
CPPSRC+=extra_solar_transforms.cpp
CPPSRC+=matrix_keypad.cpp
CPPSRC+=input_views.cpp
CPPSRC+=menu_views.cpp
CPPSRC+=output_views.cpp
CPPSRC+=proximity_views.cpp
CPPSRC+=horizontal_equatorial.cpp
CPPSRC+=alignment_sight_info.cpp
CPPSRC+=linear_algebra_facilities.cpp
CPPSRC+=linear_algebra_interface.cpp
CPPSRC+=lbr_and_xyz.cpp
CPPSRC+=topocentric_unit_vectors.cpp
CPPSRC+=flex_lexer.cpp
CPPSRC+=command_actions.cpp
CPPSRC+=sight_data_command_actions.cpp
CPPSRC+=specificities.cpp

# (Subset of) Astronomical Algorithms Source Files
CPPSRC+=$(AA_SRCDIR)/AAAngularSeparation.cpp
CPPSRC+=$(AA_SRCDIR)/AACoordinateTransformation.cpp
CPPSRC+=$(AA_SRCDIR)/AARefraction.cpp
CPPSRC+=$(AA_SRCDIR)/stdafx.cpp
CPPSRC+=$(AA_SRCDIR)/AANutation.cpp
CPPSRC+=$(AA_SRCDIR)/AASidereal.cpp
CPPSRC+=$(AA_SRCDIR)/AADate.cpp
CPPSRC+=$(AA_SRCDIR)/AAPrecession.cpp
CPPSRC+=$(AA_SRCDIR)/AAAberration.cpp
CPPSRC+=$(AA_SRCDIR)/AADynamicalTime.cpp
CPPSRC+=$(AA_SRCDIR)/AAEarth.cpp
CPPSRC+=$(AA_SRCDIR)/AASun.cpp
CPPSRC+=$(AA_SRCDIR)/AAMoon.cpp
CPPSRC+=$(AA_SRCDIR)/AAFK5.cpp
CPPSRC+=$(AA_SRCDIR)/AANearParabolic.cpp
CPPSRC+=$(AA_SRCDIR)/AAEclipticalElements.cpp
CPPSRC+=$(AA_SRCDIR)/AAKepler.cpp
CPPSRC+=$(AA_SRCDIR)/AAMercury.cpp
CPPSRC+=$(AA_SRCDIR)/AAVenus.cpp
CPPSRC+=$(AA_SRCDIR)/AAMars.cpp
CPPSRC+=$(AA_SRCDIR)/AAJupiter.cpp
CPPSRC+=$(AA_SRCDIR)/AASaturn.cpp
CPPSRC+=$(AA_SRCDIR)/AAUranus.cpp
CPPSRC+=$(AA_SRCDIR)/AANeptune.cpp
CPPSRC+=$(AA_SRCDIR)/AAPluto.cpp
CPPSRC+=$(AA_SRCDIR)/AAElliptical.cpp
CPPSRC+=$(AA_SRCDIR)/AAElementsPlanetaryOrbit.cpp
CPPSRC+=$(AA_SRCDIR)/AAParallax.cpp
CPPSRC+=$(AA_SRCDIR)/AAGlobe.cpp



# March 2014 attempted update to  STM32F4xx_DSP_StdPeriph_Lib_V1.3.0/
# Define was: 
CDEFS=-DSTM32F4XX
# See stm32f4xx.h Line 87,88,89.  The new define is STM32F40_41xxx 
# CDEFS=-DSTM32F40XX

CDEFS+=-DUSE_STDPERIPH_DRIVER
CDEFS+=-DUSE_FULL_ASSERT
# Project specific #define indicating the linear algebra package.
CDEFS+=-DUSE_EIGEN_FOR_LINEAR_ALGEBRA
#
# Eigen #define.  See COPYING.README in the Eigen source.
# Warn and stop if non permisive licence Eigen code is used.
CDEFS+=-DEIGEN_MPL2_ONLY  

MCUFLAGS=-mthumb -mcpu=cortex-m4 

# COMMONFLAGS=-O$(OPTLVL)  -ggdb3 -Werror -Wall -fno-omit-frame-pointer 
# COMMONFLAGS=-O$(OPTLVL) -ggdb3 -Werror -Wall -Wno-unused-local-typedefs -Wno-unused-function  -flto -fno-omit-frame-pointer
COMMONFLAGS=-O$(OPTLVL) -ggdb3 -Werror -Wall -Wno-unused-local-typedefs -flto -fno-omit-frame-pointer
# COMMONFLAGS=-O$(OPTLVL) -ggdb3 -Werror -flto -fno-omit-frame-pointer
# COMMONFLAGS=-O$(OPTLVL)   -g -Werror -Wall -fno-omit-frame-pointer 

CFLAGS=$(COMMONFLAGS) $(MCUFLAGS) $(INCLUDE) $(CDEFS) #
#
# My build of GCC 4.8.4 does not recognize -std=c++14
# -std=c++11 is OK
CXXFLAGS= -std=c++11 -felide-constructors $(CFLAGS) 

LDLIBS=-lm -lgcc
LDFLAGS=$(COMMONFLAGS)  -fno-exceptions -ffunction-sections -fdata-sections \
        -nostartfiles -Wl,--gc-sections,-T$(LINKER_SCRIPT) 

#####
#####
OBJ = $(SRC:%.c=%.o) $(CPPSRC:%.cpp=%.o) $(ASRC:%.s=%.o)

CC=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc
CXX=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-g++
LD=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-g++
OBJCOPY=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-objcopy
AS=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-as
AR=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-ar
GDB=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gdb

all: $(TARGET).elf
	$(OBJCOPY) -O ihex   $(TARGET).elf $(TARGET).hex
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	wc --bytes $(TARGET).bin

$(TARGET).elf: $(OBJ) $(LINKER_SCRIPT)
	$(LD) -o $(TARGET).elf $(LDFLAGS) $(OBJ)	$(LDLIBS) 
#
$(LINKER_SCRIPT): dso_handle_ld.patch $(VENDOR_STM_SOURCE_DIR)/Project/Peripheral_Examples/EXTI/TrueSTUDIO/EXTI/stm32_flash.ld
	patch $(VENDOR_STM_SOURCE_DIR)/Project/Peripheral_Examples/EXTI/TrueSTUDIO/EXTI/stm32_flash.ld -o $(LINKER_SCRIPT) < dso_handle_ld.patch

$(SRC) $(CPPSRC): $(AA_SRCDIR) 
#
$(AA_SRCDIR): aaplus.zip
	rm -Rf $(AA_SRCDIR)
	mkdir -p $(AA_SRCDIR)
	unzip -d $(AA_SRCDIR) aaplus.zip

$(OBJ): starlist.h ngc_list.h

flex_lexer.cpp: flex_lexer.l
	flex -o flex_lexer.cpp --header=flex_lexer.h flex_lexer.l

flex_lexer.h: flex_lexer.cpp

starlist.h:     make_star_list catalog
	rm -f starlist.h
	./make_star_list > starlist.h;    indent  starlist.h

FLEX_LEXER_ADDITIONAL_COMPILE_FLAG=-Wno-unused-function -Wno-sign-compare
# flex_lexer.cpp contains unused functions and won't compile under -Werror -Wall
# Ideally I should find the right flex flags to eliminate the unused functions.
flex_lexer.o: flex_lexer.cpp flex_lexer.h 
	$(CXX) $(CXXFLAGS) $(FLEX_LEXER_ADDITIONAL_COMPILE_FLAG) -c -o flex_lexer.o flex_lexer.cpp


#
#  This didn't work so well.  To many files #include "main.h"
#
#LINEAR_ALGEBRA_INTERFACE_ADDITIONAL_COMPILE_FLAGS=-Wno-unused-local-typedefs 
#linear_algebra_interface.o: linear_algebra_interface.cpp linear_algebra_interface.h 
#	$(CXX) $(CXXFLAGS) $(LINEAR_ALGEBRA_INTERFACE_ADDITIONAL_COMPILE_FLAGS) -c -o linear_algebra_interface.o linear_algebra_interface.cpp
#
#main.o: main.cpp
#	$(CXX) $(CXXFLAGS) $(LINEAR_ALGEBRA_INTERFACE_ADDITIONAL_COMPILE_FLAGS) -c -o main.o main.cpp



catalog:
	cp YaleStarCatalog/catalog.gz  .
	gunzip catalog.gz

ngc_list.h: make_ngc_list.pl DPublic_HCNGC.txt sex_to_hex
	./make_ngc_list.pl > ngc_list.h

sex_to_hex: sex_to_hex.cpp sexagesimal.cpp sexagesimal.h
	g++ -std=c++11 -g -o sex_to_hex sex_to_hex.cpp sexagesimal.cpp

DPublic_HCNGC.txt: Public_HCNGC.zip
	rm -f DPublic_HCNGC.txt Public_HCNGC.txt Public_HCNGC.xls	
	unzip Public_HCNGC.zip
	touch DPublic_HCNGC.txt

make_star_list: make_star_list.c navigation_star.cpp  sexagesimal.cpp
	g++ -std=c++11 -g -o make_star_list make_star_list.c navigation_star.cpp  sexagesimal.cpp

debug: $(TARGET).elf 
	./do_flash.pl $(TARGET).bin 
	$(GDB) $(TARGET).elf -x "target extended-remote localhost:3333"

bugger:
	@echo OBJ =
	@echo $(OBJ)


.PHONY: clean

flash: $(TARGET).bin
	./do_flash.pl $(TARGET).bin 

clean:
	rm -f $(TARGET).elf
	rm -f $(TARGET).hex
	rm -f $(TARGET).bin
	rm -f $(LINKER_SCRIPT)
	rm -f openocd.log
	rm -f *.o
	rm -f catalog
	rm -f starlist.h starlist.h~
	rm -f ngc_list.h ngc_list.h~
	rm -f make_star_list
	rm -Rf $(AA_SRCDIR)
	rm -f DPublic_HCNGC.txt Public_HCNGC.txt Public_HCNGC.xls
	rm -f sex_to_hex
	rm -f flex_lexer.cpp flex_lexer.h
