#
# Native toolchain expressed as if a cross toolchain.
#
TOOLCHAIN_PATH:=/usr/bin
TOOLCHAIN_PREFIX:=
#
OPTLVL:=0 # Optimization level, can be [0, 1, 2, 3, s, g].

TOP:=$(shell readlink -f "../../..")

AA_SRCDIR=../AA_src
#
vpath %.cpp $(AA_SRCDIR) ..

# vpath is used so object files are written to the current directory instead
# of the same directory as their source files
INCLUDE=-I..
INCLUDE+=-I$(AA_SRCDIR)


# Plain c files
# SRC=main.c
# 
# CPPSRC=main.cpp
CPPSRC+=AACoordinateTransformation.cpp
CPPSRC+=AADate.cpp
CPPSRC+=AADynamicalTime.cpp
CPPSRC+=AAAberration.cpp
CPPSRC+=AAPrecession.cpp
CPPSRC+=AANutation.cpp
CPPSRC+=AARefraction.cpp
CPPSRC+=AAElliptical.cpp
CPPSRC+=AAKepler.cpp
CPPSRC+=AANearParabolic.cpp
CPPSRC+=extra_solar_transforms.cpp
CPPSRC+=sexagesimal.cpp
CPPSRC+=AASun.cpp
CPPSRC+=AAEarth.cpp
CPPSRC+=AAFK5.cpp
CPPSRC+=AAGlobe.cpp
CPPSRC+=AAParallax.cpp
CPPSRC+=navigation_star.cpp
CPPSRC+=solar_system.cpp
CPPSRC+=horizontal_equatorial.cpp
CPPSRC+=horizontal_parallax.cpp
CPPSRC+=lbr_and_xyz.cpp


#
# It will be tedious to get all of the required includes to compile this.
# The flaw is a lack of compartmentalization.  I need to segregate the routines
# that extract data from an input_view into separate code files that can be compiled for
# either the arm uC or the x86[_64] build machine.
# CPPSRC+=input_views.cpp


CPPSRC+=AAMercury.cpp
CPPSRC+=AAVenus.cpp
CPPSRC+=AAMoon.cpp
CPPSRC+=AAMars.cpp
CPPSRC+=AAJupiter.cpp
CPPSRC+=AASaturn.cpp
CPPSRC+=AAUranus.cpp
CPPSRC+=AANeptune.cpp
CPPSRC+=AAPluto.cpp
CPPSRC+=AASidereal.cpp
CPPSRC+=neo_bsc_starlist.cpp
CPPSRC+=neo_whiz_bang_ngc_list.cpp
#
#
CPPSRC+=AAVSOP87.cpp
CPPSRC+=AAVSOP87A_EAR.cpp
CPPSRC+=AAVSOP87A_EMB.cpp
CPPSRC+=AAVSOP87A_JUP.cpp
CPPSRC+=AAVSOP87A_MAR.cpp
CPPSRC+=AAVSOP87A_MER.cpp
CPPSRC+=AAVSOP87A_NEP.cpp
CPPSRC+=AAVSOP87A_SAT.cpp
CPPSRC+=AAVSOP87A_URA.cpp
CPPSRC+=AAVSOP87A_VEN.cpp
CPPSRC+=AAVSOP87B_EAR.cpp
CPPSRC+=AAVSOP87B_JUP.cpp
CPPSRC+=AAVSOP87B_MAR.cpp
CPPSRC+=AAVSOP87B_MER.cpp
CPPSRC+=AAVSOP87B_NEP.cpp
CPPSRC+=AAVSOP87B_SAT.cpp
CPPSRC+=AAVSOP87B_URA.cpp
CPPSRC+=AAVSOP87B_VEN.cpp
CPPSRC+=AAVSOP87C_EAR.cpp
CPPSRC+=AAVSOP87C_JUP.cpp
CPPSRC+=AAVSOP87C_MAR.cpp
CPPSRC+=AAVSOP87C_MER.cpp
CPPSRC+=AAVSOP87C_NEP.cpp
CPPSRC+=AAVSOP87C_SAT.cpp
CPPSRC+=AAVSOP87C_URA.cpp
CPPSRC+=AAVSOP87C_VEN.cpp
CPPSRC+=AAVSOP87D_EAR.cpp
CPPSRC+=AAVSOP87D_JUP.cpp
CPPSRC+=AAVSOP87D_MAR.cpp
CPPSRC+=AAVSOP87D_MER.cpp
CPPSRC+=AAVSOP87D_NEP.cpp
CPPSRC+=AAVSOP87D_SAT.cpp
CPPSRC+=AAVSOP87D_URA.cpp
CPPSRC+=AAVSOP87D_VEN.cpp
CPPSRC+=AAVSOP87E_EAR.cpp
CPPSRC+=AAVSOP87E_JUP.cpp
CPPSRC+=AAVSOP87E_MAR.cpp
CPPSRC+=AAVSOP87E_MER.cpp
CPPSRC+=AAVSOP87E_NEP.cpp
CPPSRC+=AAVSOP87E_SAT.cpp
CPPSRC+=AAVSOP87E_SUN.cpp
CPPSRC+=AAVSOP87E_URA.cpp
CPPSRC+=AAVSOP87E_VEN.cpp
CPPSRC+=AAVSOP87_EMB.cpp
CPPSRC+=AAVSOP87_JUP.cpp
CPPSRC+=AAVSOP87_MAR.cpp
CPPSRC+=AAVSOP87_MER.cpp
CPPSRC+=AAVSOP87_NEP.cpp
CPPSRC+=AAVSOP87_SAT.cpp
CPPSRC+=AAVSOP87_URA.cpp
CPPSRC+=AAVSOP87_VEN.cpp
# See documentation aa.html for version 1.63 (16 September 2015) if Naughter's source.
CDEFS+=-DAAPLUS_VSOP87_NO_HIGH_PRECISION

