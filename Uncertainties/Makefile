# This Makefile creates a ratio map of hybrid/slope Vs30 values. 

# This will eventually work for all periods and PGA/PGV

include ~/.vs30/Constants.mk

GLOBAL_REGION = -180/180/-56/84
SMALLER_REGION = -128/-100/35/48

all :  vs30_slope_uncert.grd

plots : vs30_slope_uncert

clean : clean_plots
	$(RM) vs30_slope_uncert.grd

clean_plots : 
	$(RM) *.ps *.png *.pdf

veryclean : clean

##################################################################
# Make the Vs30 Hybrid/Slope ratio map.

vs30_slope_uncert.grd : ../Slope/global_grad.grd
	gmt grdmath $< 0.0022 GE 0.23 MUL 0.2 ADD = $@

######################################################################################
# Make the plots.

vs30_slope_uncert : vs30_slope_uncert.png

# Set some initial flags for plotting. Edit these to change the plotting specs for
# every map below, or just edit them individually lower down. 

# Jflags = M9i		# Set the projection. Mercator, 9 inches wide.
Jflags = N0/9i
Bflags1 = a20WSen	# Axis labels for the whole plot. One label every 2 degrees, only plot on W and S side.
Sflags = 128/128/255	# Make wet areas this color. Good blue color for water. 
Bflags2 = af		# Axis labels for the scale. 
Dflags = 24/5/9/0.5	# Position(x,y), length, and width of the scale (all in cm).
Xflags = c		# X pos. of the map.
Yflags = c		# Y pos. of the map.
Tflags = g		# png
Eflags = 1080		# dpi

vs30_slope_uncert.png : vs30_slope_uncert.grd
	gmt grdimage $< -J$(Jflags) -R$(GLOBAL_REGION) -C../Misc/uncert.cpt -B$(Bflags1) -X$(Xflags) -Y$(Yflags) -K > vs30_slope_uncert.ps
	gmt pscoast -R$(GLOBAL_REGION) -J$(Jflags) -B -S$(Sflags) -X$(Xflags) -Y$(Yflags) -O -K >> vs30_slope_uncert.ps
	gmt psscale -D$(Dflags) -B$(Bflags2) -C../Misc/uncert.cpt -O >> vs30_slope_uncert.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) vs30_slope_uncert.ps
	rm gmt.history
