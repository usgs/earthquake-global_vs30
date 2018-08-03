#
# Makefile for PNW Vs30 grid into the global+CA grid
#
include ~/.vs30/Constants.mk

#
# MYEXT is this region's insert map extension. MYNAME is just for
# informational messages. MYEXT must be defined before the Regional.mk
# include file.
#
MYEXT = waor
MYNAME = Pacific Northwest

#
# Regional include; sets INGRD and OUTGRD.
#
include ../Regional.mk

PNW_BASE_REGION = -125.144158614/-116.462158614/41.995401663/49.083401663
PNW_EXT_REGION = -126/-116/41/50

.PHONY: all plots clean clean_plots veryclean plot_base_map plot_clip_mask \
        plot_smooth_clip_mask plot_clip_mask_2 plot_weights plot_final_map_wus

IS_COARSER := $(shell [ $(IRES) -gt 20 ] && echo true)

#
# Regional.mk has the target "all" in it:
#
# all : ../$(OUTGRD)
#
#
#../$(OUTGRD) : $(OUTGRD)
#	$(RM) $(INGRD)
#	cp $< ..
#

plots : plot_base_map plot_ext_map plot_clip_mask plot_smooth_clip_mask \
        plot_clip_mask_2 plot_weights plot_final_map_waor plot_final_map_wus

clean : clean_plots
	$(RM) orwash20c_600.grd $(MYEXT)_$(RES)c.grd north.grd south.grd east.grd \
              west.grd waor_ext.grd mask.grd \
              mask_smooth.grd mask_2.grd mask_smooth_2.grd \
              weights.grd 

clean_plots :
	$(RM) *.ps *.png
	$(RM) wus_vs30.grd waor_embedded.grd

veryclean : clean
	$(RM) global_vs30*_$(MYEXT).grd

#################################
#
# Run the insert program; this will insert the PNW map into the global grid 
# that already has the CA map inserted:
#
$(OUTGRD) : $(INGRD) $(MYEXT)_$(RES)c.grd weights.grd ../src/insert_grd
	../src/insert_grd grid1=$(INGRD) grid2=$(MYEXT)_$(RES)c.grd gout=$(OUTGRD) gmask=weights.grd

#
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, 
# inside ranging from 0 to 0.5), multiply by 2 (making outside still < 0, 
# inside ranging from 0 to 1.0), and then keep only positive values (making 
# outside = 0, inside ranging from 0 to 1). 
#
weights.grd : mask_smooth_2.grd
	gmt grdmath -fg mask_smooth_2.grd 0.5 SUB 2 MUL DUP 0 GT MUL = weights.grd

#
# This is the smoothed grid, but it has anomalies in it from places where 
# the Vs30 grid is 600 inside the borders.  To fix this, we 
# re-binary (?) the mask, then smooth it again:
#

mask_smooth_2.grd : mask_2.grd ../src/smooth
	../src/smooth infile=mask_2.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=mask_smooth_2.grd

mask_2.grd : mask_smooth.grd
	gmt grdmath mask_smooth.grd 0.5 GE = mask_2.grd

#
# Smooth the mask. This will blur the border, but we'll fix that in a 
# minute. We make the filter twice the width and height that we want (for the
# same reason). Let's try a 0.35 degree transition, which means a 0.7 degree 
# filter, which is 84 grid points, but we want an odd number. That means 
# fx = fy = 85:
#
mask_smooth.grd : mask.grd ../src/smooth
	../src/smooth infile=mask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=mask_smooth.grd

#
# The PNW Vs30 is now 600 everywhere outside of known geology. So now we
# make a mask in which 600 -> 0, and everything else -> 1.
#
mask.grd : $(MYEXT)_$(RES)c.grd 
	gmt grdmath $(MYEXT)_$(RES)c.grd $(WATER) NEQ = mask.grd

#
# Rescale to the proper resolution and shift the map to make it co-register
# with the global grid. For a coarser grid, we want to filter, but we also
# want to preserve the quantized Vs30 values, so we use a 3x3 median filter, 
# which preserves most (but for some reason not all) of the Vs30 values.
# For a finer grid, we just use grdsample with a nearest-neighbor sampling.
#
ifeq ($(IS_COARSER),true)
$(MYEXT)_$(RES)c.grd : waor_ext.grd
	gmt grdfilter -I$(RES)s -R$(PNW_EXT_REGION) -D0 -Fm0.016 waor_ext.grd -G$(MYEXT)_$(RES)c.grd
