#
# Makefile for inserting Iran Vs30 grid into the global mosaic.
#

#
# Include the boilerplate
#

include ~/.vs30/Constants.mk

IR_WEST = 44.04166666666666666
IR_EAST = 63.33333333333333333
IR_NORTH = 39.775
IR_SOUTH = 25.083333333333333

IR_BASE_REGION = $(IR_WEST)/$(IR_EAST)/$(IR_SOUTH)/$(IR_NORTH)

IR_EXT_WEST = 41.04166666666666666
IR_EXT_EAST = 64.33333333333333333
IR_EXT_NORTH = 40.775
IR_EXT_SOUTH = 24.083333333333333

IR_EXT_REGION = $(IR_EXT_WEST)/$(IR_EXT_EAST)/$(IR_EXT_SOUTH)/$(IR_EXT_NORTH)

ME_REGION = 40/68/21/43

.PHONY : all plots clean clean_plots veryclean plot_base_map \
         plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
         plot_weights plot_final_map_wus

#
# The output grid is slightly higher resolution than the input, so we set this statement false for a
# bilinear interpolation below.
#

IS_COARSER := $(shell [ $(IRES) -gt 31 ] && echo true)

all : iran.grd weights.grd

plots : plot_original_map plot_base_map plot_vs30_landmask plot_clip_mask plot_smooth_clip_mask \
        plot_weights plot_final_map_me

clean : clean_plots
	$(RM) vs30_*.grd  landmask*.grd \
              top.grd east.grd bot.grd west.grd \
              weights.grd iran.grd new_mask* mask_a.grd ir_n* final_mask.grd gmt.history clipmask* \
              *.aux.xml ir_lake_mask.grd nan_600.grd
clean_plots :
	$(RM) *.ps *.png
	$(RM) me_vs30.grd

veryclean : clean

#
# weights.grd is plotted and explained:
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
# final_mask.grd is plotted.
#

final_mask.grd : new_mask_mul_landmask_add_a.grd
	gmt grdmath $< DUP 0 GE MUL = $@

new_mask_mul_landmask_add_a.grd : mask_a.grd new_mask_mul_landmask.grd
	gmt grdmath mask_a.grd new_mask_mul_landmask.grd ADD 1 SUB = $@

new_mask_mul_landmask.grd : new_mask.grd landmask_land.grd
	gmt grdmath new_mask.grd landmask_land.grd MUL = $@

new_mask.grd : ir_non_zero.grd landmask_smooth.grd
	gmt grdmath ir_non_zero.grd DUP NOT landmask_smooth.grd MUL ADD = $@

landmask_smooth.grd : landmask_land.grd ../src/smooth
	../src/smooth infile=landmask_land.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

mask_a.grd : clipmask_smooth.grd landmask_land.grd
	gmt grdmath clipmask_smooth.grd landmask_land.grd MUL = $@

clipmask_smooth.grd : clipmask.grd ../src/smooth
	../src/smooth infile=clipmask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

clipmask.grd : landmask_water.grd ir_non_zero.grd
	gmt grdmath landmask_water.grd ir_non_zero.grd ADD 0 GT = $@

ir_non_zero.grd : iran.grd
	gmt grdmath $< 0 GT 0 AND = $@

#
# For more details on the steps that follow, please view the Italy Makefile
# to see the same workflow and more plots of the intermediate steps.
#
########################################################################

#
# Create the grid that will be inserted into the global mosaic. The final 
# grid will be one in which water is 0, dry land outside Iran is 0, and dry 
# land inside Iran has the new Vs30 values.
#
# The Iran Vs30 grid has one problem area - two lakes in the south
# are not recognized by gmt's grdlandmask, therefore their values must be
# manually set to 600 so before creating the clipping mask. The lakes are given NaN 
# values, consistent with other lakes and oceans in Iran. To fix this, 
# create a small mask around the region, make those two lakes 600 with AND,
# and then multiply that grid with the mask, and then replace all remaining NaN's 
# in the original mask with 0's. 
#

iran.grd : ir_nan_600_mask.grd vs30_$(RES)c_ext.grd
	gmt grdmath vs30_$(RES)c_ext.grd ir_nan_600_mask.grd AND = $@

ir_nan_600_mask.grd : nan_600.grd ir_lake_mask.grd
	gmt grdmath ir_lake_mask.grd nan_600.grd MUL = $@

nan_600.grd : vs30_$(RES)c_ext.grd
	gmt grdmath $< 600 AND = $@

ir_lake_mask.grd : ir_lake_coords.xy
	gmt grdmask ir_lake_coords.xy -I$(RES)s -R$(IR_EXT_REGION) -N0/0/1 -fg -G$@

landmask_water.grd :
	gmt grdlandmask -V -R$(IR_EXT_REGION) -I$(RES)s/$(RES)s -G$@ -Df -N1/0/1/0/1

landmask_land.grd :
	gmt grdlandmask -V -R$(IR_EXT_REGION) -I$(RES)s/$(RES)s -G$@ -Df -N0/1/0/1/0

#
# Paste a 1-degree chunk of NaN's to the top, bottom, east, and west sides of the map:
#

