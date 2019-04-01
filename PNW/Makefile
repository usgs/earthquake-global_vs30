#
# Makefile for PNW Vs30 grid into the global grid
#

include ~/.vs30/Constants.mk

PNW_BASE_REGION = -125.144158614/-116.462158614/41.995401663/49.083401663
PNW_EXT_REGION = -126/-116/41/50

.PHONY: all plots clean clean_plots veryclean plot_base_map plot_clip_mask \
        plot_smooth_clip_mask plot_clip_mask_2 plot_weights plot_final_map_wus

IS_COARSER := $(shell [ $(IRES) -gt 20 ] && echo true)

all : pnw.grd weights.grd

plots : plot_base_map plot_ext_map plot_clip_mask plot_smooth_clip_mask \
        plot_mask_a plot_weights plot_final_map_waor plot_final_map_wus

clean : clean_plots
	$(RM) pnw.grd orwash20c_600.grd waor_$(RES)c.grd north.grd south.grd east.grd \
              west.grd waor_ext.grd \
              new_mask.grd clipmask* mask_a* landmask* new_* final_mask.grd \
              weights.grd gmt.history

clean_plots :
	$(RM) *.ps *.png
	$(RM) wus_vs30.grd waor_embedded.grd

veryclean : clean

################################################################################
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, 
# inside ranging from 0 to 0.5), multiply by 2 (making outside still < 0, 
# inside ranging from 0 to 1.0), and then keep only positive values (making 
# outside = 0, inside ranging from 0 to 1). 
#

weights.grd : final_mask.grd
	gmt grdmath -fg $< 0.5 SUB 2 MUL DUP 0 GT MUL = $@

#################################################################################
# Smooth the mask. This will blur the border, but we'll fix that in a 
# minute. We make the filter twice the width and height that we want (for the
# same reason). Let's try a 0.35 degree transition, which means a 0.7 degree 
# filter, which is 84 grid points, but we want an odd number. That means 
# fx = fy = 85:
#

final_mask.grd : new_mask_mul_landmask_add_a.grd
	gmt grdmath $< DUP 0 GE MUL = $@

new_mask_mul_landmask_add_a.grd : mask_a.grd new_mask_mul_landmask.grd
	gmt grdmath mask_a.grd new_mask_mul_landmask.grd ADD 1 SUB = $@

new_mask_mul_landmask.grd : new_mask_2.grd landmask_land.grd
	gmt grdmath new_mask_2.grd landmask_land.grd MUL = $@

new_mask_2.grd : clipmask.grd landmask_smooth.grd
	gmt grdmath clipmask.grd DUP NOT landmask_smooth.grd MUL ADD = $@

landmask_smooth.grd : landmask_land.grd ../src/smooth
	../src/smooth infile=landmask_land.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

mask_a.grd : clipmask_smooth.grd landmask_land.grd
	gmt grdmath clipmask_smooth.grd landmask_land.grd MUL = $@

clipmask_smooth.grd : clipmask.grd ../src/smooth
	../src/smooth infile=clipmask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@


################################################################################
# The PNW Vs30 is now 600 everywhere outside of known geology. So now we
# make a mask in which 600 -> 0, and everything else -> 1.
#

clipmask.grd : new_mask.grd landmask_water.grd
	gmt grdmath new_mask.grd landmask_water.grd ADD 0 GT = $@

new_mask.grd : pnw.grd landmask_land.grd
	gmt grdmath pnw.grd $(WATER) NEQ landmask_land.grd MUL = $@

landmask_water.grd :
	gmt grdlandmask -V -N1/0/0/0/0 -R$(CA_EXT_REGION) -I$(RES)s -G$@ -Df

landmask_land.grd : 
	gmt grdlandmask -N0/1/1/1/1 -R$(PNW_EXT_REGION) -I$(RES)s/$(RES)s -G$@

