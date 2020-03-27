#
# Makefile for New England Vs30 grid
#
# Please read the note in the README and do not use this as a template for other regions with coastline,
# as some steps are not explained.
#

include ~/.vs30/Constants.mk

NE_WEST = -81.0
NE_EAST = -67.975
NE_NORTH = 47.45
NE_SOUTH = 34.025

NE_WEST_EXT = -82.0
NE_EAST_EXT = -66.975
NE_NORTH_EXT = 48.45
NE_SOUTH_EXT = 33.025

NE_BASE_REGION = $(NE_WEST)/$(NE_EAST)/$(NE_SOUTH)/$(NE_NORTH)

NE_EXT_REGION = $(NE_WEST_EXT)/$(NE_EAST_EXT)/$(NE_SOUTH_EXT)/$(NE_NORTH_EXT)

NEUS_REGION = -85/-65/30/50

.PHONY : all plots clean clean_plots veryclean plot_base_map \
         plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
         plot_weights plot_final_map_neus

IS_COARSER := $(shell [ $(IRES) -gt 29 ] && echo true)

all : new_england.grd weights.grd

plots : plot_original_map plot_base_map plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
        plot_weights plot_final_map_neus

clean : clean_plots
	$(RM) vs30_*c*.grd set_-99999_zero.grd landmask*.grd water*.grd water_600.grd new_england.grd \
              ne_*.grd clipmask.grd clipmask_smooth.grd mask_a.grd landmask_smooth.grd new_mask.grd new_mask_mul*.grd final_mask.grd \
              weights.grd north.grd south.grd west.grd east.grd gmt.history

clean_plots :
	$(RM) *.ps *.png
	$(RM) neus_vs30.grd

veryclean : clean

#################################################################################
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, 
# inside ranging from 0 to 0.5), multiply by 2 (making outside still < 0, 
# inside ranging from 0 to 1.0), and then keep only positive values (making 
# outside = 0, inside ranging from 0 to 1). 
#

weights.grd : final_mask.grd
	gmt grdmath -fg $< 0.5 SUB 2 MUL DUP 0 GT MUL = $@

##############################################################################
# This workflow is difficult to describe in writing, but check the Greece
# or Italy Makefiles for more info on what each step is doing.
#
# In this case, it's not going to perfectly mask the coastline because 
# grdlandmask isn't lining up perfectly with the barrier islands in
# the Gulf. When that happens, we'll end up with a value slightly 
# less than 1, i.e., it will be averaged slightly with the background
# Vs30 (slope) value. Not a huge deal since the "average" is still 
# very close to the new Vs30 (hybrid) value.
#

final_mask.grd : new_mask_mul_landmask_add_a.grd
	gmt grdmath $< DUP 0 GE MUL = $@

new_mask_mul_landmask_add_a.grd : mask_a.grd new_mask_mul_landmask.grd
	gmt grdmath mask_a.grd new_mask_mul_landmask.grd ADD 1 SUB = $@

new_mask_mul_landmask.grd : new_mask.grd landmask_land.grd
	gmt grdmath new_mask.grd landmask_land.grd MUL = $@

new_mask.grd : ne_non_zero.grd landmask_smooth.grd
	gmt grdmath ne_non_zero.grd DUP NOT landmask_smooth.grd MUL ADD = $@

landmask_smooth.grd : landmask_land.grd ../src/smooth
	../src/smooth infile=landmask_land.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

mask_a.grd : clipmask_smooth.grd landmask_land.grd
	gmt grdmath clipmask_smooth.grd landmask_land.grd MUL = $@

clipmask_smooth.grd : clipmask.grd ../src/smooth
	../src/smooth infile=clipmask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

clipmask.grd : landmask_water.grd ne_non_zero.grd
	gmt grdmath landmask_water.grd ne_non_zero.grd ADD 0 GT = $@

ne_non_zero.grd : new_england.grd
	gmt grdmath $< 0 GT = $@

###############################################################################
# Redefine a landmask that only cares about the oceans. Useful in this case
# since there are may differences between grdlandmask and actual lakes.
#

watermask.grd :
	gmt grdlandmask -V -N1/0/0/0/0 -R$(TX_BASE_REGION) -I$(RES)s -G$@ -Df

