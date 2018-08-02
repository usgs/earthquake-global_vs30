#
# Makefile for California Vs30 grid
#

#
# Include the boilerplate
#
include ~/.vs30/Constants.mk

#
# MYEXT is this region's insert map extension. MYNAME is just for
# informational messages. MYEXT must be defined before the Regional.mk
# include file.
#
MYEXT = ca
MYNAME = California

#
# Regional include; sets INGRD and OUTGRD.
#
include ../Regional.mk

CA_BASE_REGION = -124.475/-114.025/32.05/41.9916666667
CA_EXT_REGION = -124.475/-114.025/32.05/43.0

.PHONY : all plots clean clean_plots veryclean plot_base_map \
         plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
         plot_clip_mask_2 plot_weights plot_final_map_wus

#
# Regional.mk has the targets "all" and ../$(OUTGRD) in it:
#
# all : ../$(OUTGRD)
#
#
#../$(OUTGRD) : $(OUTGRD)
#	$(RM) $(INGRD)
#	cp $< ..
#

IS_COARSER := $(shell [ $(IRES) -gt 20 ] && echo true)

plots : plot_base_map plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
        plot_clip_mask_2 plot_weights plot_final_map_wus

clean : clean_plots
	$(RM) vs30_$(RES)c.grd top.grd vs30_$(RES)c_ext.grd landmask.grd \
              vs30_ext_h2o600.grd mask.grd mask_smooth.grd mask_2.grd \
              mask_smooth_2.grd weights.grd

clean_plots :
	$(RM) *.ps *.png
	$(RM) wus_vs30.grd

veryclean : clean
	$(RM) global_vs30*_$(MYEXT).grd

#
# Insert the CA grid into the global grid found in the parent
# directory
#
$(OUTGRD) : ../src/insert_grd $(INGRD) vs30_ext_h2o600.grd weights.grd
	../src/insert_grd grid1=$(INGRD) grid2=vs30_ext_h2o600.grd gmask=weights.grd gout=$@

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

#
# This is the smoothed grid, but it has anomalies in it from places where 
# the Vs30 grid had NaNs that were not coincident with water in the GMT 
# land mask. There also appear to be areas where there were non-NaN values 
# outside the CA border. To fix this, we re-binary (?) the mask, then smooth 
# it again:
#
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
mask.grd : vs30_ext_h2o600.grd
	gmt grdmath -fg $< 0 GT = $@

#
# The CA Vs30 is NaN in water (including lakes, rivers, etc) and outside CA. 
# So make a landmask wet=1 dry = 0. Then set all the water areas to 600 and 
# do an AND (NaN if A and B == NaN, B if A == NaN, else A) with the Vs30 
# grid -- that will leave wet = 600, dry areas inside CA with the Vs30, and 
# dry outside CA as 0.
#
vs30_ext_h2o600.grd : vs30_$(RES)c_ext.grd landmask.grd
	gmt grdmath vs30_$(RES)c_ext.grd landmask.grd 600 MUL AND = $@

landmask.grd : 
	gmt grdlandmask -V -R$(CA_EXT_REGION) -I$(RES)s -G$@ -Df -N1/0/0/0/0

#
# Paste a 1-degree chunk of NaNs to the top of the map:
#
vs30_$(RES)c_ext.grd : vs30_$(RES)c.grd top.grd
	gmt grdpaste vs30_$(RES)c.grd top.grd -G$@

top.grd : 
	gmt grdmath -R-124.475/-114.025/41.9916666667/43.0 -I$(RES)s 0 0 NAN = top.grd

#
# Rescale to proper resolution. If the output is coarser than the
# input (20c), then we filter with a 3x3 gaussian filter; if the
# output is finer than the input, we resample with bilinear interpolation
#

ifeq ($(IS_COARSER),true)
vs30_$(RES)c.grd : Vs30_Cal_hyb_KT_ml_20c_fill_Ql.grd
	gmt grdfilter -I$(RES)s -D0 -Fg0.016 $< -G$@
else
vs30_$(RES)c.grd : Vs30_Cal_hyb_KT_ml_20c_fill_Ql.grd
	gmt grdsample -nl+t0.1 -I$(RES)s -fg $< -G$@
endif

Vs30_Cal_hyb_KT_ml_20c_fill_Ql.grd :
	echo "California grid file Vs30_Cal_hyb_KT_ml_20c_fill_Ql.grd must be supplied."

###################################################
#
# Plots
#
plot_base_map : base.png

plot_vs30_landmask : landmask.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : landmask_smooth.png

plot_clip_mask_2 : clipmask_2.png

plot_weights : weights.png

plot_final_map_wus : wus.png

# Plot the resampled base Vs30 map

base.png : vs30_$(RES)c.grd
	gmt grdimage vs30_$(RES)c.grd -JM12 -C$(VS30_CPT) -Ba2f1WSen -K > base.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> base.ps
	gmt pscoast -JM12 -R$(CA_BASE_REGION) -O -N1 -N2 -W >> base.ps
	convert -rotate 90 base.ps base.png
 
# Plot the landmask with CA Vs30s inserted:
 
landmask.png : vs30_ext_h2o600.grd
	gmt grdimage vs30_ext_h2o600.grd -JM12 -C$(VS30_CPT) -Ba2f1WSen -K > landmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> landmask.ps
	gmt pscoast -JM12 -R$(CA_EXT_REGION) -O -N1 -N2 -W >> landmask.ps
	convert -rotate 90 landmask.ps landmask.png
 
# Plot the clipping mask:

clipmask.png : mask.grd
	gmt grdimage mask.grd -JM12 -C$(WEIGHTS_CPT) -Ba2f1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O >> clipmask.ps
	convert -rotate 90 clipmask.ps clipmask.png
 
# Plot the smoothed clipping mask:

landmask_smooth.png : mask_smooth.grd
	gmt grdimage mask_smooth.grd -JM12 -C$(WEIGHTS_CPT) -Ba2f1WSen -K > landmask_smooth.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> landmask_smooth.ps
	gmt pscoast -JM12 -R$(CA_EXT_REGION) -O -N1 -N2 -W >> landmask_smooth.ps
	convert -rotate 90 landmask_smooth.ps landmask_smooth.png
 
# Plot the rescaled clipping mask.

clipmask_2.png : mask_2.grd
	gmt grdimage mask_2.grd -JM12 -C$(WEIGHTS_CPT) -Ba2f1WSen -K > clipmask_2.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> clipmask_2.ps
	gmt pscoast -JM12 -R$(CA_EXT_REGION) -O -N1 -N2 -W >> clipmask_2.ps
	convert -rotate 90 clipmask_2.ps clipmask_2.png
 
# Plot the weights:

weights.png : weights.grd
	gmt grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba2f1WSen -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -JM12 -R$(CA_EXT_REGION) -O -N1 -N2 -W >> weights.ps
	convert -rotate 90 weights.ps weights.png
 
# Plot the new Vs30 map in a cut out map of the western US
 
wus.png : wus_vs30.grd
	gmt grdimage wus_vs30.grd -JM12 -C$(VS30_CPT) -Ba2f1WSen -K > wus.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> wus.ps
	gmt pscoast -JM12 -R$(WUS_REGION) -O -N1 -N2 -W >> wus.ps
	convert -rotate 90 wus.ps wus.png

wus_vs30.grd : ../$(OUTGRD)
	gmt grdcut -R$(WUS_REGION) $< -G$@

