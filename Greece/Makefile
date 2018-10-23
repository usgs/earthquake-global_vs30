#
# Makefile for inserting Greece Vs30 grid into the global mosaic.
#

include ~/.vs30/Constants.mk

GR_NORTH = 43
GR_SOUTH = 34
GR_WEST = 18
GR_EAST = 30

GR_BASE_REGION = $(GR_WEST)/$(GR_EAST)/$(GR_SOUTH)/$(GR_NORTH)

BIG_EUR_REGION = 4/21/34/50

.PHONY: all plots clean clean_plots veryclean

all : greece.grd weights.grd

plots : plot_base_map plot_final_mask plot_weights plot_greece \
	plot_raw_map plot_greece_non_zero_smooth plot_mask_a plot_new_mask plot_new_mask_mul \
        plot_final_map_eur

clean : clean_plots
	$(RM) gr_$(RES)c.grd final_mask.grd gr_non_zero* landmask_smooth.grd \
              weights.grd clipmask_smooth.grd mask_a.grd new_mask* \
	      landmask_land.grd landmask_water.grd clipmask.grd \
              gmt.history greece.grd

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

#
# weights.grd is plotted.
#

weights.grd : final_mask.grd
	gmt grdmath -fg $< 0.5 SUB 2 MUL DUP 0 GT MUL = $@

#
# final_mask.grd is plotted.
#

final_mask.grd : new_mask_mul_landmask_add_a.grd
	gmt grdmath $< DUP 0 GE MUL = $@

#
# new_mask_mul_landmask_add_a.grd is plotted.
#

new_mask_mul_landmask_add_a.grd : mask_a.grd new_mask_mul_landmask.grd
	gmt grdmath mask_a.grd new_mask_mul_landmask.grd ADD 1 SUB = $@

new_mask_mul_landmask.grd : new_mask.grd landmask_land.grd
	gmt grdmath new_mask.grd landmask_land.grd MUL = $@

#
# new_mask.grd is plotted.
#

new_mask.grd : gr_non_zero.grd landmask_smooth.grd
	gmt grdmath gr_non_zero.grd DUP NOT landmask_smooth.grd MUL ADD = $@

landmask_smooth.grd : landmask_land.grd ../src/smooth
	../src/smooth infile=landmask_land.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

#
# mask_a.grd is plotted.
#

mask_a.grd : clipmask_smooth.grd landmask_land.grd
	gmt grdmath clipmask_smooth.grd landmask_land.grd MUL = $@

#
# clipmask_smooth.grd is plotted.
#

clipmask_smooth.grd : clipmask.grd ../src/smooth
	../src/smooth infile=clipmask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

clipmask.grd : landmask_water.grd gr_non_zero.grd
	gmt grdmath landmask_water.grd gr_non_zero.grd ADD 0 GT = $@

gr_non_zero.grd : gr_$(RES)c.grd
	gmt grdmath $< 0 GT 0 AND = $@

#
# Now make the actual grid that will be inserted. Water is 600 and dry land not in Greece is 0.
#

greece.grd : greece_land.grd landmask_water.grd
	gmt grdmath greece_land.grd landmask_water.grd 600 MUL AND = $@

greece_land.grd : landmask_land.grd gr_$(RES)c.grd
	gmt grdmath gr_$(RES)c.grd landmask_land.grd 600 MUL AND = $@

landmask_land.grd : gr_$(RES)c.grd
	gmt grdlandmask -V -N0/1/0/1/0 -R$(GR_BASE_REGION) -I$(RES)s -G$@ -Df

landmask_water.grd : gr_$(RES)c.grd
	gmt grdlandmask -V -N1/0/1/0/1 -R$(GR_BASE_REGION) -I$(RES)s -G$@ -Df

#
# Rescale to 30-second resolution and shift the map to make it co-register
# with the global grid. In this case the grid is already 30-second resolution
# so this doesn't really do anything.
#

gr_$(RES)c.grd : Hellas-geology_18-30_34-43_30s.grd
	gmt grdsample -nl+t0.1 -I$(RES)s/$(RES)s -fg -R$(GR_BASE_REGION) $< -G$@

Hellas-geology_18-30_34-43_30s.grd :
	echo "Greece grid file Hellas-geology_18-30_34-43_30s.grd must be supplied."