vs30_$(RES)c_ext.grd : vs30_$(RES)c_top_bot_east.grd west.grd
	gmt grdpaste vs30_$(RES)c_top_bot_east.grd west.grd -G$@

vs30_$(RES)c_top_bot_east.grd : vs30_$(RES)c_top_bot.grd east.grd
	gmt grdpaste vs30_$(RES)c_top_bot.grd east.grd -G$@

vs30_$(RES)c_top_bot.grd : vs30_$(RES)c_top.grd bot.grd
	gmt grdpaste vs30_$(RES)c_top.grd bot.grd -G$@

vs30_$(RES)c_top.grd : vs30_$(RES)c.grd top.grd
	gmt grdpaste vs30_$(RES)c.grd top.grd -G$@

top.grd : 
	gmt grdmath -R$(IR_WEST)/$(IR_EAST)/$(IR_NORTH)/$(IR_EXT_NORTH) -I$(RES)s/$(RES)s 0 0 NAN = $@

bot.grd :
	gmt grdmath -R$(IR_WEST)/$(IR_EAST)/$(IR_EXT_SOUTH)/$(IR_SOUTH) -I$(RES)s/$(RES)s 0 0 NAN = $@

west.grd :
	gmt grdmath -R$(IR_EXT_WEST)/$(IR_WEST)/$(IR_EXT_SOUTH)/$(IR_EXT_NORTH) -I$(RES)s/$(RES)s 0 0 NAN = $@

east.grd :
	gmt grdmath -R$(IR_EAST)/$(IR_EXT_EAST)/$(IR_EXT_SOUTH)/$(IR_EXT_NORTH) -I$(RES)s/$(RES)s 0 0 NAN = $@

#
# Rescale to proper resolution. If the output is coarser than the
# input (7.5c), then we filter with a 3x3 gaussian filter; if the
# output is finer than the input, we resample with bilinear interpolation
#

ifeq ($(IS_COARSER),true)
vs30_$(RES)c.grd : Iran_Hybrid_Vs30.grd
	gmt grdfilter -I$(RES)s/$(RES)s -R$(IR_WEST)/$(IR_EAST)/$(IR_SOUTH)/$(IR_NORTH) -T -D0 -Fg0.016 -fg $< -G$@
else
vs30_$(RES)c.grd : Iran_Hybrid_Vs30.grd
	gmt grdsample -nl+t0.1 -I$(RES)s/$(RES)s -R$(IR_WEST)/$(IR_EAST)/$(IR_SOUTH)/$(IR_NORTH) -T -fg $< -G$@
endif

#
# Check for the file.
# 

Iran_Hybrid_Vs30.grd :
	echo "Iran grid file Iran_Hybrid_Vs30.grd must be supplied."

../src/smooth :
	$(MAKE) -C ../src smooth

###################################################
#
# Plots
#

plot_original_map : original.png

plot_base_map : base.png

plot_vs30_landmask : iran.png

plot_clip_mask : final_mask.png

plot_weights : weights.png

plot_final_map_me : me.png

# Plot original basemap

original.png : Iran_Hybrid_Vs30.grd
	gmt grdimage Iran_Hybrid_Vs30.grd -JM12 -C$(NEW_VS30_CPT) -Ba2f1WSen -K > original.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> original.ps
	gmt pscoast -JM12 -R$(IR_BASE_REGION) -Df -O -N1 -N2 -W >> original.ps
	convert -rotate 90 original.ps original.png

# Plot the resampled base Vs30 map

base.png : vs30_$(RES)c.grd
	gmt grdimage vs30_$(RES)c.grd -JM12 -C$(NEW_VS30_CPT) -Ba2f1WSen -K > base.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> base.ps
	gmt pscoast -JM12 -R$(IR_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	convert -rotate 90 base.ps base.png
 
# Plot the landmask with CA Vs30s inserted:
 
iran.png : iran.grd
	gmt grdimage iran.grd -JM12 -C$(NEW_VS30_CPT) -Ba2f1WSen -K > iran.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> iran.ps
	gmt pscoast -JM12 -R$(IR_EXT_REGION) -Df -O -N1 -N2 -W >> iran.ps
	convert -rotate 90 iran.ps iran.png
 
# Plot the clipping mask:

final_mask.png : final_mask.grd
	gmt grdimage final_mask.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba2f1WSen -K > final_mask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O >> final_mask.ps
	convert -rotate 90 final_mask.ps final_mask.png

# Plot the weights:

weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -JM12 -Ba2f1WSen -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -JM12 -R$(IR_EXT_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -rotate 90 weights.ps weights.png
 
# Plot the new Vs30 map in a cut out map of the western US
 
me.png : me_vs30.grd
	gmt grdimage me_vs30.grd -JM12 -C$(NEW_VS30_CPT) -Ba2f1WSen -K > me_vs30.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> me_vs30.ps
	gmt pscoast -JM12 -R$(ME_REGION) -Df -O -N1 -N2 -W >> me_vs30.ps
	convert -rotate 90 me_vs30.ps me_vs30.png

me_vs30.grd : ../global_vs30.grd
	gmt grdcut -R$(ME_REGION) $< -G$@

