# This Makefile creates an amplification map from the global Vs30 map. 

# This will eventually work for all periods and PGA/PGV

include ~/.vs30/Constants.mk

GLOBAL_REGION = -180/180/-56/84
SMALLER_REGION = -128/-112/52/58

all : combo_glob.grd hybrid_amplification_blend.grd

plots : combo_glob final_mask hybrid_amplification_blend

# Each line in the make clean protocol is a different major step in creating the Amplification map. 

clean : clean_plots
	$(RM) landmask_land.grd landmask_water.grd \
	active_regions.grd \
	le_V1.grd FV_le_V1.grd le_V2_gt_V1.grd FV_le_V2_gt_V1.grd gt_V2.grd FV_gt_V2*.grd ln_FV.grd FV.grd stable_glob.grd \
	le_Vref.grd ln_flin_le.grd gt_Vref.grd ln_flin_gt.grd ln_flin.grd flin.grd active_glob.grd \
	combo_glob.grd \
	clipmask.grd clipmask_smooth.grd mask_a.grd landmask_smooth.grd new_mask.grd new_mask_mul_landmask.grd new_mask_mul_landmask_add_a.grd final_mask.grd \
	hybrid_amplification_blend.grd

clean_plots : 
	$(RM) *.ps *.png gmt.history

veryclean : clean

##################################################################
# Make the final Amplification map.

hybrid_amplification_blend.grd : ../../src/insert_grd flin.grd \
	FV.grd final_mask.grd
	../../src/insert_grd gin=flin.grd gout=$@ \
		grid1=FV.grd gmask1=final_mask.grd

########################################################################
# Make the weighted clipping mask.
# This is the same workflow as seen in Makefiles for Greece, Italy, etc.
# Please see the comments and plots in those Makefiles for more details.

final_mask.grd : new_mask_mul_landmask_add_a.grd
	gmt grdmath $< DUP 0 GT MUL = $@

new_mask_mul_landmask_add_a.grd : mask_a.grd new_mask_mul_landmask.grd
	gmt grdmath mask_a.grd new_mask_mul_landmask.grd ADD 1 SUB = $@

new_mask_mul_landmask.grd : new_mask.grd landmask_land.grd
	gmt grdmath new_mask.grd landmask_land.grd MUL = $@

new_mask.grd : clipmask.grd landmask_smooth.grd
	gmt grdmath clipmask.grd DUP NOT landmask_smooth.grd MUL ADD = $@

landmask_smooth.grd : landmask_land.grd ../../src/smooth
	../../src/smooth infile=landmask_land.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

mask_a.grd : clipmask_smooth.grd landmask_land.grd
	gmt grdmath clipmask_smooth.grd landmask_land.grd MUL = $@

clipmask_smooth.grd : clipmask.grd ../../src/smooth
	../../src/smooth infile=clipmask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

clipmask.grd : landmask_water.grd stable_regions.grd
	gmt grdmath landmask_water.grd stable_regions.grd ADD 0 GT 1 AND 1 GE = $@

########################################################################################
# Combining the acitive and stable region maps without smoothing at the interface.

combo_glob.grd : active_glob.grd stable_glob.grd
	gmt grdmath active_glob.grd stable_glob.grd ADD = $@

########################################################################################
# For active crustal regions there following equations are used to model 
# amplification values from variable Vs30.

# This is based on the methodology from Seyhan and Stewart (2014).

# Frist, just define the variables. Later, this should read in arrays and pick all
# the variables based on just period. For now, just do it manually.

# This is for the linear term, a non-linear term can be added later.
# We'll default to a 1.0 sec period for now.

T = 1.0
c_active = -1.0500
Vc = 1109.95
Vref = 760

active_glob.grd : flin.grd stable_regions.grd landmask_land.grd
	gmt grdmath stable_regions.grd 0 EQ flin.grd MUL = $@

flin.grd : ln_flin.grd
	gmt grdmath 2.71828 $< POW = $@

ln_flin.grd : ln_flin_gt.grd ln_flin_le.grd
	gmt grdmath ln_flin_le.grd ln_flin_gt.grd ADD = $@

ln_flin_gt.grd : gt_Vref.grd
	gmt grdmath $(Vc) $(Vref) DIV LOG $(c_active) MUL gt_Vref.grd MUL = $@

gt_Vref.grd : ../../global_vs30.grd
	gmt grdmath $< $(Vc) GT = $@

ln_flin_le.grd : ../../global_vs30.grd le_Vref.grd
	gmt grdmath ../../global_vs30.grd $(Vref) DIV LOG $(c_active) MUL le_Vref.grd MUL = $@

le_Vref.grd : ../../global_vs30.grd
	gmt grdmath $< $(Vc) LE = $@ 

###################################################################################
# For stable cratonic regions we use the following relationship, based on the
# methodology from Stewart et al. (2017).

