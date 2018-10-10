#
# Makefile for Italy Vs30 grid into the global+regional grid
#
include ~/.vs30/Constants.mk

IT_NORTH = 47.10833333333333
IT_SOUTH = 35.21666666666666
IT_WEST = 6.45833333333333
IT_EAST = 19.58333333333333

IT_BASE_REGION = $(IT_WEST)/$(IT_EAST)/$(IT_SOUTH)/$(IT_NORTH)

IT_NORTH_EXT = 48.1083333333333

IT_EXT_REGION = $(IT_WEST)/$(IT_EAST)/$(IT_SOUTH)/$(IT_NORTH_EXT)

BIG_EUR_REGION = 4/21/34/50

.PHONY: all plots clean clean_plots veryclean

all : italy.grd final_mask.grd weights.grd

plots : plot_base_map plot_final_mask plot_weights plot_italy plot_final_map_eur \
	plot_raw_map plot_it_non_zero_smooth plot_mask_a plot_new_mask plot_new_mask_mul

clean : clean_plots
	$(RM) it_$(RES)c.grd north.grd final_mask.grd it_non_zero* landmask_smooth.grd \
              it_ext.grd weights.grd clipmask_smooth.grd mask_a.grd new_mask* \
	      landmask.grd landmask_water.grd clipmask.grd \
              gmt.history

clean_plots :
	$(RM) *.ps *.png
	$(RM) eur_vs30.grd

veryclean : clean

#
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, 
# inside ranging from 0 to 0.5), multiply by 2 (making outside still < 0, 
# inside ranging from 0 to 1.0), and then keep only positive values (making 
# outside = 0, inside ranging from 0 to 1). 
#

weights.grd : final_mask.grd
	gmt grdmath -fg $< 0.5 SUB 2 MUL DUP 0 GT MUL = $@

#
# Smooth the mask. This will blur the border, but we'll fix that in a 
# minute. We make the filter twice the width and height that we want (for the
# same reason). We're using a 0.35 degree transition, which means a 0.7 degree 
# filter, which is 84 grid points, but we want an odd number. That means 
# fx = fy = 85:
#

# mask_smooth.grd : mask.grd ../src/smooth
# 	../src/smooth infile=mask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

#
# Make a clipping mask = 1 where we have Vs30, = 0 where we don't 
#

# mask.grd : italy.grd
# 	gmt grdmath -fg $< 0 GT = $@

#
# Land outside Italy is still 603, so we want all 603 nodes to become 0. 
# First, replace all 603 nodes with NaN's, then replace the NaN's with 0.
# This can probably be done in one step but I couldn't find the grdmath command
# to do it.
#
# This makes dry land outside Italy = 0, water = 600, and dry land inside Italy = new Vs30 values.
#

#
# Now let's check to see if there are still 603 values inside italy (i.e., the grid maker
# assigned it to water but GMT says it is not water. Venice??
# 

#
# The Italy map has value 603 everywhere except where there is a calculated Vs30
# value. This step makes all ocean, lake, and pond water 600. It leaves land nodes outside of Italy
# equal to 603 still, and land nodes inside Italy = new Vs30 values. 
# 

final_mask.grd : new_mask_mul_landmask_add_a.grd
	gmt grdmath $< DUP 0 GE MUL = $@

new_mask_mul_landmask_add_a.grd : mask_a.grd new_mask_mul_landmask.grd
	gmt grdmath mask_a.grd new_mask_mul_landmask.grd ADD 1 SUB = $@

new_mask_mul_landmask.grd : new_mask.grd landmask.grd
	gmt grdmath new_mask.grd landmask.grd MUL = $@

# Now this one doen't have the same gradient from Italy to its coasts, but it does have the issues
# of other countries being blended into their coasts.

new_mask.grd : it_non_zero.grd landmask_smooth.grd
	gmt grdmath it_non_zero.grd DUP NOT landmask_smooth.grd MUL ADD = $@

landmask_smooth.grd : landmask.grd ../src/smooth
	../src/smooth infile=landmask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

mask_a.grd : clipmask_smooth.grd landmask.grd
	gmt grdmath clipmask_smooth.grd landmask.grd MUL = $@

clipmask_smooth.grd : clipmask.grd ../src/smooth
	../src/smooth infile=clipmask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

clipmask.grd : landmask_water.grd it_non_zero.grd
	gmt grdmath landmask_water.grd it_non_zero.grd ADD 0 GT = $@

landmask_water.grd :
	gmt grdlandmask -V -R$(IT_EXT_REGION) -I$(RES)s -G$@ -Df -N1/0/1/0/0

landmask.grd :
	gmt grdlandmask -V -R$(IT_EXT_REGION) -I$(RES)s -G$@ -Df -N0/1/0/1/0

it_non_zero.grd : it_ext.grd
	gmt grdmath $< 603 NEQ = $@