############################################################################################
# Two steps here: First, set all lakes that ARE recognized by grdlandmask equal to 600.
# Second, any lake values that are now 600 will overwrite all old lake values. 
#
# So, effectively we've set lakes = 600 twice now, once to capture all the lakes not
# recognized by grdlandmask (vs30_$(RES)c_lakes_600.grd) and again for the lakes that are 
# recognized by grdlandmask (water_600.grd/texas.grd). water_600.grd also sets the 
# Gulf of Mexico to 600. 
#

#texas.grd : water_600.grd set_-99999_zero.grd vs30_$(RES)c_lakes_600.grd
#	gmt grdmath water_600.grd 0 EQ set_-99999_zero.grd MUL water_600.grd ADD = $@

new_england.grd : ne_ext.grd landmask_water.grd
	gmt grdmath landmask_water.grd 600 MUL ne_ext.grd ADD = $@

############################
# Create land/water masks.
#

landmask_land.grd :
	gmt grdlandmask -V -N0/1/0/1/0 -R$(NE_EXT_REGION) -I$(RES)s -G$@ -Df

landmask_water.grd :
	gmt grdlandmask -V -N1/0/1/0/1 -R$(NE_EXT_REGION) -I$(RES)s -G$@ -Df

###################################
# Now set all NaN values to 0. 
#

set_NaN_zero.grd : ne_ext.grd
	gmt grdmath $< 0 AND = $@

#################################################################################
# Before doing anything, it's useful to set all the lakes that are 0 to begin with
# equal to 600. This took me a while to figure out but basically, the authors of 
# this map set all ocean water, land outside of Texas, and some large lakes to be
# -99999. However, some smaller lakes were set to 0. So, it's easiest to just 
# start by setting all 0 values to 600, then processing things somewhat normally.
#

#vs30_$(RES)c_lakes_600.grd : vs30_$(RES)c.grd
#	gmt grdmath $< 0 EQ 600 MUL $< ADD = $@


#################################################################################
# Add a buffer zone on all four sides on the New England initial map.
# Make the zone NaN just like it is now.
#

ne_ext.grd : vs30_$(RES)c_NaN_to_zero.grd north.grd south.grd west.grd east.grd
	gmt grdpaste vs30_$(RES)c_NaN_to_zero.grd north.grd -Gtmp.grd
	gmt grdpaste tmp.grd south.grd -Gne_ext.grd
	gmt grdpaste west.grd ne_ext.grd -Gtmp.grd
	gmt grdpaste tmp.grd east.grd -Gne_ext.grd
	$(RM) tmp.grd

north.grd :
	gmt grdmath -R$(NE_WEST)/$(NE_EAST)/$(NE_NORTH)/$(NE_NORTH_EXT) -I$(RES)s 0 = $@

south.grd :
	gmt grdmath -R$(NE_WEST)/$(NE_EAST)/$(NE_SOUTH_EXT)/$(NE_SOUTH) -I$(RES)s 0 = $@

west.grd :
	gmt grdmath -R$(NE_WEST_EXT)/$(NE_WEST)/$(NE_SOUTH_EXT)/$(NE_NORTH_EXT) -I$(RES)s 0 = $@

east.grd :
	gmt grdmath -R$(NE_EAST)/$(NE_EAST_EXT)/$(NE_SOUTH_EXT)/$(NE_NORTH_EXT) -I$(RES)s 0 = $@

#################################################################################
# First set all NaN values to zero (water and land outside NE)
#

vs30_$(RES)c_NaN_to_zero.grd : vs30_$(RES)c.grd
	gmt grdmath $< 0 AND = $@

#################################################################################
# Rescale to proper resolution. If the output is coarser than the
# input (~3c), then we filter with a 3x3 gaussian filter; if the
# output is finer than the input, we resample with bilinear interpolation
#

ifeq ($(IS_COARSER),true)
vs30_$(RES)c.grd : ne.grd
	gmt grdfilter -I$(RES)s/$(RES)s -R$(NE_WEST)/$(NE_EAST)/$(NE_SOUTH)/$(NE_NORTH) -D0 -Fg0.016 -fg $< -G$@
else
vs30_$(RES)c.grd : ne.grd
	gmt grdsample -nl+t0.1 -I$(RES)s/$(RES)s -R$(NE_WEST)/$(NE_EAST)/$(NE_SOUTH)/$(NE_NORTH) -fg $< -G$@
