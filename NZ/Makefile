#
# Makefile for NZ Vs30 grid into the global+regional grid
#
include ~/.vs30/Constants.mk

NZ_XMIN = 166.0
NZ_XMAX = 179.0
NZ_YMIN = -47.5
NZ_YMAX = -34.0
NZ_REGION = $(NZ_XMIN)/$(NZ_XMAX)/$(NZ_YMIN)/$(NZ_YMAX)

IS_FINER := $(shell [ $(IRES) -lt 36 ] && echo true)

.PHONY: all plots clean clean_plots veryclean

all : newzealand.grd weights.grd

plots :  plot_raw_map plot_weights plot_final_map

clean : clean_plots
	$(RM) newzealand.grd weights.grd nz_*c.grd vs30_nz_gmt4.grd 

clean_plots :
	$(RM) *.ps *.png
	$(RM) newzealand_region.grd

veryclean : clean
	$(RM) global_vs30*_$(MYEXT).grd

#
# Make a weighted clipping mask; this is trivial in this case
# because our region is entirely surrounded by water; the 
# weights = 1 everywhere
#
weights.grd : newzealand.grd
	gmt grdmath $< 0 MUL 1 ADD = $@

#
# Make the insert map, Set zeros to the water velocity, and values < 150 to 150
# (per Nick Horspool)
#
newzealand.grd : nz_$(RES)c.grd
	gmt grdmath $< DUP 0 EQ $(WATER) MUL ADD 150 MAX = $@

ifeq ($(IS_FINER),true)
nz_$(RES)c.grd : vs30_nz_gmt4.grd
	gmt grdsample -nl+t0.1 -I$(RES)s -fg $< -G$@
else
nz_$(RES)c.grd : vs30_nz_gmt4.grd
	cp $< $@
endif

vs30_nz_gmt4.grd : vs30_nz_gmt4_pixel.grd
	gmt grdsample $< -G$@ -T

vs30_nz_gmt4_pixel.grd : 
	( echo "File vs30_nz_gmt4_pixel.grd must be supplied." && false )

####################################
#
# Plots
#
plot_weights : weights.png

plot_final_map : newzealand.png

plot_raw_map : newzealand_raw.png

# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba4f2eWSn -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps
	gmt pscoast -JM12 -R$(NZ_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 -density 300x300 weights.ps weights.png
 
# Plot the new Vs30 map:
ONE = 1.0
NZ_REG_XMIN = $(shell echo $(NZ_XMIN) - $(ONE) | bc)
NZ_REG_XMAX = $(shell echo $(NZ_XMAX) + $(ONE) | bc)
NZ_REG_YMIN = $(shell echo $(NZ_YMIN) - $(ONE) | bc)
NZ_REG_YMAX = $(shell echo $(NZ_YMAX) + $(ONE) | bc)
NZ_REG = $(NZ_REG_XMIN)/$(NZ_REG_XMAX)/$(NZ_REG_YMIN)/$(NZ_REG_YMAX)
 
newzealand.png : newzealand_region.grd
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba4f2eWSn -K > newzealand_region.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> newzealand_region.ps
	gmt pscoast -JM12 -R$(NZ_REG) -O -Df -N1 -N2 -W >> newzealand_region.ps
	convert -trim -density 300x300 -rotate 90 newzealand_region.ps newzealand_region.png

newzealand_region.grd : ../$(OUTGRD)
	gmt grdcut -R$(NZ_REG) $< -G$@

# Plot the raw geology map

newzealand_raw.png : vs30_nz_gmt4.grd
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba4f2eWSn -K > newzealand_raw.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> newzealand_raw.ps
	gmt pscoast -JM12 -R$(NZ_REGION) -O -Df -N1 -N2 -W >> newzealand_raw.ps
	convert -trim -rotate 90 -density 300x300 newzealand_raw.ps newzealand_raw.png