#
# Now make the actual grid that will be inserted.
#

italy.grd : it_ext.grd
	gmt grdmath it_ext.grd DUP 603 EQ 3 MUL SUB = $@

#
# We're going to paste a 1 degree chunk of nodes on top of the grid to help
# with smoothing later. Value for each node will be 603, consistent with the
# original grid.
#

it_ext.grd : it_$(RES)c.grd north.grd
	gmt grdpaste it_$(RES)c.grd north.grd -Git_ext.grd

north.grd :
	gmt grdmath -R$(IT_WEST)/$(IT_EAST)/$(IT_NORTH)/$(IT_NORTH_EXT) -I$(RES)s/$(RES)s 603 = $@

#
# Rescale to 30-second resolution and shift the map to make it co-register
# with the global grid.
#

it_$(RES)c.grd : italygeol60s_vsgrid.grd
	gmt grdsample -nl+t0.1 -I$(RES)s/$(RES)s -fg -R$(IT_BASE_REGION) $< -G$@

italygeol60s_vsgrid.grd :
	echo "Italy grid file italygeol60s_vsgrid.grd must be supplied."

#################################
#
# Plots
#

plot_base_map : base.png

plot_final_mask : final_mask.png

plot_weights : weights.png

plot_raw_map : italy_raw.png

plot_italy : italy.png

plot_it_non_zero_smooth : clipmask_smooth.png

plot_mask_a : mask_a.png

plot_new_mask : new_mask.png

plot_new_mask_mul : new_mask_mul_landmask_add_a.png

plot_final_map_eur : eur.png

# Plot the final Vs30 Map:

final_mask.png : final_mask.grd
	gmt grdimage $< -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > final_mask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> final_mask.ps
	gmt pscoast -JM12 -R$(IT_EXT_REGION) -Df -O -N1 -N2 -W >> final_mask.ps
	convert -trim -rotate 90 final_mask.ps final_mask.png

base.png : it_$(RES)c.grd
	gmt grdimage it_$(RES)c.grd -JM12 -C$(NEW_VS30_CPT) -Ba1WSen -K > base.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> base.ps
	gmt pscoast -JM12 -R$(IT_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	convert -trim -density 300x300 -rotate 90 base.ps base.png

italy.png : italy.grd
	gmt grdimage italy.grd -JM12 -C$(NEW_VS30_CPT) -Ba1WSen -K > italy.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> italy.ps
	gmt pscoast -JM12 -R$(IT_BASE_REGION) -Df -O -N1 -N2 -W >> italy.ps
	convert -trim -density 300x300 -rotate 90 italy.ps italy.png

# Plot the clipping mask:
 
mask_a.png : mask_a.grd
	gmt grdimage mask_a.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -JM12 -R$(IT_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps mask_a.png

new_mask.png : new_mask.grd
	gmt grdimage new_mask.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -JM12 -R$(IT_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps new_mask.png

new_mask_mul_landmask_add_a.png : new_mask_mul_landmask_add_a.grd
	gmt grdimage new_mask_mul_landmask_add_a.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -JM12 -R$(IT_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps new_mask_mul_landmask_add_a.png
 
# Plot the smoothed clipping mask:
 
clipmask_smooth.png : clipmask_smooth.grd
	gmt grdimage clipmask_smooth.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > it_non_zero_smooth.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> it_non_zero_smooth.ps
	gmt pscoast -JM12 -R$(IT_EXT_REGION) -Df -O -N1 -N2 -W >> it_non_zero_smooth.ps
	convert -trim -rotate 90 it_non_zero_smooth.ps clipmask_smooth.png
 
# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -JM12 -Ba1WSen -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -JM12 -R$(IT_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 weights.ps weights.png
 
# Plot the raw geology map using its own scale.

italy_raw.png : italygeol60s_vsgrid.grd
	gmt grdimage italygeol60s_vsgrid.grd -JM12 -C../Misc/utah.cpt -Ba1WSen -K > italy_raw.ps
	gmt psscale -D14/4.3/9/0.5 -L -C../Misc/utah.cpt -O -K >> italy_raw.ps
	gmt pscoast -R$(IT_BASE_REGION) -JM12 -N1 -N2 -W -Df -O >> italy_raw.ps
	convert -rotate 90 -density 300x300 italy_raw.ps italy_raw.png

# Plot the new Vs30 map:

eur.png : eur_vs30.grd
	gmt grdimage eur_vs30.grd -JM12 -C$(NEW_VS30_CPT) -Ba4f2WSen -K > eur.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> eur.ps
	gmt pscoast -JM12 -R$(BIG_EUR_REGION) -O -Df -N1 -N2 -W >> eur.ps
	convert -trim -density 300x300 -rotate 90 eur.ps eur.png

eur_vs30.grd : ../global_vs30.grd
	gmt grdcut -R$(BIG_EUR_REGION) $< -G$@