endif

################################################################################3
# Check for the file.
# 

ne.grd :
	echo "New England grid file ne.grd must be supplied."

../src/smooth :
	$(MAKE) -C ../src smooth

###################################################
# Plots
#

plot_original_map : original.png

plot_base_map : base.png

plot_vs30_landmask : new_england.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : landmask_smooth.png

plot_weights : weights.png

plot_final_map_neus : neus.png

# Set some flags for plotting. To change things for an individual map, 
# make sure to only change it in that particular code block below.

Jflags = M12		# Mercator projection, 12 cm across
Bflags = a2f1WSen	# Tick marks. Every 2 degrees deneoted with number, 1 degree tickmarks, print numbers on W and S axes
Dflags = 14/4.3/9/0.5	# Scalebar - position 14, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

# Plot original basemap

original.png : ne.grd
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -R$(NE_BASE_REGION) -B$(Bflags) -K > original.ps
	gmt psscale -D$(Dflags) -Ba -C$(NEW_VS30_CPT) -O -K >> original.ps
	gmt pscoast -J$(Jflags) -R$(NE_BASE_REGION) -Df -O -N1 -N2 -W >> original.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) original.ps

# Plot the resampled base Vs30 map

base.png : vs30_$(RES)c.grd
	gmt grdimage vs30_$(RES)c.grd -J$(Jflags) -C$(NEW_VS30_CPT) -R$(NE_BASE_REGION) -B$(Bflags) -K > base.ps
	gmt psscale -D$(Dflags) -Ba -C$(NEW_VS30_CPT) -O -K >> base.ps
	gmt pscoast -J$(Jflags) -R$(TX_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) base.ps

# Plot the landmask with NE Vs30s inserted:
 
new_england.png : new_england.grd
	gmt grdimage new_england.grd -J$(Jflags) -C$(NEW_VS30_CPT) -R$(NE_EXT_REGION) -B$(Bflags) -K > new_england.ps
	gmt psscale -D$(Dflags) -Ba -C$(NEW_VS30_CPT) -O -K >> new_england.ps
	gmt pscoast -J$(Jflags) -R$(NE_EXT_REGION) -Df -O -N1 -N2 -W >> new_england.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) new_england.ps
 
# Plot the clipping mask:

clipmask.png : clipmask.grd
	gmt grdimage clipmask.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -R$(NE_EXT_REGION) -B$(Bflags) -K > clipmask.ps
	gmt psscale -D$(Dflags) -Ba -C$(NEW_WEIGHTS_CPT) -O >> clipmask.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) clipmask.ps
 
# Plot the smoothed clipping mask:

landmask_smooth.png : landmask_smooth.grd
	gmt grdimage landmask_smooth.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -R$(NE_EXT_REGION) -B$(Bflags) -K > landmask_smooth.ps
	gmt psscale -D$(Dflags) -Ba -C$(NEW_WEIGHTS_CPT) -O -K >> landmask_smooth.ps
	gmt pscoast -J$(Jflags) -R$(NE_EXT_REGION) -Df -O -N1 -N2 -W >> landmask_smooth.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) landmask_smooth.ps

# Plot the weights:

weights.png : weights.grd
	gmt grdimage weights.grd -C$(WEIGHTS_CPT) -J$(Jflags) -R$(NE_EXT_REGION) -B$(Bflags) -K > weights.ps
	gmt psscale -D$(Dflags) -Ba -C$(WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -J$(Jflags) -R$(NE_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) weights.ps
 
# Plot the new Vs30 map in a cut out map of the southern US
 
neus.png : neus_vs30.grd
	gmt grdimage neus_vs30.grd -J$(Jflags) -C$(NEW_VS30_CPT) -R$(NEUS_REGION) -B$(Bflags) -K > neus.ps
	gmt psscale -D$(Dflags) -Ba -C$(NEW_VS30_CPT) -O -K >> neus.ps
	gmt pscoast -J$(Jflags) -R$(NEUS_REGION) -Df -O -N1 -N2 -W >> neus.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) neus.ps
	rm gmt.history

neus_vs30.grd : ../global_vs30.grd
	gmt grdcut -R$(NEUS_REGION) $< -G$@

