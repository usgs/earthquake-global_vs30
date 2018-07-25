#
# Makefile for Utah Vs30 grid into the global+regional grid
#
include ~/.vs30/Constants.mk

#
# MYEXT is this region's insert map extension. MYNAME is just for
# informational messages. MYEXT must be defined before the Regional.mk
# include file.
#
MYEXT = ut
MYNAME = Utah

#
# Regional include; sets INGRD and OUTGRD.
#
include ../Regional.mk

UTAH_BASE_REGION = -114.2/-108.9/36.8/42.4
UTAH_EXT_REGION = -114.7/-108.2/36.3/42.9

.PHONY: all plots clean clean_plots veryclean

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

plots : plot_base_map plot_clip_mask plot_smooth_clip_mask plot_weights \
	plot_final_map_wus plot_raw_map

clean : clean_plots
	$(RM) $(MYEXT)_$(RES)c.grd north.grd south.grd east.grd west.grd \
              ut_ext.grd mask.grd mask_smooth.grd weights.grd 

clean_plots :
	$(RM) *.ps *.png
	$(RM) wus_vs30.grd

veryclean : clean
	$(RM) global_vs30*_$(MYEXT).grd

#################################
#
# Run the insert program; this will insert the Utah map into the global grid 
# that may already have other regional maps inserted:
#
$(OUTGRD) : $(INGRD) ut_ext.grd weights.grd ../src/insert_grd 
	../src/insert_grd grid1=$(INGRD) grid2=ut_ext.grd gmask=weights.grd gout=$(OUTGRD)

#
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, 
# inside ranging from 0 to 0.5), multiply by 2 (making outside still < 0, 
# inside ranging from 0 to 1.0), and then keep only positive values (making 
# outside = 0, inside ranging from 0 to 1). 
#
weights.grd : mask_smooth.grd
	grdmath -fg mask_smooth.grd 0.5 SUB 2 MUL DUP 0 GT MUL = weights.grd

#
# Smooth the mask. This will blur the border, but we'll fix that in a 
# minute. We make the filter twice the width and height that we want (for the
# same reason). We're using a 0.35 degree transition, which means a 0.7 degree 
# filter, which is 84 grid points, but we want an odd number. That means 
# fx = fy = 85:
#
mask_smooth.grd : mask.grd ../src/smooth 
	../src/smooth infile=mask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=mask_smooth.grd

#
# Make a clipping mask = 1 where we have Vs30, = 0 where we don't 
#
mask.grd : ut_ext.grd 
	grdmath ut_ext.grd 0 GT = mask.grd

#
# The Utah map doesn't have any water areas or coastlines, which simplifies
# things greatly. We paste a 0.5-degess chunk of zeroes all around so we
# have room to do our smoothing.
#
ut_ext.grd : $(MYEXT)_$(RES)c.grd north.grd south.grd west.grd east.grd
	grdpaste $(MYEXT)_$(RES)c.grd north.grd -Gtmp.grd
	grdpaste tmp.grd south.grd -Gut_ext.grd
	grdpaste west.grd ut_ext.grd -Gtmp.grd
	grdpaste tmp.grd east.grd -Gut_ext.grd
	$(RM) tmp.grd

north.grd :
	grdmath -R-114.2/-108.9/42.4/42.9 -I$(RES)s 0 = north.grd

south.grd :
	grdmath -R-114.2/-108.9/36.3/36.8 -I$(RES)s 0 = south.grd

west.grd :
	grdmath -R-114.7/-114.2/36.3/42.9 -I$(RES)s 0 = west.grd

east.grd :
	grdmath -R-108.9/-108.2/36.3/42.9 -I$(RES)s 0 = east.grd

#
# Rescale to 30-second resolution and shift the map to make it co-register
# with the global grid.
#
$(MYEXT)_$(RES)c.grd : utah6_geology_60s.grd
	grdsample -nl+t0.1 -I$(RES)s -fg -R$(UTAH_BASE_REGION) $< -G$@

utah6_geology_60s.grd :
	echo "Utah grid file utah6_geology_60s.grd must be supplied."

#################################
#
# Plots
#
plot_base_map : base.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : mask_smooth.png

plot_weights : weights.png

plot_final_map_wus : wus.png

plot_raw_map : utah.png

# Plot the base Vs30 Map:
 
base.png : $(MYEXT)_$(RES)c.grd
	grdimage $(MYEXT)_$(RES)c.grd -JM12 -C$(VS30_CPT) -Ba1WSen -K > base.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> base.ps
	pscoast -JM12 -R$(UTAH_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	convert -trim -density 300x300 -rotate 90 base.ps base.png
 
# Plot the clipping mask:
 
clipmask.png : mask.grd
	grdimage mask.grd -JM12 -C$(WEIGHTS_CPT) -Ba1WSen -K > clipmask.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> clipmask.ps
	pscoast -JM12 -R$(UTAH_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps clipmask.png
 
# Plot the smoothed clipping mask:
 
mask_smooth.png : mask_smooth.grd
	grdimage mask_smooth.grd -JM12 -C$(WEIGHTS_CPT) -Ba1WSen -K > mask_smooth.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> mask_smooth.ps
	pscoast -JM12 -R$(UTAH_EXT_REGION) -Df -O -N1 -N2 -W >> mask_smooth.ps
	convert -trim -rotate 90 mask_smooth.ps mask_smooth.png
 
# Plot the weights:
 
weights.png : weights.grd
	grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba1WSen -K > weights.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps     
	pscoast -JM12 -R$(UTAH_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 weights.ps weights.png
 
# Plot the new Vs30 map:
 
wus.png : wus_vs30.grd
	grdimage wus_vs30.grd -JM12 -C$(VS30_CPT) -Ba4f2WSen -K > wus.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> wus.ps
	pscoast -JM12 -R$(BIG_WUS_REGION) -O -Df -N1 -N2 -W >> wus.ps
	convert -trim -density 300x300 -rotate 90 wus.ps wus.png

wus_vs30.grd : ../$(OUTGRD)
	grdcut -R$(BIG_WUS_REGION) $< -G$@

# Plot the raw geology map using its own scale.

utah.png : utah6_geology_60s.grd
	grdimage utah6_geology_60s.grd -JM12 -C../Misc/utah.cpt -Ba1WSen -K > utah.ps
	psscale -D14/4.3/9/0.5 -L -C../Misc/utah.cpt -O -K >> utah.ps
	pscoast -R-114.2365/-108.877/36.7625/42.4865 -JM12 -N1 -N2 -W -Df -O >> utah.ps
	convert -rotate 90 -density 300x300 utah.ps utah.png

