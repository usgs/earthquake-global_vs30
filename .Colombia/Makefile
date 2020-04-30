#
# Makefile for inserting Colombia Vs30 grid into the global mosaic.
#
# This grid is fairly easy to add because there entire grid file
# has calculated Vs30 values, meaning we can skip the workflow
# of smoothing Vs30 values on land but not into the water. We instead
# just pad the resampled map with zeros and then create a weighted
# clipping mask.
# 
# Note that the 2015 Vs30 map is included in this directory. This is
# the old version of the map, but we include it for the user's 
# 

include ~/.vs30/Constants.mk

CO_NORTH = 15
CO_SOUTH = -4.991666667
CO_WEST = -82
CO_EAST = -65

CO_NORTH_EXT = 16
CO_SOUTH_EXT = -5.991666667
CO_WEST_EXT = -83
CO_EAST_EXT = -64

CO_BASE_REGION = $(CO_WEST)/$(CO_EAST)/$(CO_SOUTH)/$(CO_NORTH)
CO_EXT_REGION = $(CO_WEST_EXT)/$(CO_EAST_EXT)/$(CO_SOUTH_EXT)/$(CO_NORTH_EXT)

BIG_SA_REGION = -82/-60/-10/16

.PHONY: all plots clean clean_plots veryclean

all : colombia.grd weights.grd

plots : plot_base_map plot_weights plot_colombia plot_raw_map plot_final_map_sa

clean : clean_plots
	$(RM) co_$(RES)c.grd weights.grd gmt.history greece.grd col_*.grd \
	east.grd west.grd south.grd north.grd weights_smooth.grd colombia.grd colombia_orig.grd

clean_plots :
	$(RM) *.ps *.png
	$(RM) sa_vs30.grd

veryclean : clean

###############################################################################
# We have values that range from 0 to 1. Areas outside the border will have 
# values < 0.5, and inside > 0.5. So we subtract 0.5 (making outside < 0, 
# inside ranging from 0 to 0.5), multiply by 2 (making outside still < 0, 
# inside ranging from 0 to 1.0), and then keep only positive values (making 
# outside = 0, inside ranging from 0 to 1). 
#

weights.grd : weights_smooth.grd
	gmt grdmath -fg $< 0.5 SUB 2 MUL DUP 0 GT MUL = $@

weights_smooth.grd : col_ones_zeros.grd ../src/smooth
	../src/smooth infile=col_ones_zeros.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

col_ones_zeros.grd : colombia.grd
	gmt grdmath $< 0 GT = $@

#################################################################################
# To make the final colombia grid we pad it with a one degree strip of zeros.
# This gives the smoothing operation above room to operate. The zeros will
# be replaced completely by the background slope values. 
#

colombia.grd : col_e_w_s.grd north.grd
	gmt grdpaste col_e_w_s.grd north.grd -G$@
col_e_w_s.grd : col_e_w.grd south.grd
	gmt grdpaste col_e_w.grd south.grd -G$@
col_e_w.grd : col_e.grd west.grd
	gmt grdpaste col_e.grd west.grd -G$@
col_e.grd : colombia_orig.grd east.grd
	gmt grdpaste colombia_orig.grd east.grd -G$@

north.grd :
	gmt grdmath -R$(CO_WEST_EXT)/$(CO_EAST_EXT)/$(CO_NORTH)/$(CO_NORTH_EXT) -I$(RES)s/$(RES)s 0 1 NAN = $@
south.grd :
	gmt grdmath -R$(CO_WEST_EXT)/$(CO_EAST_EXT)/$(CO_SOUTH_EXT)/$(CO_SOUTH) -I$(RES)s/$(RES)s 0 1 NAN = $@
west.grd :
	gmt grdmath -R$(CO_WEST_EXT)/$(CO_WEST)/$(CO_SOUTH)/$(CO_NORTH) -I$(RES)s/$(RES)s 0 1 NAN = $@
east.grd :
	gmt grdmath -R$(CO_EAST)/$(CO_EAST_EXT)/$(CO_SOUTH)/$(CO_NORTH) -I$(RES)s/$(RES)s 0 1 NAN = $@

####################################################################################################
# Now make the actual grid that will be inserted where water is 600.
#

colombia_orig.grd : co_$(RES)c.grd
	gmt grdmath $< 0 LE 600 $< IFELSE = $@

##############################################################################################
# Rescale to user-defined resolution and shift the map to make it co-register
# with the global grid. If you want the 2015 map, you'll have to change this step
# and change the boundaries of the region. Also, the background values in the
# 2015 map are NaN's whereas in this new map it's already in a place where we 
# can readily insert it (with the exception of water values).
#

co_$(RES)c.grd : Vs30_Reg_JERASO_18052016_int.grd
	gmt grdsample -nl+t0.1 -I$(RES)s/$(RES)s -fg -R$(CO_BASE_REGION) $< -G$@

Vs30_Reg_JERASO_18052016_int.grd :
	echo "Colombia grid file Vs30_Reg_JERASO_18052016_int.grd must be supplied."

../src/smooth :
	$(MAKE) -C ../src smooth

#################################
# Plots
#

plot_base_map : base.png

plot_weights : weights.png

plot_raw_map : colombia_raw.png

plot_colombia : colombia.png

plot_final_map_sa : sa.png

# Set some flags for plotting. To change things for individual plots,
# make sure to only change it in the code block below.

Jflags = M15		# Mercator projection, 15 cm across
Bflags = a2f1WSen	# Tick marks. Every 2 degrees deneoted with number, 1 degree tickmarks, print numbers on W and S axes
Dflags = 17/4.3/9/0.5	# Scalebar - position 17, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

base.png : co_$(RES)c.grd
	gmt grdimage co_$(RES)c.grd -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > base.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> base.ps
	gmt pscoast -J$(Jflags) -R$(CO_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) base.ps

colombia.png : colombia.grd
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > colombia.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> colombia.ps
	gmt pscoast -J$(Jflags) -R$(CO_EXT_REGION) -Df -O -N1 -N2 -W >> colombia.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) colombia.ps
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -J$(Jflags) -B$(Bflags) -K > weights.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -J$(Jflags) -R$(BIG_SA_REGION) -Df -O -N1 -N2 -W >> weights.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) weights.ps

colombia_raw.png : Vs30_Reg_JERASO_18052016_int.grd
	gmt grdimage Vs30_Reg_JERASO_18052016_int.grd -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > colombia_raw.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> colombia_raw.ps
	gmt pscoast -R$(CO_BASE_REGION) -J$(Jflags) -N1 -N2 -W -Df -O >> colombia_raw.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) colombia_raw.ps

sa.png : sa_vs30.grd
	gmt grdimage sa_vs30.grd -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > sa.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> sa.ps
	gmt pscoast -J$(Jflags) -R$(BIG_SA_REGION) -O -Df -N1 -N2 -W >> sa.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) sa.ps
	rm gmt.history

sa_vs30.grd : ../global_vs30.grd
	gmt grdcut -R$(BIG_SA_REGION) $< -G$@
