#
# Makefile for PNW Vs30 grid into the global+CA grid
#
include ../Constants.mk

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

PNW_BASE_REGION = -124.925/-116.45/42/49
PNW_EXT_REGION = -125.925/-115.45/41/50

.PHONY: all plots clean clean_plots veryclean plot_base_map plot_clip_mask \
        plot_smooth_clip_mask plot_clip_mask_2 plot_weights plot_final_map_wus

#
# Regional.mk has the target "all" in it:
#
# all : ../$(OUTGRD)
#

../$(OUTGRD) : $(OUTGRD)
	$(RM) $(INGRD)
	cp $< ..

plots : plot_base_map plot_clip_mask plot_smooth_clip_mask \
        plot_clip_mask_2 plot_weights plot_final_map_wus

clean : clean_plots
	$(RM) waor45_600.grd $(MYEXT)_$(RES)c.grd north.grd south.grd east.grd \
              west.grd waor_ext.grd landmask.grd mask.grd \
              mask_smooth.grd mask_2.grd mask_smooth_2.grd \
              weights.grd 

clean_plots :
	$(RM) *.ps *.png
	$(RM) wus_vs30.grd

veryclean : clean
	$(RM) global_vs30*_$(MYEXT).grd

#################################
#
# Run the insert program; this will insert the PNW map into the global grid that
# already has the CA map inserted:
#
$(OUTGRD) : $(INGRD) waor_ext.grd weights.grd \
            ../src/insert_grd insert_waor.par
	../src/insert_grd par=insert_waor.par grid1=$(INGRD) gout=$(OUTGRD)

#################################
#
# This is the fallback if we don't find ../global_vs30*.grd.
#
../global_vs30.grd : ../Slope/global_vs30.grd
	cp $< $@

../Slope/global_vs30.grd :
	$(MAKE) -C ../Slope global_vs30.grd

#
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, inside 
# ranging from 0 to 0.5), multiply by 2 (making outside still < 0, inside ranging 
# from 0 to 1.0), and then keep only positive values (making outside = 0, inside 
# ranging from 0 to 1). 
#
weights.grd : mask_smooth_2.grd
	grdmath -fg mask_smooth_2.grd 0.5 SUB 2 MUL DUP 0 GT MUL = weights.grd

#
# This is the smoothed grid, but it has anomalies in it from places where the Vs30
# grid had lakes inside the state borders.  To fix this, we re-binary (?) the mask, then 
# smooth it again:
#

mask_smooth_2.grd : mask_2.grd ../src/smooth smooth2.par
	../src/smooth par=smooth2.par

mask_2.grd : mask_smooth.grd
	grdmath mask_smooth.grd 0.5 GE = mask_2.grd

#
# Smooth the mask. This will blur the border, but we'll fix that in a 
# minute. We make the filter twice the width and height that we want (for the
# same reason). Let's try a 0.35 degree transition, which means a 0.7 degree 
# filter, which is 84 grid points, but we want an odd number. That means 
# fx = fy = 85:
#
mask_smooth.grd : mask.grd ../src/smooth smooth1.par
	../src/smooth par=smooth1.par

#
# The PNW Vs30 is now 600 in water (including lakes, rivers, etc) and outside WA/OR. So 
# make a landmask wet=1 dry = 0. Then, in another grid, set all the non-600 areas to 1. 
# Then add the two together and set > 1 to 1.
#
mask.grd : landmask.grd waor_ext.grd 
	grdmath landmask.grd waor_ext.grd $(WATER) NEQ ADD 0 GT = mask.grd

landmask.grd :
	grdlandmask -V -R-125.925/-115.45/41/50 -I$(RES)c -Glandmask.grd -Df -N1/0/0/0/0

