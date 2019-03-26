#
# Makefile for California Vs30 grid
#
# Please read the note in the README and do not use this as a template for other regions with coastline.
# This DOES, however, fix a unique problem where lakes were given a unique value different 
# to the value of oceans. It also fixes another problem with SF Bay. The solutions could be useful
# for other maps, but it's a pretty unique problem I'd guess. 
#

include ~/.vs30/Constants.mk

CA_WEST = -124.5
CA_EAST = -114.1
CA_NORTH = 42.1
CA_SOUTH = 32.5

CA_BASE_REGION = $(CA_WEST)/$(CA_EAST)/$(CA_SOUTH)/$(CA_NORTH)

CA_EXT_EAST = -113.1
CA_EXT_NORTH = 43.1
CA_EXT_SOUTH = 31.5

CA_EXT_REGION = $(CA_WEST)/$(CA_EXT_EAST)/$(CA_EXT_SOUTH)/$(CA_EXT_NORTH)

.PHONY : all plots clean clean_plots veryclean plot_base_map \
         plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
         plot_weights plot_final_map_wus

IS_COARSER := $(shell [ $(IRES) -gt 7 ] && echo true)

all : california.grd weights.grd

plots : plot_original_map plot_base_map plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
        plot_weights plot_final_map_wus

clean : clean_plots
	$(RM) vs30_*c.grd vs30_*c_ext.grd vs30_*c_top_bot.grd vs30_*c_top.grd landmask.grd \
              mask.grd mask_smooth.grd top.grd east.grd bot.grd \
              weights.grd california.grd lakes_600.grd california_SF_bay_1200.grd gmt.history

clean_plots :
	$(RM) *.ps *.png
	$(RM) wus_vs30.grd

veryclean : clean

#################################################################################
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, 
# inside ranging from 0 to 0.5), multiply by 2 (making outside still < 0, 
# inside ranging from 0 to 1.0), and then keep only positive values (making 
# outside = 0, inside ranging from 0 to 1). 
#

weights.grd : mask_smooth.grd
	gmt grdmath -fg $< 0.5 SUB 2 MUL DUP 0 GT MUL = $@

#################################################################################
# Smooth the mask. This will blur the border, but we'll fix that in a 
# minute. We make the filter twice the width and height that we want (for the
# same reason). Let's try a 0.35 degree transition, which means a 0.7 degree 
# filter, which is 84 grid points, but we want an odd number. That means 
# fx = fy = 85:
#

mask_smooth.grd : mask.grd ../src/smooth 
	../src/smooth infile=mask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

#
# Make a mask where inside the borders = 1, outside = 0 (note that we
# are not considering the coastline a "border".
#

mask.grd : california.grd
	gmt grdmath -fg $< 0 GT = $@

################################################################################################################################3
# The CA Vs30 is 0 in the oceans and outside CA, and -1 in lakes. First make all -1 vales 600. 
# Next make a landmask where oceans=1, dry = 0. Then multiply by 600 to set all the oceans to 600 and 
# do an ADD with the new Vs30 grid -- that will leave wet (oceans and lakes) = 600, dry areas inside CA with the new Vs30, 
# and dry outside CA as 0. Last, we fix SF Bay which was made 1200 by the ADD. Since it is the only 1200 value in the grid we
# set all 1200 values to 600.
#

california.grd : california_SF_bay_1200.grd
	gmt grdmath $< DUP 1200 EQ 600 MUL SUB = $@

california_SF_bay_1200.grd : lakes_600.grd landmask.grd
	gmt grdmath lakes_600.grd landmask.grd 600 MUL ADD = $@

landmask.grd : 
	gmt grdlandmask -V -R$(CA_EXT_REGION) -I$(RES)s/$(RES)s -G$@ -Df -N1/0/0/0/0

lakes_600.grd : vs30_$(RES)c_ext.grd
	gmt grdmath $< DUP 0 LT 601 MUL ADD = $@

#
# Paste a 1-degree chunk of zeros to the top, bottom, and east side of the map:
#

vs30_$(RES)c_ext.grd : vs30_$(RES)c_top_bot.grd east.grd
	gmt grdpaste vs30_$(RES)c_top_bot.grd east.grd -G$@

vs30_$(RES)c_top_bot.grd : vs30_$(RES)c_top.grd bot.grd
	gmt grdpaste vs30_$(RES)c_top.grd bot.grd -G$@

vs30_$(RES)c_top.grd : vs30_$(RES)c.grd top.grd
	gmt grdpaste vs30_$(RES)c.grd top.grd -G$@

