#
# Makefile for California Vs30 grid
#

#
# Include the boilerplate
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
         plot_clip_mask_2 plot_weights plot_final_map_wus

IS_COARSER := $(shell [ $(IRES) -gt 7 ] && echo true)

all : california.grd weights.grd

plots : plot_original_map plot_base_map plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
        plot_clip_mask_2 plot_weights plot_final_map_wus

clean : clean_plots
	$(RM) vs30_*c.grd top.grd vs30_*c_ext.grd landmask.grd \
              mask.grd mask_smooth.grd mask_2.grd east.grd \
              mask_smooth_2.grd weights.grd California_Vs30_7p5c_clean.grd \
              vs30_*c_top_bot.grd vs30_*c_top.grd california.grd bot.grd \
              California_Vs30_7p5c_non_zero.grd non_zero.grd non_zero_2.grd non_zero_new.grd

clean_plots :
	$(RM) *.ps *.png
	$(RM) wus_vs30.grd

veryclean : clean

#################################
#
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, 
# inside ranging from 0 to 0.5), multiply by 2 (making outside still < 0, 
# inside ranging from 0 to 1.0), and then keep only positive values (making 
# outside = 0, inside ranging from 0 to 1). 
#

weights.grd : mask_smooth_2.grd
	gmt grdmath -fg $< 0.5 SUB 2 MUL DUP 0 GT MUL = $@


############
# Trial

mask_smooth_2.grd : mask_2.grd ../src/smooth
	../src/smooth infile=mask_2.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

mask_2.grd : mask_smooth.grd
	gmt grdmath $< 0.5 GE = $@


#
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

#
# The CA Vs30 is 0 in water (including lakes, rivers, etc) and outside CA. 
# So make a landmask wet=1 dry = 0. Then multiply by 600 to set all the water areas to 600 and 
# do an ADD with the new Vs30 grid -- that will leave wet = 600, dry areas inside CA with the new Vs30, 
# and dry outside CA as 0.
#

california.grd : vs30_$(RES)c_ext.grd landmask.grd
	gmt grdmath vs30_$(RES)c_ext.grd landmask.grd 600 MUL ADD = $@

landmask.grd : 
	gmt grdlandmask -V -R$(CA_EXT_REGION) -A4 -I$(RES)s/$(RES)s -G$@ -Df -N1/0/1/0/1

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

#
# Rescale to proper resolution. If the output is coarser than the
# input (7.5c), then we filter with a 3x3 gaussian filter; if the
# output is finer than the input, we resample with bilinear interpolation
#

ifeq ($(IS_COARSER),true)
vs30_$(RES)c.grd : California_Vs30_7p5c_clean.grd
	gmt grdfilter -I$(RES)s/$(RES)s -R$(CA_WEST)/$(CA_EAST)/$(CA_SOUTH)/$(CA_NORTH) -T -D0 -Fg0.016 -fg $< -G$@
else
vs30_$(RES)c.grd : California_Vs30_7p5c_clean.grd
	gmt grdsample -nl+t0.1 -I$(RES)s -fg $< -G$@
endif

#
# Start by noticing that lakes are -1. Raise the floor so that all values are 0 or greater.
# 

California_Vs30_7p5c_clean.grd : California_Vs30_7p5c_non_zero.grd non_zero_2.grd
	gmt grdmath California_Vs30_7p5c_non_zero.grd non_zero_2.grd AND -fg = $@

non_zero_2.grd : California_Vs30_7p5c_non_zero.grd
	gmt grdmath California_Vs30_7p5c_non_zero.grd 0 GE -fg = $@

California_Vs30_7p5c_non_zero.grd : California_Vs30_7p5c.grd non_zero.grd
	gmt grdmath California_Vs30_7p5c.grd non_zero.grd MUL -fg = $@ 

non_zero.grd : California_Vs30_7p5c.grd
	gmt grdmath California_Vs30_7p5c.grd 0 GE -fg = $@

#
# Check for the file.
# 

California_Vs30_7p5c.grd :
	echo "California grid file California_Vs30_7p5c.grd must be supplied."

../src/smooth :
	$(MAKE) -C ../src smooth

###################################################
#
# Plots
#
plot_original_map : original.png

plot_base_map : base.png

plot_vs30_landmask : landmask.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : landmask_smooth.png

plot_clip_mask_2 : landmask_smooth_2.png

plot_weights : weights.png

plot_final_map_wus : wus.png

# Plot original basemap

original.png : California_Vs30_7p5c_clean.grd
	gmt grdimage California_Vs30_7p5c_clean.grd -JM12 -C$(NEW_VS30_CPT) -Ba2f1WSen -K > original.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> original.ps
	gmt pscoast -JM12 -R$(CA_BASE_REGION) -O -N1 -N2 -W >> original.ps
	convert -rotate 90 original.ps original.png

# Plot the resampled base Vs30 map

base.png : vs30_$(RES)c.grd
	gmt grdimage vs30_$(RES)c.grd -JM12 -C$(NEW_VS30_CPT) -Ba2f1WSen -K > base.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> base.ps
	gmt pscoast -JM12 -R$(CA_BASE_REGION) -O -N1 -N2 -W >> base.ps
	convert -rotate 90 base.ps base.png
 
# Plot the landmask with CA Vs30s inserted:
 
landmask.png : california.grd
	gmt grdimage california.grd -JM12 -C$(NEW_VS30_CPT) -Ba2f1WSen -K > landmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> landmask.ps
	gmt pscoast -JM12 -R$(CA_EXT_REGION) -O -N1 -N2 -W >> landmask.ps
	convert -rotate 90 landmask.ps landmask.png
 
# Plot the clipping mask:

clipmask.png : mask.grd
	gmt grdimage mask.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba2f1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O >> clipmask.ps
	convert -rotate 90 clipmask.ps clipmask.png
 
# Plot the smoothed clipping mask:

landmask_smooth.png : mask_smooth.grd
	gmt grdimage mask_smooth.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba2f1WSen -K > landmask_smooth.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> landmask_smooth.ps
	gmt pscoast -JM12 -R$(CA_EXT_REGION) -O -N1 -N2 -W >> landmask_smooth.ps
	convert -rotate 90 landmask_smooth.ps landmask_smooth.png
 
landmask_smooth_2.png : mask_smooth_2.grd
	gmt grdimage mask_smooth_2.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba2f1WSen -K > landmask_smooth_2.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> landmask_smooth_2.ps
	gmt pscoast -JM12 -R$(CA_EXT_REGION) -O -N1 -N2 -W >> landmask_smooth_2.ps
	convert -rotate 90 landmask_smooth_2.ps landmask_smooth_2.png

# Plot the weights:

weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -JM12 -Ba2f1WSen -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -JM12 -R$(CA_EXT_REGION) -O -N1 -N2 -W >> weights.ps
	convert -rotate 90 weights.ps weights.png
 
# Plot the new Vs30 map in a cut out map of the western US
 
wus.png : wus_vs30.grd
	gmt grdimage wus_vs30.grd -JM12 -C$(VS30_CPT) -Ba2f1WSen -K > wus.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> wus.ps
	gmt pscoast -JM12 -R$(WUS_REGION) -O -N1 -N2 -W >> wus.ps
	convert -rotate 90 wus.ps wus.png

wus_vs30.grd : $(OUTGRD)
	gmt grdcut -R$(WUS_REGION) $< -G../$@