else
$(MYEXT)_$(RES)c.grd : waor_ext.grd
	gmt grdsample -nn -I$(RES)s -fg -R$(PNW_EXT_REGION) waor_ext.grd -G$(MYEXT)_$(RES)c.grd
endif

#
# In this map, 600 is used as water/unknown geology. The map is kind of tight
# so we paste a 1-degree chunk of 600s to each side:
#
waor_ext.grd : orwash20c_600.grd north.grd south.grd west.grd east.grd
	gmt grdpaste orwash20c_600.grd north.grd -Gtmp.grd
	gmt grdpaste tmp.grd south.grd -Gwaor_ext.grd
	gmt grdpaste west.grd waor_ext.grd -Gtmp.grd
	gmt grdpaste tmp.grd east.grd -Gwaor_ext.grd
	$(RM) tmp.grd

north.grd :
	gmt grdmath -R-125.144158614/-116.462158614/49.083401663/50.083401663 -I20s $(WATER) = north.grd

south.grd :
	gmt grdmath -R-125.144158614/-116.462158614/40.995401663/41.995401663 -I20s $(WATER) = south.grd

west.grd :
	gmt grdmath -R-126.144158614/-125.144158614/40.995401663/50.083401663 -I20s $(WATER) = west.grd

east.grd :
	gmt grdmath -R-116.462158614/-115.462158614/40.995401663/50.083401663 -I20s $(WATER) = east.grd

#
# This grid uses NaN as water and unknown gelolgy, but we want 600, so 
# we fix that
#
orwash20c_600.grd : orwash20c.grd
	gmt grdmath orwash20c.grd 600 AND = orwash20c_600.grd

orwash20c.grd :
	echo "Washington/Oregon grid file orwash20c.grd must be supplied."

###################################
#
# Plots
#
plot_base_map : base.png

plot_ext_map : ext.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : clipmask_smooth.png

plot_clip_mask_2 : clipmask_2.png

plot_weights : weights.png

plot_final_map_waor : waor.png

plot_final_map_wus : wus.png

# Plot the base Vs30 Map:
 
base.png : orwash20c.grd
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba4d/a2eWSen -K > base.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> base.ps
	gmt pscoast -JM12 -R$(PNW_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	convert -trim -density 300x300 -rotate 90 base.ps base.png

# Plot the extended Vs30 map:

ext.png : $(MYEXT)_$(RES)c.grd 
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba2f1WSen -K > ext.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> ext.ps
	gmt pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> ext.ps
	convert -trim -density 300x300 -rotate 90 ext.ps ext.png
 
# Plot the clipping mask:
 
clipmask.png : mask.grd
	gmt grdimage mask.grd -JM12 -C$(WEIGHTS_CPT) -Ba2f1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps clipmask.png
 
# Plot the smoothed clipping mask:
 
clipmask_smooth.png : mask_smooth.grd
	gmt grdimage mask_smooth.grd -JM12 -C$(WEIGHTS_CPT) -Ba2f1WSen -K > clipmask_smooth.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> clipmask_smooth.ps
	gmt pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask_smooth.ps
	convert -trim -rotate 90 clipmask_smooth.ps clipmask_smooth.png
 
# Plot the rescaled clipping mask.
 
clipmask_2.png : mask_2.grd
	gmt grdimage mask_2.grd -JM12 -C$(WEIGHTS_CPT) -Ba2f1WSen -K > clipmask_2.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> clipmask_2.ps
	gmt pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask_2.ps
	convert -trim -rotate 90 clipmask_2.ps clipmask_2.png
 
# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba2f1WSen -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 weights.ps weights.png
 
# Plot the new Vs30 map in the western US region:
 
wus.png : wus_vs30.grd
	gmt grdimage wus_vs30.grd -JM12 -C$(VS30_CPT) -Ba4f2WSen -K > wus.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> wus.ps
	gmt pscoast -JM12 -R$(WUS_REGION) -O -Df -N1 -N2 -W >> wus.ps
	convert -trim -density 300x300 -rotate 90 wus.ps wus.png

wus_vs30.grd : ../$(OUTGRD)
	gmt grdcut -R$(WUS_REGION) $< -G$@
 
# Plot the new Vs30 map at the original scale:
 
waor.png : waor_embedded.grd
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba2f1WSen -K > waor.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> waor.ps
	gmt pscoast -JM12 -R$(PNW_BASE_REGION) -O -Df -N1 -N2 -W >> waor.ps
	convert -trim -density 300x300 -rotate 90 waor.ps waor.png

waor_embedded.grd : ../$(OUTGRD)
	gmt grdcut -R$(PNW_BASE_REGION) $< -G$@