#
# In this map, 600 is used as water/unknown geology. The map is kind of tight
# so we paste a 1-degree chunk of 600s to each side:
#
waor_ext.grd : $(MYEXT)_$(RES)c.grd north.grd south.grd west.grd east.grd
	grdpaste $(MYEXT)_$(RES)c.grd north.grd -Gtmp.grd
	grdpaste tmp.grd south.grd -Gwaor_ext.grd
	grdpaste west.grd waor_ext.grd -Gtmp.grd
	grdpaste tmp.grd east.grd -Gwaor_ext.grd
	$(RM) tmp.grd

north.grd :
	grdmath -R-124.925/-116.45/49/50 -I$(RES)c $(WATER) = north.grd

south.grd :
	grdmath -R-124.925/-116.45/41/42 -I$(RES)c $(WATER) = south.grd

west.grd :
	grdmath -R-125.925/-124.925/41/50 -I$(RES)c $(WATER) = west.grd

east.grd :
	grdmath -R-116.45/-115.45/41/50 -I$(RES)c $(WATER) = east.grd

#
# Rescale to 30-second resolution and shift the map to make it co-register
# with the global grid.
#
$(MYEXT)_$(RES)c.grd : waor45_600.grd
	grdsample -Ql0.1 -I$(RES)c -fg -R$(PNW_BASE_REGION) waor45_600.grd -G$(MYEXT)_$(RES)c.grd

#
# This grid uses 603 as water, but we want 600, so we fix that
#
waor45_600.grd : waor45.grd
	grdmath waor45.grd DUP 603 EQ 3 MUL SUB = waor45_600.grd

waor45.grd :
	echo "Washington/Oregon grid file waor45.grd must be supplied."

###################################
../src/smooth :
	$(MAKE) -C ../src make smooth

../src/insert_grd :
	$(MAKE) -C ../src make insert_grd


plot_base_map : base.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : landmask_smooth.png

plot_clip_mask_2 : clipmask_2.png

plot_weights : weights.png

plot_final_map_wus : wus.png

# Plot the base Vs30 Map:
 
base.png : $(MYEXT)_$(RES)c.grd
	grdimage $< -JM12 -C$(VS30_CPT) -Ba4d/a2eWSen -K > base.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> base.ps
	pscoast -JM12 -R$(PNW_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	convert -trim -density 300x300 -rotate 90 base.ps base.png
 
# Plot the clipping mask:
 
clipmask.png : mask.grd
	grdimage mask.grd -JM12 -C$(WEIGHTS_CPT) -Ba4d/a2eWSen -K > clipmask.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> clipmask.ps
	pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps clipmask.png
 
# Plot the smoothed clipping mask:
 
landmask_smooth.png : mask_smooth.grd
	grdimage mask_smooth.grd -JM12 -C$(WEIGHTS_CPT) -Ba4d/a2eWSen -K > landmask_smooth.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> landmask_smooth.ps
	pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> landmask_smooth.ps
	convert -trim -rotate 90 landmask_smooth.ps landmask_smooth.png
 
# Plot the rescaled clipping mask.
 
clipmask_2.png : mask_2.grd
	grdimage mask_2.grd -JM12 -C$(WEIGHTS_CPT) -Ba4d/a2eWSen -K > clipmask_2.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> clipmask_2.ps
	pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask_2.ps
	convert -trim -rotate 90 clipmask_2.ps clipmask_2.png
 
# Plot the weights:
 
weights.png : weights.grd
	grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba4dg2d/a2eg2dWSen -K > weights.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps     
	pscoast -JM12 -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 weights.ps weights.png
 
# Plot the new Vs30 map:
 
wus.png : wus_vs30.grd
	grdimage wus_vs30.grd -JM12 -C$(VS30_CPT) -Ba6d/a4eWSen -K > wus.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> wus.ps
	pscoast -JM12 -R$(WUS_REGION) -O -Df -N1 -N2 -W >> wus.ps
	convert -trim -density 300x300 -rotate 90 wus.ps wus.png

wus_vs30.grd : global_vs30*_$(MYEXT).grd
	grdcut -R$(WUS_REGION) $< -G$@
