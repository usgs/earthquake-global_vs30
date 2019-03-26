#
# Makefile for Utah Vs30 grid into the global+regional grid
#
# Note this is a good example for adding a region that is completely landlocked. 
# The complicated workflow for not smoothing coastlines is not needed. The only 
# thing we add here is a "buffer zone" around the region, which helps with smoothing. 
#

include ~/.vs30/Constants.mk

UTAH_BASE_REGION = -114.2/-108.9/36.8/42.4
UTAH_EXT_REGION = -114.7/-108.2/36.3/42.9

.PHONY: all plots clean clean_plots veryclean

all : ut_ext.grd weights.grd

plots : plot_base_map plot_clip_mask plot_smooth_clip_mask plot_weights \
	plot_final_map_wus plot_raw_map

clean : clean_plots
	$(RM) ut_$(RES)c.grd north.grd south.grd east.grd west.grd \
              ut_ext.grd mask.grd mask_smooth.grd weights.grd gmt.history

clean_plots :
	$(RM) *.ps *.png
	$(RM) wus_vs30.grd

veryclean : clean

##################################################################################
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
# same reason). We're using a 0.35 degree transition, which means a 0.7 degree 
# filter, which is 84 grid points, but we want an odd number. That means 
# fx = fy = 85:
#

mask_smooth.grd : mask.grd ../src/smooth 
	../src/smooth infile=mask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

#
# Make a clipping mask = 1 where we have Vs30, = 0 where we don't 
#

mask.grd : ut_ext.grd 
	gmt grdmath $< 0 GT = $@

#
# The Utah map doesn't have any water areas or coastlines, which simplifies
# things greatly. We paste a 0.5-degess chunk of zeroes all around so we
# have room to do our smoothing.
#

ut_ext.grd : ut_$(RES)c.grd north.grd south.grd west.grd east.grd
	gmt grdpaste ut_$(RES)c.grd north.grd -Gtmp.grd
	gmt grdpaste tmp.grd south.grd -Gut_ext.grd
	gmt grdpaste west.grd ut_ext.grd -Gtmp.grd
	gmt grdpaste tmp.grd east.grd -Gut_ext.grd
	$(RM) tmp.grd

north.grd :
	gmt grdmath -R-114.2/-108.9/42.4/42.9 -I$(RES)s 0 = $@

south.grd :
	gmt grdmath -R-114.2/-108.9/36.3/36.8 -I$(RES)s 0 = $@

west.grd :
	gmt grdmath -R-114.7/-114.2/36.3/42.9 -I$(RES)s 0 = $@

east.grd :
	gmt grdmath -R-108.9/-108.2/36.3/42.9 -I$(RES)s 0 = $@

##################################################################################
# Rescale to 30-second resolution and shift the map to make it co-register
# with the global grid.
#

ut_$(RES)c.grd : utah6_geology_60s.grd
	gmt grdsample -nl+t0.1 -I$(RES)s -fg -R$(UTAH_BASE_REGION) $< -G$@

utah6_geology_60s.grd :
	echo "Utah grid file utah6_geology_60s.grd must be supplied."

#################################
# Plots
#

plot_base_map : base.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : mask_smooth.png

plot_weights : weights.png

plot_final_map_wus : wus.png

plot_raw_map : utah.png

# Set some flags for plotting. To change things for an individual map,
# make sure to only change it in that particular code block below.

Jflags = M12		# Mercator projection, 12 cm across
Bflags = a2f1WSen	# Tick marks. Every 2 degrees deneoted with number, 1 degree tickmarks, print numbers on W and S axes
Dflags = 14/4.3/9/0.5	# Scalebar - position 14, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

# Plot the base Vs30 Map:
 
base.png : ut_$(RES)c.grd
	gmt grdimage ut_$(RES)c.grd -J$(Jflags) -C$(VS30_CPT) -B$(Bflags) -K > base.ps
	gmt psscale -D$(Dflags) -L -C$(VS30_CPT) -O -K >> base.ps
	gmt pscoast -J$(Jflags) -R$(UTAH_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) base.ps
 
# Plot the clipping mask:
 
clipmask.png : mask.grd
	gmt grdimage mask.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > clipmask.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -J$(Jflags) -R$(UTAH_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) clipmask.ps
 
# Plot the smoothed clipping mask:
 
mask_smooth.png : mask_smooth.grd
	gmt grdimage mask_smooth.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > mask_smooth.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> mask_smooth.ps
	gmt pscoast -J$(Jflags) -R$(UTAH_EXT_REGION) -Df -O -N1 -N2 -W >> mask_smooth.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) mask_smooth.ps

# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -J$(Jflags) -B$(Bflags) -K > weights.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -J$(Jflags) -R$(UTAH_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) weights.ps
 
# Plot the new Vs30 map:
 
wus.png : wus_vs30.grd
	gmt grdimage wus_vs30.grd -J$(Jflags) -C$(VS30_CPT) -B$(Bflags) -K > wus.ps
	gmt psscale -D$(Dflags) -L -C$(VS30_CPT) -O -K >> wus.ps
	gmt pscoast -J$(Jflags) -R$(BIG_WUS_REGION) -O -Df -N1 -N2 -W >> wus.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) wus.ps

wus_vs30.grd : ../global_vs30.grd
	gmt grdcut -R$(BIG_WUS_REGION) $< -G$@

# Plot the raw geology map using its own scale.

utah.png : utah6_geology_60s.grd
	gmt grdimage utah6_geology_60s.grd -J$(Jflags) -C../Misc/utah.cpt -B$(Bflags) -K > utah.ps
	gmt psscale -D$(Dflags) -L -C../Misc/utah.cpt -O -K >> utah.ps
	gmt pscoast -R-114.2365/-108.877/36.7625/42.4865 -J$(Jflags) -N1 -N2 -W -Df -O >> utah.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) utah.ps
	rm gmt.history