top.grd : 
	gmt grdmath -R$(CA_WEST)/$(CA_EAST)/$(CA_NORTH)/$(CA_EXT_NORTH) -I$(RES)s/$(RES)s 0 1 NAN = $@

bot.grd :
	gmt grdmath -R$(CA_WEST)/$(CA_EAST)/$(CA_EXT_SOUTH)/$(CA_SOUTH) -I$(RES)s/$(RES)s 0 1 NAN = $@

east.grd :
	gmt grdmath -R$(CA_EAST)/$(CA_EXT_EAST)/$(CA_EXT_SOUTH)/$(CA_EXT_NORTH) -I$(RES)s/$(RES)s 0 1 NAN = $@

#################################################################################
# Rescale to proper resolution. If the output is coarser than the
# input (7.5c), then we filter with a 3x3 gaussian filter; if the
# output is finer than the input, we resample with bilinear interpolation
#

ifeq ($(IS_COARSER),true)
vs30_$(RES)c.grd : California_Vs30_7p5c.grd
	gmt grdfilter -I$(RES)s/$(RES)s -R$(CA_WEST)/$(CA_EAST)/$(CA_SOUTH)/$(CA_NORTH) -T -D0 -Fg0.016 -fg $< -G$@
else
vs30_$(RES)c.grd : California_Vs30_7p5c.grd
	gmt grdsample -nl+t0.1 -I$(RES)s/$(RES)s -R$(CA_WEST)/$(CA_EAST)/$(CA_SOUTH)/$(CA_NORTH) -fg $< -G$@
endif

################################################################################3
# Check for the file.
# 

California_Vs30_7p5c.grd :
	echo "California grid file California_Vs30_7p5c.grd must be supplied."

../src/smooth :
	$(MAKE) -C ../src smooth

###################################################
# Plots
#

plot_original_map : original.png

plot_base_map : base.png

plot_vs30_landmask : california.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : landmask_smooth.png

plot_weights : weights.png

plot_final_map_wus : wus.png

# Set some flags for plotting. To change things for an individual map, 
# make sure to only change it in that particular code block below.

Jflags = M12		# Mercator projection, 12 cm across
Bflags = a2f1WSen	# Tick marks. Every 2 degrees deneoted with number, 1 degree tickmarks, print numbers on W and S axes
Dflags = 14/4.3/9/0.5	# Scalebar - position 14, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

# Plot original basemap

original.png : California_Vs30_7p5c.grd
	gmt grdimage California_Vs30_7p5c.grd -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > original.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> original.ps
	gmt pscoast -J$(Jflags) -R$(CA_BASE_REGION) -Df -O -N1 -N2 -W >> original.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) original.ps

# Plot the resampled base Vs30 map

base.png : vs30_$(RES)c.grd
	gmt grdimage vs30_$(RES)c.grd -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > base.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> base.ps
	gmt pscoast -J$(Jflags) -R$(CA_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) base.ps

# Plot the landmask with CA Vs30s inserted:
 
california.png : california.grd
	gmt grdimage california.grd -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > california.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> california.ps
	gmt pscoast -J$(Jflags) -R$(CA_EXT_REGION) -Df -O -N1 -N2 -W >> california.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) california.ps
 
# Plot the clipping mask:

clipmask.png : mask.grd
	gmt grdimage mask.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > clipmask.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O >> clipmask.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) clipmask.ps
 
# Plot the smoothed clipping mask:

landmask_smooth.png : mask_smooth.grd
	gmt grdimage mask_smooth.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > landmask_smooth.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> landmask_smooth.ps
	gmt pscoast -J$(Jflags) -R$(CA_EXT_REGION) -Df -O -N1 -N2 -W >> landmask_smooth.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) landmask_smooth.ps

# Plot the weights:

weights.png : weights.grd
	gmt grdimage weights.grd -C$(WEIGHTS_CPT) -J$(Jflags) -B$(Bflags) -K > weights.ps
	gmt psscale -D$(Dflags) -L -C$(WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -J$(Jflags) -R$(CA_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	gmt psconvert -E720 -P -TGg weights.ps
 
# Plot the new Vs30 map in a cut out map of the western US
 
wus.png : wus_vs30.grd
	gmt grdimage wus_vs30.grd -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > wus.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> wus.ps
	gmt pscoast -J$(Jflags) -R$(WUS_REGION) -Df -O -N1 -N2 -W >> wus.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) wus.ps
	rm gmt.history

wus_vs30.grd : ../global_vs30.grd
	gmt grdcut -R$(WUS_REGION) $< -G$@

