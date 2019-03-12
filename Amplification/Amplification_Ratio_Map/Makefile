# This Makefile creates a ratio map from the Hybrid/Slope amplification maps. 

include ~/.vs30/Constants.mk

GLOBAL_REGION = -180/180/-56/84
SMALLER_REGION = -128/-100/35/48

all :  amplification_ratio.grd

plots : amplification_ratio

clean : clean_plots
	$(RM) amplification_ratio.grd

clean_plots : 
	$(RM) *.ps *.png

veryclean : clean

##################################################################
# Make the Amplification Hybrid/Slope ratio map.

amplification_ratio.grd : ../Hybrid_Amplification/hybrid_amplification_blend.grd ../Slope_Amplification/slope_amplification_blend.grd
	gmt grdmath ../Hybrid_Amplification/hybrid_amplification_blend.grd ../Slope_Amplification/slope_amplification_blend.grd DIV = $@

######################################################################################
# Make the plots.

amplification_ratio : amplification_ratio.png

# Set some initial flags for plotting. Edit these to change the plotting specs for
# every map below, or just edit them individually lower down. 

Jflags = M9i		# Set the projection. Mercator, 9 inches wide.
Bflags1 = a2WSen	# Axis labels for the whole plot. One label every 2 degrees, only plot on W and S side. 
Bflags2 = af		# Axis labels for the scale. 
Dflags = 24/4.3/9/0.5	# Position, length, and width of the scale.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

amplification_ratio.png : amplification_ratio.grd
	gmt grdimage $< -J$(Jflags) -R$(SMALLER_REGION) -C../$(RATIO_CPT) -B$(Bflags1) -K > amplification_ratio.ps
	gmt psscale -D$(Dflags) -B$(Bflags2) -C../$(RATIO_CPT) -O >> amplification_ratio.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) amplification_ratio.ps
	rm gmt.history
