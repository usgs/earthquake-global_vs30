#
# Makefile for Australia Vs30 grid into the global+regional grid
#
# Note this is a good example of the case where your regional map is just an island. 
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

#####################################################################
# Make a weighted clipping mask; this is trivial in this case
# because our region is entirely surrounded by water; the 
# weights = 1 where we have Vs30, = 0 where we don't 
#

weights.grd : aus_$(RES)c.grd
	gmt grdmath $< 0 AND 0 GT = $@

#####################################################################
# Make the insert map, set NaNs to the water velocity
#

aus.grd : aus_$(RES)c.grd
	gmt grdmath $< $(WATER) AND = $@

####################################################################
# Check the resolution of the input map against the desired output
# resolution. 

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
# Plots
#

plot_weights : weights.png

plot_final_map : aus.png

plot_raw_map : aus_raw.png

# Set some flags for plotting. To change things for individual plots,
# make sure to only change it in the code block below.

Jflags = M7i		# Mercator projection, 7 inches across
Bflags = a10f5eWSn	# Tick marks. Every 10 degrees deneoted with number, 5 degree tickmarks, print numbers on W and S axes
Dflags = 20/4.3/9/0.5	# Scalebar - position 20, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -J$(Jflags) -B$(Bflags) -K > weights.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps
	gmt pscoast -J$(Jflags) -R$(AUS_REGION) -Df -O -N1 -N2 -W >> weights.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) weights.ps
 
# Plot the new Vs30 map:
ONE = 1.0
AUS_REG_XMIN = $(shell echo $(AUS_XMIN) - $(ONE) | bc)
AUS_REG_XMAX = $(shell echo $(AUS_XMAX) + $(ONE) | bc)
AUS_REG_YMIN = $(shell echo $(AUS_YMIN) - $(ONE) | bc)
AUS_REG_YMAX = $(shell echo $(AUS_YMAX) + $(ONE) | bc)
AUS_REG = $(AUS_REG_XMIN)/$(AUS_REG_XMAX)/$(AUS_REG_YMIN)/$(AUS_REG_YMAX)
 
aus.png : aus_region.grd
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > aus.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> aus.ps
	gmt pscoast -J$(Jflags) -R$(AUS_REG) -O -Df -N1 -N2 -W >> aus.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) aus.ps
	rm gmt.history

aus_region.grd : ../global_vs30.grd
	gmt grdcut -R$(AUS_REG) $< -G$@

# Plot the raw geology map

aus_raw.png : $(AUS_GRD_FILE)
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > aus_raw.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> aus_raw.ps
	gmt pscoast -J$(Jflags) -R$(AUS_REGION) -O -Df -N1 -N2 -W >> aus_raw.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) aus_raw.ps