#################################
#
# Plots
#

plot_base_map : base.png

plot_final_mask : final_mask.png

plot_weights : weights.png

plot_raw_map : greece_raw.png

plot_greece : greece.png

plot_greece_non_zero_smooth : clipmask_smooth.png

plot_mask_a : mask_a.png

plot_new_mask : new_mask.png

plot_new_mask_mul : new_mask_mul_landmask_add_a.png

plot_final_map_eur : eur.png

# Plot the final Vs30 Map:

final_mask.png : final_mask.grd
	gmt grdimage $< -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > final_mask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> final_mask.ps
	gmt pscoast -JM12 -R$(GR_BASE_REGION) -Df -O -N1 -N2 -W >> final_mask.ps
	convert -trim -rotate 90 final_mask.ps final_mask.png

base.png : gr_$(RES)c.grd
	gmt grdimage gr_$(RES)c.grd -JM12 -C$(NEW_VS30_CPT) -Ba1WSen -K > base.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> base.ps
	gmt pscoast -JM12 -R$(GR_BASE_REGION) -Df -O -N1 -N2 -W >> base.ps
	convert -trim -density 300x300 -rotate 90 base.ps base.png

greece.png : greece.grd
	gmt grdimage greece.grd -JM12 -C$(NEW_VS30_CPT) -Ba1WSen -K > greece.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> greece.ps
	gmt pscoast -JM12 -R$(GR_BASE_REGION) -Df -O -N1 -N2 -W >> greece.ps
	convert -trim -density 300x300 -rotate 90 greece.ps greece.png

# Plot the clipping mask:
 
mask_a.png : mask_a.grd
	gmt grdimage mask_a.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -JM12 -R$(GR_EXT_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps mask_a.png

new_mask.png : new_mask.grd
	gmt grdimage new_mask.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -JM12 -R$(GR_BASE_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps new_mask.png

new_mask_mul_landmask_add_a.png : new_mask_mul_landmask_add_a.grd
	gmt grdimage new_mask_mul_landmask_add_a.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > clipmask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask.ps
	gmt pscoast -JM12 -R$(GR_BASE_REGION) -Df -O -N1 -N2 -W >> clipmask.ps
	convert -trim -rotate 90 clipmask.ps new_mask_mul_landmask_add_a.png
 
# Plot the smoothed clipping mask:
 
clipmask_smooth.png : clipmask_smooth.grd
	gmt grdimage clipmask_smooth.grd -JM12 -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > it_non_zero_smooth.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> it_non_zero_smooth.ps
	gmt pscoast -JM12 -R$(GR_BASE_REGION) -Df -O -N1 -N2 -W >> it_non_zero_smooth.ps
	convert -trim -rotate 90 it_non_zero_smooth.ps clipmask_smooth.png
 
# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -JM12 -Ba1WSen -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps     
	gmt pscoast -JM12 -R$(GR_BASE_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 weights.ps weights.png
 
# Plot the raw geology map using its own scale.

greece_raw.png : Hellas-geology_18-30_34-43_30s.grd
	gmt grdimage Hellas-geology_18-30_34-43_30s.grd -JM12 -C../Misc/utah.cpt -Ba1WSen -K > greece_raw.ps
	gmt psscale -D14/4.3/9/0.5 -L -C../Misc/utah.cpt -O -K >> greece_raw.ps
	gmt pscoast -R$(GR_BASE_REGION) -JM12 -N1 -N2 -W -Df -O >> greece_raw.ps
	convert -rotate 90 -density 300x300 greece_raw.ps greece_raw.png

# Plot the new Vs30 map:

eur.png : eur_vs30.grd
	gmt grdimage eur_vs30.grd -JM12 -C$(NEW_VS30_CPT) -Ba4f2WSen -K > eur.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_VS30_CPT) -O -K >> eur.ps
	gmt pscoast -JM12 -R$(BIG_EUR_REGION) -O -Df -N1 -N2 -W >> eur.ps
	convert -trim -density 300x300 -rotate 90 eur.ps eur.png

eur_vs30.grd : ../global_vs30.grd
	gmt grdcut -R$(BIG_EUR_REGION) $< -G$@
