# This Makefile creates a ratio map of hybrid/slope Vs30 values. 

# This will eventually work for all periods and PGA/PGV

include ~/.vs30/Constants.mk

GLOBAL_REGION = -180/180/-56/84
SMALLER_REGION = -128/-100/40/52

all :  vs30_ratio.grd

plots : vs30_ratio

clean : clean_plots
	$(RM) vs30_ratio.grd

clean_plots : 
	$(RM) *.ps *.png

veryclean : clean

##################################################################
# Make the Vs30 Hybrid/Slope ratio map.

vs30_ratio.grd : ../global_vs30.grd ../Slope/global_vs30.grd
	gmt grdmath ../global_vs30.grd ../Slope/global_vs30.grd DIV = $@

######################################################################################
# Make the plots.

vs30_ratio : vs30_ratio.png

# Set some initial flags for plotting. Edit these to change the plotting specs for
# every map below, or just edit them individually lower down. 

Jflags = M9i		# Set the projection. Mercator, 9 inches wide.
Bflags1 = a2WSen	# Axis labels for the whole plot. One label every 2 degrees, only plot on W and S side. 
Bflags2 = af		# Axis labels for the scale. 
Dflags = 24/4.3/9/0.5	# Position, length, and width of the scale.
	
vs30_ratio.png : vs30_ratio.grd
	gmt grdimage $< -J$(Jflags) -R$(SMALLER_REGION) -C$(AMP_CPT) -B$(Bflags1) -K > vs30_ratio.ps
	gmt psscale -D$(Dflags) -B$(Bflags2) -C$(AMP_CPT) -O >> vs30_ratio.ps
	convert -trim -rotate 90 vs30_ratio.ps vs30_ratio.png
	rm gmt.history