COMMONFLAGS=-O$(OPTLVL) -ggdb3 -Wall -Werror -std=c++14
CFLAGS=$(COMMONFLAGS) $(MCUFLAGS) $(INCLUDE) $(CDEFS)


# e.g. -lm
LDLIBS=
LDFLAGS=$(COMMONFLAGS) -Wl,--gc-sections

#####
#####

OBJ = $(SRC:%.c=%.o) $(CPPSRC:%.cpp=%.o) $(ASRC:%.s=%.o)

CC=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)g++
CXX=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)g++
#CC=/usr/local/bin/g++
#CXX=/usr/local/bin/g++
LD=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-ld
OBJCOPY=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-objcopy
AS=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-as
AR=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-ar
GDB=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gdb
CXXFLAGS=$(CFLAGS)

TESTS= example_23b example_33a nautical_almanac astronomical_almanac jd nutation_data \
 memoized_mean_sidereal memoized_apparent_sidereal burnham_test horizontal2equatorial \
 h2q intercept_azimuth near_parabolic moon_test planet_declinations

all: $(AA_SRCDIR) objects tests

objects: ../neo_bsc_starlist.cpp ../neo_whiz_bang_ngc_list.cpp
	$(MAKE) -C . $(OBJ)

tests: $(TESTS)
#
$(TESTS): $(AA_SRCDIR) $(OBJ)
#
#
#
$(AA_SRCDIR) : ../aaplus.zip
	$(MAKE) -C .. AA_src

# $(CPPSRC) : $(AA_SRCDIR)
# AACoordinateTransformation.o: $(AA_SRCDIR) $(AA_SRCDIR)/AACoordinateTransformation.cpp
# 	$(CXX) $(CXXFLAGS) -c -o AACoordinateTransformation.o $(INCLUDE) $(AA_SRCDIR)/AACoordinateTransformation.cpp

../neo_bsc_starlist.cpp:
	$(MAKE) -C .. neo_bsc_starlist.cpp

../neo_whiz_bang_ngc_list.cpp:
	$(MAKE) -C .. neo_whiz_bang_ngc_list.cpp

../ngc_list.h:
	 $(MAKE) -C .. ngc_list.h

example_23b: $(OBJ) example_23b.o
	$(CXX) -o example_23b  $(LDFLAGS) $(OBJ) example_23b.o $(LDLIBS) 

example_23b.o: example_23b.cpp
	$(CXX) $(CXXFLAGS) -c -o example_23b.o $(INCLUDE) example_23b.cpp

example_33a: $(OBJ) example_33a.o
	$(CXX) -o example_33a  $(LDFLAGS) $(OBJ) example_33a.o $(LDLIBS) 

example_33a.o: example_33a.cpp
	$(CXX) $(CXXFLAGS) -c -o example_33a.o $(INCLUDE) example_33a.cpp

nautical_almanac: $(OBJ) nautical_almanac.o
	$(CXX) -o nautical_almanac  $(LDFLAGS) $(OBJ) nautical_almanac.o $(LDLIBS) 

nautical_almanac.o: nautical_almanac.cpp
	$(CXX) $(CXXFLAGS) -c -o nautical_almanac.o $(INCLUDE) nautical_almanac.cpp

astronomical_almanac: $(OBJ) astronomical_almanac.o
	$(CXX) -o astronomical_almanac  $(LDFLAGS) $(OBJ) astronomical_almanac.o $(LDLIBS) 

astronomical_almanac.o: astronomical_almanac.cpp
	$(CXX) $(CXXFLAGS) -c -o astronomical_almanac.o $(INCLUDE) astronomical_almanac.cpp

planet_declinations: $(OBJ) planet_declinations.o
	$(CXX) -o planet_declinations  $(LDFLAGS) $(OBJ) planet_declinations.o $(LDLIBS) 

