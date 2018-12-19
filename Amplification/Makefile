# This Makefile creates an amplification map from the global Vs30 map. 

# This will eventually work for all periods and PGA/PGV

include ~/.vs30/Constants.mk

GLOBAL_REGION = -180/180/-56/80
SMALLER_REGION = -160/-100/40/70

all : ln_flin.grd FV.grd

plots : ln_flin_plot FV_plot

clean : clean_plots
	$(RM) amp_values.grd flin_lt.grd gmt.history ln_flin*.grd *_Vref.grd *FV* \
        gt_V2.grd le_V1.grd le_V2_gt_V1.grd
clean_plots : 
	$(RM) *.ps *.png

veryclean : clean


# For active crustal regions there following equations are used to model 
# amplification values from variable Vs30.

# Frist, just define the variables. Later, this should read in arrays and pick all
# the variables based on just period. 

# This is for the linear term.
# We'll default to a .2 sec period for now.

T = 0.2
c = -0.6876
Vc = 1392.61
Vref = 760


ln_flin.grd : ln_flin_gt.grd ln_flin_le.grd
	gmt grdmath ln_flin_le.grd ln_flin_gt.grd ADD = $@

ln_flin_gt.grd : gt_Vref.grd
	gmt grdmath $(Vc) $(Vref) DIV LOG $(c) MUL gt_Vref.grd MUL = $@

gt_Vref.grd : ../global_vs30.grd
	gmt grdmath $< $(Vc) GT = $@

ln_flin_le.grd : ../global_vs30.grd le_Vref.grd
	gmt grdmath ../global_vs30.grd $(Vref) DIV LOG $(c) MUL le_Vref.grd MUL = $@

le_Vref.grd : ../global_vs30.grd
	gmt grdmath $< $(Vc) LE = $@ 

# These are the variables for the non-linear term.
# The non-linear term is only useful for very large shaking. For small PGA values, 
# it is scaled to be effectively negligible. 

f1 = 0
f3 = 0.1
f4 = -0.2466
f5 = -0.00614

#f2 = f4 * exp(f5 * min(Vs30 or 760) - 360))

#ln_Fnl = f1 + f2 * ln((PGAr + f3)/f3)


#################
# For stable cratonic regions we use the following relationship. 

# Corner frequencies V1 and V2, as well as c, are a function of period.

V1 = 314
V2 = 760
Vref = 760
c = -0.408

FV.grd : FV_gt_V2.grd FV_le_V2_gt_V1.grd FV_le_V1.grd
	gmt grdmath FV_gt_V2.grd FV_le_V2_gt_V1.grd ADD FV_le_V1.grd ADD = $@

FV_gt_V2.grd : gt_V2.grd ../global_vs30.grd
	gmt grdmath ../global_vs30.grd $(V2) DIV LOG $(c) MUL 2 DIV $(V2) $(Vref) DIV LOG $(c) MUL ADD = $@

gt_V2.grd : ../global_vs30.grd
	gmt grdmath $< $(V2) GT = $@

FV_le_V2_gt_V1.grd : le_V2_gt_V1.grd ../global_vs30.grd
	gmt grdmath ../global_vs30.grd $(Vref) DIV LOG $(c) MUL = $@

le_V2_gt_V1.grd :../global_vs30.grd
	gmt grdmath $< $(V2) LE $< MUL $(V1) GT = $@

FV_le_V1.grd : le_V1.grd
	gmt grdmath $(V1) $(Vref) DIV LOG $(c) MUL $< MUL = $@

le_V1.grd : ../global_vs30.grd
	gmt grdmath $< $(V1) LE = $@

# Make the plots.

ln_flin_plot : ln_flin_plot.png

FV_plot : FV_plot.png

ln_flin_plot.png : ln_flin.grd
	gmt grdimage ln_flin.grd -JM12 -R$(SMALLER_REGION) -C$(AMP_CPT) -Bpx8Snew -Bpy4Wnes -K > ln_flin_plot.ps
	gmt psscale -D14/4.3/9/0.5 -Baf -C$(AMP_CPT) -O -K >> ln_flin_plot.ps
#	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> ln_flin_plot.ps
	rm gmt.history
	convert -rotate 90 ln_flin_plot.ps -strip ln_flin_plot.png

FV_plot.png : FV.grd
	gmt grdimage FV.grd -JM12 -R$(SMALLER_REGION) -C$(AMP_CPT) -Bpx8Snew -Bpy4Wnes -K > FV_plot.ps
	gmt psscale -D14/4.3/9/0.5 -Baf -C$(AMP_CPT) -O -K >> FV_plot.ps
#	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> FV_plot.ps
	rm gmt.history
	convert -rotate 90 FV_plot.ps -strip FV_plot.png