# Corner frequencies V1 and V2, as well as c, are a function of period.
# For now, define these variables manually. In the future, they will be read in automatically. 

V1 = 278
V2 = 1103
Vref = 760
c_stable = -0.554

stable_glob.grd : FV.grd stable_regions.grd landmask_land.grd
	gmt grdmath FV.grd stable_regions.grd MUL = $@

FV.grd : ln_FV.grd
	gmt grdmath 2.71828 $< POW = $@

ln_FV.grd : FV_gt_V2_2.grd FV_le_V2_gt_V1.grd FV_le_V1.grd
	gmt grdmath FV_gt_V2_2.grd FV_le_V2_gt_V1.grd ADD FV_le_V1.grd ADD = $@

FV_gt_V2_2.grd : FV_gt_V2.grd gt_V2.grd
	gmt grdmath $(V2) $(Vref) DIV LOG $(c_stable) MUL FV_gt_V2.grd ADD gt_V2.grd MUL = $@

FV_gt_V2.grd : gt_V2.grd ../../global_vs30.grd
	gmt grdmath ../../global_vs30.grd $(V2) DIV LOG $(c_stable) MUL 2 DIV = $@

gt_V2.grd : ../../global_vs30.grd
	gmt grdmath $< $(V2) GT = $@

FV_le_V2_gt_V1.grd : le_V2_gt_V1.grd ../../global_vs30.grd
	gmt grdmath ../../global_vs30.grd $(Vref) DIV LOG $(c_stable) MUL le_V2_gt_V1.grd MUL = $@

le_V2_gt_V1.grd : ../../global_vs30.grd
	gmt grdmath $< $(V2) LE $< MUL $(V1) GT = $@

FV_le_V1.grd : le_V1.grd
	gmt grdmath $(V1) $(Vref) DIV LOG $(c_stable) MUL $< MUL = $@

le_V1.grd : ../../global_vs30.grd
	gmt grdmath $< $(V1) LE = $@

######################################################################################
# Invert the stable_regions.grd file to get an active_regions.grd file, where the
# active crustal regions are 1 and stable is 0.

active_regions.grd : stable_regions.grd
	gmt grdmath $< -1 MUL 1 ADD = $@

######################################################################################
# Use grdlandmask to create masks where all land is 1 and water is 0 and vice versa.

landmask_land.grd : 
	gmt grdlandmask -R$(GLOBAL_REGION) -I$(RES)s/$(RES)s -N0/1/1/1/1 -G$@

landmask_water.grd :
	gmt grdlandmask -N1/0/0/0/0 -R$(GLOBAL_REGION) -I$(RES)s/$(RES)s -G$@

######################################################################################
# Compile the insert_grd and smooth functions. 

../../src/insert_grd :
	$(MAKE) -C ../../src insert_grd

../../src/smooth :
	$(MAKE) -C ../../src smooth

######################################################################################
# Make the plots.

combo_glob : combo_glob.png

final_mask : final_mask.png

hybrid_amplification_blend : hybrid_amplification_blend.png

# Set some initial flags for plotting. Edit these to change the plotting specs for
# every map below, or just edit them individually lower down. 

Jflags = M9i		# Set the projection. Mercator, 9 inches wide.
Bflags1 = a2WSen	# Axis labels for the whole plot. One label every 2 degrees, only plot on W and S side. 
Bflags2 = af		# Axis labels for the scale. 
Dflags = 24/4.3/9/0.5	# Position, length, and width of the scale.

combo_glob.png : combo_glob.grd
	gmt grdimage $< -J$(Jflags) -R$(SMALLER_REGION) -C../$(AMP_CPT) -B$(Bflags1) -K > combo_glob.ps
	gmt psscale -D$(Dflags) -B$(Bflags2) -C../$(AMP_CPT) -O >> combo_glob.ps
	convert -rotate 90 combo_glob.ps -strip combo_glob.png

final_mask.png : final_mask.grd
	gmt grdimage $< -J$(Jflags) -R$(SMALLER_REGION) -C../$(NEW_WEIGHTS_CPT) -B$(Bflags1) -K > final_mask.ps
	gmt psscale -D$(Dflags) -B$(Bflags2) -C../$(NEW_WEIGHTS_CPT) -O -K >> final_mask.ps
	gmt pscoast -J$(Jflags) -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> final_mask.ps
	convert -trim -rotate 90 final_mask.ps final_mask.png

hybrid_amplification_blend.png : hybrid_amplification_blend.grd
	gmt grdimage $< -J$(Jflags) -R$(SMALLER_REGION) -C../$(AMP_CPT) -B$(Bflags1) -K > hybrid_amplification_blend.ps
	gmt psscale -D$(Dflags) -B$(Bflags2) -C../$(AMP_CPT) -O >> hybrid_amplification_blend.ps
	convert -trim -rotate 90 hybrid_amplification_blend.ps hybrid_amplification_blend.png
	rm gmt.history
