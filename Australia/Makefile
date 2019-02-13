#
# Makefile for Australia Vs30 grid into the global+regional grid
#
include ~/.vs30/Constants.mk

AUS_GRD_FILE = asscm_wii_vs30.grd

AUS_XMIN = 112.921404378
AUS_XMAX = 153.638071043
AUS_YMIN = -43.6586201656
AUS_YMAX = -9.14195350033
AUS_REGION = $(AUS_XMIN)/$(AUS_XMAX)/$(AUS_YMIN)/$(AUS_YMAX)

IS_FINER := $(shell [ $(IRES) -lt 30 ] && echo true)

.PHONY: all plots clean clean_plots veryclean

all : aus.grd weights.grd

plots :  plot_raw_map plot_weights plot_final_map

clean : clean_plots
	$(RM) aus.grd weights.grd aus_*c.grd gmt.history

clean_plots :
	$(RM) *.ps *.png
	$(RM) aus_region.grd

veryclean : clean

#
# Make a weighted clipping mask; this is trivial in this case
# because our region is entirely surrounded by water; the 
# weights = 1 where we have Vs30, = 0 where we don't 
#
weights.grd : aus_$(RES)c.grd
	gmt grdmath $< 0 AND 0 GT = $@

#
# Make the insert map, set NaNs to the water velocity
#
aus.grd : aus_$(RES)c.grd
	gmt grdmath $< $(WATER) AND = $@

ifeq ($(IS_FINER),true)
aus_$(RES)c.grd : $(AUS_GRD_FILE)
	gmt grdsample -nl+t0.1 -I$(RES)s -fg $< -G$@
else
aus_$(RES)c.grd : $(AUS_GRD_FILE)
	cp $< $@
endif

$(AUS_GRD_FILE) : 
	( echo "File $(AUS_GRD_FILE) must be supplied." && false )

####################################
#
# Plots
#
plot_weights : weights.png

plot_final_map : aus.png

plot_raw_map : aus_raw.png

# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -JM12 -Ba10f5eWSn -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps
	gmt pscoast -JM12 -R$(AUS_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 -density 300x300 weights.ps weights.png
 
# Plot the new Vs30 map:
ONE = 1.0
AUS_REG_XMIN = $(shell echo $(AUS_XMIN) - $(ONE) | bc)
AUS_REG_XMAX = $(shell echo $(AUS_XMAX) + $(ONE) | bc)
AUS_REG_YMIN = $(shell echo $(AUS_YMIN) - $(ONE) | bc)
AUS_REG_YMAX = $(shell echo $(AUS_YMAX) + $(ONE) | bc)
AUS_REG = $(AUS_REG_XMIN)/$(AUS_REG_XMAX)/$(AUS_REG_YMIN)/$(AUS_REG_YMAX)
 
aus.png : aus_region.grd
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba10f5eWSn -K > aus_region.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> aus_region.ps
	gmt pscoast -JM12 -R$(AUS_REG) -O -Df -N1 -N2 -W >> aus_region.ps
	convert -trim -density 300x300 -rotate 90 aus_region.ps aus_region.png

aus_region.grd : ../global_vs30.grd
	gmt grdcut -R$(AUS_REG) $< -G$@

# Plot the raw geology map

aus_raw.png : $(AUS_GRD_FILE)
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba10f5eWSn -K > aus_raw.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> aus_raw.ps
	gmt pscoast -JM12 -R$(AUS_REGION) -O -Df -N1 -N2 -W >> aus_raw.ps
	convert -trim -rotate 90 -density 300x300 aus_raw.ps aus_raw.png

