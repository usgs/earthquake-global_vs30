#
# Project-wide constants for Makefiles
#
# Edit this file as needed to produce the maps you want
#

#
# Set these to the directories where libnetcdf, the CDF include file,
# libgmt, and the GMT include files reside, respectively.
#
CDFLIB  = /usr/local/Cellar/netcdf/4.6.1_2/lib
CDFINC  = /usr/local/Cellar/netcdf/4.6.1_2/include
GMTLIB  = /usr/local/Cellar/gmt@4/4.5.17_2/lib
GMTINC  = /usr/local/Cellar/gmt@4/4.5.17_2/include

#
# CURL (or curl) utility. Many systems have this by default, others
# can easily install it with a package manager, or it can be had 
# from http://curl.haxx.se.
#
CURL=curl

#
# The path leading to the gdal binaries -- in particular,
# gdal_rasterize and gdal_translate
#
GDAL_PATH=/anaconda3/bin

#
# Sometimes the C programs in src will compile but die at runtime
# due to obscure, hard to diagnose dynamic library problems,
# usually having to do with -lgmt or -lnetcdf. If that happens
# you can try setting the STATIC macro below to "-static":
#
# STATIC = -static
#
STATIC =

#
# The output map resolution in arc-seconds (RES) and decimal degrees (RES_DD);
# both of these must represent the same resolution; the GMTED2010 is available 
# in 30, 15, and 7.5 arc second resolutions (0.008333333333, 0.004166666667, 
# 0.002083333333 degrees), so those are good choices; other resolutions will
# require makefile modifications.
# 30c makes a global file that is approximately 3GB
# 15c makes a global file that is approximately 12GB
# 7.5c makes a global file that is approximately 46GB 
# In all cases the disk space required to construct the map is at 
# least ten times the final file size (unless you go step by step and
# clean up as you go).
#
# NOTE: If you try to make the 7.5c map and run into memory problems
# (and you very likely will), see the file "commands_75.txt" in the 
# Slope directory for a way to make the map in two pieces and then
# stitch it together. If that's still too big for your machine, you
# can follow the pattern there and split the map into four or more
# pieces. I didn't build any of this into the Makefile because it's
# just too complicated and a couple of years from now everyone is 
# going to have a machine that can handle the full grid.
#
RES = 30
RES_DD = 0.00833333333333
#RES = 15
#RES_DD = 0.004166666667
#RES = 7.5
#RES_DD = 0.002083333333

#
# The region for which we have a DEM (GMTED2010); this defines
# the extent of the global map. You probably don't want to change
# this.
#
GXMIN = -180
GXMAX = 180
GYMIN = -56
GYMAX = 84
GLOBAL_REGION=$(GXMIN)/$(GXMAX)/$(GYMIN)/$(GYMAX)

#
# A region of the map covering the western US
#
WUS_REGION = -130.0/-113.0/31.0/50.0

#
# A region of the map covering the western US including Utah
#
BIG_WUS_REGION = -130.0/-107.0/31.0/50.0

#
# cpt files needed for plotting; these are relative to the regional
# map directories, not this top level directory
#
VS30_CPT = ../Misc/usa.cpt
WEIGHTS_CPT = ../Misc/weights.cpt

#
# The velocity assigned to water-covered areas
#
WATER = 600

#
# Taiwan map: Pick one or the other map to insert.
# According to Eric Thompson:
# --Taiwan_Vs30_AW09gk_30c.grd uses a baseline model from topographic slope
#   (Allen & Wald, 2009) and is then adjusted for the mean residual in each 
#   geologic unit and then the residuals are kriged.
# --Taiwan_Vs30_PRG_30c.grd uses a 'hybrid trend' with a slope that is allowed
#   to vary by geologic unit. This model is closest to the later work on 
#   California and elsewhere.
#
#TW_GRD_FILE = Taiwan_Vs30_AW09gk_30c.grd
TW_GRD_FILE = Taiwan_Vs30_PRG_30c.grd

#
# WARNING: The stuff below should only be edited if you really
# know what you're doing
#

#
# IRES is an integer version of RES. Normally it will be whatever
# RES is, but if RES is 7.5 we'll set it to 7, below, so that the
# -gt/-lt style tests in other Makfiles will work
#
IRES = $(RES)

#
# GRES is the resolution string that is found in the file names
# on the GMTED2010 site
#
GRES = $(RES)

#
# Depending on the resolution, we want to set the width of the
# smoothing filter (in grid points). Our defaults are to use
# smoothing filters for the regions of 0.7x0.7 degrees (but these
# are cut in half, to 0.35x0.35 degrees, by the processing), and
# for the global grid the smoothing from active tectonic to stable 
# craton is over 2x2 degrees. In both cases we subtract one grid 
# point to make the filter dimensions (in grid points) odd.
#
ifeq ($(RES), 7.5)
# IRES and GRES get reset to integers for 7.5 second resolution
IRES = 7
GRES = 75
REGION_FX = 339
REGION_FY = 339
GLOBE_FX = 959
GLOBE_FY = 959
else ifeq ($(RES), 15)
REGION_FX = 169
REGION_FY = 169
GLOBE_FX = 479
GLOBE_FY = 479
else ifeq ($(RES), 30)
REGION_FX = 85
REGION_FY = 85
GLOBE_FX = 239
GLOBE_FY = 239
else
$(error Unknown grid resolution -- you must specify filter sizes.)
endif

# Don't mess with this definition -- the blank lines are necessary
define N


endef

# End