planet_declinations.o: planet_declinations.cpp
	$(CXX) $(CXXFLAGS) -c -o planet_declinations.o $(INCLUDE) planet_declinations.cpp

burnham_test: $(OBJ) burnham_test.o
	$(CXX) -o burnham_test  $(LDFLAGS) $(OBJ) burnham_test.o $(LDLIBS) 

burnham_test.o: burnham_test.cpp
	$(CXX) $(CXXFLAGS) -c -o burnham_test.o $(INCLUDE) burnham_test.cpp

nutation_data: $(OBJ) nutation_data.o
	$(CXX) -o nutation_data  $(LDFLAGS) $(OBJ) nutation_data.o $(LDLIBS) 

nutation_data.o: nutation_data.cpp
	$(CXX) $(CXXFLAGS) -c -o nutation_data.o $(INCLUDE) nutation_data.cpp

memoized_apparent_sidereal: $(OBJ) memoized_apparent_sidereal.o
	$(CXX) -o memoized_apparent_sidereal  $(LDFLAGS) $(OBJ) memoized_apparent_sidereal.o $(LDLIBS) 

memoized_apparent_sidereal.o: memoized_apparent_sidereal.cpp
	$(CXX) $(CXXFLAGS) -c -o memoized_apparent_sidereal.o $(INCLUDE) memoized_apparent_sidereal.cpp


memoized_mean_sidereal: $(OBJ) memoized_mean_sidereal.o
	$(CXX) -o memoized_mean_sidereal  $(LDFLAGS) $(OBJ) memoized_mean_sidereal.o $(LDLIBS) 

memoized_mean_sidereal.o: memoized_mean_sidereal.cpp
	$(CXX) $(CXXFLAGS) -c -o memoized_mean_sidereal.o $(INCLUDE) memoized_mean_sidereal.cpp

jd: $(OBJ) jd.o
	$(CXX) -o jd  $(LDFLAGS) $(OBJ) jd.o $(LDLIBS) 

jd.o: jd.cpp
	$(CXX) $(CXXFLAGS) -c -o jd.o $(INCLUDE) jd.cpp

horizontal2equatorial: $(OBJ) horizontal2equatorial.o
	$(CXX) -o horizontal2equatorial  $(LDFLAGS) $(OBJ) horizontal2equatorial.o $(LDLIBS) 

horizontal2equatorial.o: horizontal2equatorial.cpp
	$(CXX) $(CXXFLAGS) -c -o horizontal2equatorial.o $(INCLUDE) horizontal2equatorial.cpp

h2q: $(OBJ) h2q.o
	$(CXX) -o h2q  $(LDFLAGS) $(OBJ) h2q.o $(LDLIBS) 

h2q.o: h2q.cpp
	$(CXX) $(CXXFLAGS) -c -o h2q.o $(INCLUDE) h2q.cpp


intercept_azimuth: $(OBJ) intercept_azimuth.o
	$(CXX) -o intercept_azimuth  $(LDFLAGS) $(OBJ) intercept_azimuth.o $(LDLIBS) 

intercept_azimuth.o: intercept_azimuth.cpp
	$(CXX) $(CXXFLAGS) -c -o intercept_azimuth.o $(INCLUDE) intercept_azimuth.cpp

near_parabolic: $(OBJ) near_parabolic.o
	$(CXX) -o near_parabolic  $(LDFLAGS) $(OBJ) near_parabolic.o $(LDLIBS) 

near_parabolic.o: near_parabolic.cpp
	$(CXX) $(CXXFLAGS) -c -o near_parabolic.o $(INCLUDE) near_parabolic.cpp


moon_test: $(OBJ) moon_test.o
	$(CC) -o moon_test  $(LDFLAGS) $(OBJ) moon_test.o $(LDLIBS) 

moon_test.o: moon_test.cpp
	$(CXX) $(CXXFLAGS) -c -o moon_test.o $(INCLUDE) moon_test.cpp


bugger:
	echo $(OBJ)



.PHONY: clean

clean:
	rm -f $(OBJ)
	rm -f ../starlist.h
	rm -Rf $(AA_SRCDIR)
	rm -f example_23b example_23b.o
	rm -f example_33a example_33a.o
	rm -f nautical_almanac nautical_almanac.o
	rm -f astronomical_almanac astronomical_almanac.o
	rm -f planet_declinations planet_declinations.o
	rm -f burnham_test burnham_test.o
	rm -f memoized_mean_sidereal memoized_mean_sidereal.o
	rm -f memoized_apparent_sidereal memoized_apparent_sidereal.o
	rm -f nutation_data nutation_data.o
	rm -f horizontal2equatorial horizontal2equatorial.o
	rm -f h2q h2q.o
	rm -f intercept_azimuth intercept_azimuth.o
	rm -f jd jd.o
	rm -f near_parabolic near_parabolic.o
	rm -f moon_test moon_test.o
#
#