###############################################################################
# Rescale to the proper resolution and shift the map to make it co-register
# with the global grid. For a coarser grid, we want to filter, but we also
# want to preserve the quantized Vs30 values, so we use a 3x3 median filter, 
# which preserves most (but for some reason not all) of the Vs30 values.
# For a finer grid, we just use grdsample with a nearest-neighbor sampling.
#

ifeq ($(IS_COARSER),true)
pnw.grd : waor_ext.grd
	gmt grdfilter -I$(RES)s -R$(PNW_EXT_REGION) -D0 -Fm0.016 $< -G$@
else
pnw.grd : waor_ext.grd
	gmt grdsample -nn -I$(RES)s -fg -R$(PNW_EXT_REGION) $< -G$@
endif

###############################################################################
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

##########################################################################################################
# This grid uses NaN as water and unknown gelolgy, but we want 600, so 
# we fix that
#

orwash20c_600.grd : orwash20c.grd
	gmt grdmath orwash20c.grd 600 AND = orwash20c_600.grd

orwash20c.grd :
	echo "Washington/Oregon grid file orwash20c.grd must be supplied."

../src/smooth :
	$(MAKE) -C ../src smooth

###################################
# Plots
#

plot_base_map : base.png

plot_ext_map : ext.png

plot_clip_mask : clipmask.png

plot_smooth_clip_mask : clipmask_smooth.png

plot_mask_a : mask_a.png

plot_weights : weights.png

plot_final_map_waor : waor.png

plot_final_map_wus : wus.png

# Set some flags for plotting. To change things for an individual map,
# make sure to only change it in that particular code block below.

Jflags = M12		# Mercator projection, 12 cm across
Bflags = a2f1WSen	# Tick marks. Every 2 degrees deneoted with number, 1 degree tickmarks, print numbers on W and S axes
Dflags = 14/4.3/9/0.5	# Scalebar - position 14, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

# Plot the base Vs30 Map:
 
base.png : orwash20c.grd
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > base.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> base.ps
	gmt pscoast -J$(Jflags) -R$(PNW_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) base.ps

# Plot the extended Vs30 map:

ext.png : pnw.grd 
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > ext.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> ext.ps
	gmt pscoast -J$(Jflags) -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> ext.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) ext.ps

# Plot the clipping mask:
 
clipmask.png : clipmask.grd
	gmt grdimage clipmask.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > clipmask.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -J$(Jflags) -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) clipmask.ps
 
# Plot the smoothed clipping mask:
 
clipmask_smooth.png : clipmask_smooth.grd
	gmt grdimage clipmask_smooth.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > clipmask_smooth.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask_smooth.ps
	gmt pscoast -J$(Jflags) -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask_smooth.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) clipmask_smooth.ps
 
# Plot the rescaled clipping mask.
 
mask_a.png : mask_a.grd
	gmt grdimage mask_a.grd -J$(Jflags) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > mask_a.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> mask_a.ps
	gmt pscoast -J$(Jflags) -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> mask_a.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) mask_a.ps
 
# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -J$(Jflags) -B$(Bflags) -K > weights.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -J$(Jflags) -R$(PNW_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) weights.ps
 
# Plot the new Vs30 map in the western US region:
 
wus.png : wus_vs30.grd
	gmt grdimage wus_vs30.grd -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > wus.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> wus.ps
	gmt pscoast -J$(Jflags) -R$(WUS_REGION) -O -Df -N1 -N2 -W >> wus.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) wus.ps
	rm gmt.history

wus_vs30.grd : ../global_vs30.grd
	gmt grdcut -R$(WUS_REGION) $< -G$@
 
# Plot the new Vs30 map at the original scale:
 
waor.png : waor_embedded.grd
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > waor.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> waor.ps
	gmt pscoast -J$(Jflags) -R$(PNW_BASE_REGION) -O -Df -N1 -N2 -W >> waor.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) waor.ps

waor_embedded.grd : ../global_vs30.grd
	gmt grdcut -R$(PNW_BASE_REGION) $< -G$@
