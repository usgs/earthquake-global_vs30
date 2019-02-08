# This Makefile creates an amplification map from the global Vs30 map. 

# This will eventually work for all periods and PGA/PGV

include ~/.vs30/Constants.mk

GLOBAL_REGION = -180/180/-56/84
SMALLER_REGION = -130/-110/50/60

all : amp.grd

plots : ln_flin_plot FV_plot clipmask_smooth final_mask final_mask_2 amp

clean : clean_plots
	$(RM) clipmask.grd clipmask_smooth.grd final_mask.grd mask_a.grd new_mask* landmask_smooth.grd *.aux.xml \
        amp_values.grd flin_lt.grd gmt.history ln_flin*.grd *_Vref.grd *FV* \
        gt_V2.grd le_V1.grd le_V2_gt_V1.grd FLIP_* combo_glob.grd amp.grd clipmask_* final_mask_2.grd \
        fix_line* landmask* mask_a_2* pre_weights.grd weights.grd stable_regions.grd active_regions.grd \
        active_glob.grd stable_glob.grd 

clean_plots : 
	$(RM) *.ps *.png

veryclean : clean

##################################
# Make the final Amplification map.

amp.grd : fix_line_3.grd combo_glob.grd
	gmt grdmath fix_line_3.grd combo_glob.grd MUL = $@

##################################
# Combine the two clipping masks into one.

# Make anything less than 0.5 = 0.5 and set water back to 0.

fix_line_3.grd : landmask_land.grd fix_line_2.grd
	gmt grdmath landmask_land.grd fix_line_2.grd MUL = $@

fix_line_2.grd : fix_line.grd weights.grd
	gmt grdmath weights.grd 0.5 GT weights.grd MUL fix_line.grd ADD = $@

fix_line.grd : weights.grd
	gmt grdmath $< 0.5 LT 0.5 MUL = $@

weights.grd : pre_weights.grd final_mask_2.grd active_regions.grd
	gmt grdmath final_mask_2.grd active_regions.grd MUL pre_weights.grd ADD = $@

pre_weights.grd : final_mask.grd stable_regions.grd
	gmt grdmath final_mask.grd stable_regions.grd MUL = $@

##################################
# Make the SECOND weighted clipping mask.

#
# final_mask.grd is plotted.
#

final_mask_2.grd : new_mask_mul_landmask_add_a_2.grd
	gmt grdmath $< DUP 0 GE MUL = $@

#
# new_mask_mul_landmask_add_a.grd is plotted.
#

new_mask_mul_landmask_add_a_2.grd : mask_a_2.grd new_mask_mul_landmask_2.grd
	gmt grdmath mask_a_2.grd new_mask_mul_landmask_2.grd ADD 1 SUB = $@

new_mask_mul_landmask_2.grd : new_mask_2.grd landmask_land.grd
	gmt grdmath new_mask_2.grd landmask_land.grd MUL = $@
#
# new_mask.grd is plotted.
#

new_mask_2.grd : active_regions.grd landmask_smooth.grd
	gmt grdmath active_regions.grd DUP NOT landmask_smooth.grd MUL ADD = $@

landmask_smooth.grd : landmask_land.grd ../src/smooth
	../src/smooth infile=landmask_land.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

#
# mask_a.grd is plotted.
#

mask_a_2.grd : clipmask_smooth_2.grd landmask_land.grd
	gmt grdmath clipmask_smooth_2.grd landmask_land.grd MUL = $@

#
# clipmask_smooth.grd is plotted.
#

clipmask_smooth_2.grd : clipmask_2.grd ../src/smooth
	../src/smooth infile=clipmask_2.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

clipmask_2.grd : landmask_water.grd active_regions.grd
	gmt grdmath landmask_water.grd active_regions.grd ADD 0 GT 1 AND = $@






##################################
# Make the FIRST weighted clipping mask.

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

new_mask.grd : stable_regions.grd landmask_smooth.grd
	gmt grdmath stable_regions.grd DUP NOT landmask_smooth.grd MUL ADD = $@

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

clipmask.grd : landmask_water.grd stable_regions.grd
	gmt grdmath landmask_water.grd stable_regions.grd ADD 0 GT 1 AND = $@

#gr_non_zero.grd : gr_$(RES)c.grd
#gmt grdmath $< 0 GT 0 AND = $@

##################################
# Make flipped maps - put the active amp. values in stable regions and vice versa.

FLIP_combo_glob.grd : FLIP_stable_glob.grd FLIP_active_glob.grd
	gmt grdmath FLIP_stable_glob.grd FLIP_active_glob.grd ADD = $@

FLIP_stable_glob.grd : FV.grd stable_regions.grd landmask_land.grd
	gmt grdmath stable_regions.grd 0 EQ FV.grd MUL landmask_land.grd MUL = $@

FLIP_active_glob.grd : ln_flin.grd stable_regions.grd landmask_land.grd
	gmt grdmath stable_regions.grd ln_flin.grd MUL landmask_land.grd MUL = $@

##################################
# Combining the acitive and stable region maps.

combo_glob.grd : active_glob.grd stable_glob.grd
	gmt grdmath active_glob.grd stable_glob.grd ADD = $@

##################################
# For active crustal regions there following equations are used to model 
# amplification values from variable Vs30.

# Frist, just define the variables. Later, this should read in arrays and pick all
# the variables based on just period. 

# This is for the linear term.
# We'll default to a .2 sec period for now.

T = 1.0
c = -1.0500
Vc = 1109.95
Vref = 760

active_glob.grd : ln_flin.grd stable_regions.grd landmask_land.grd
	gmt grdmath stable_regions.grd 0 EQ ln_flin.grd MUL landmask_land.grd MUL = $@

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

V1 = 278
V2 = 1103
Vref = 760
c = -0.554

stable_glob.grd : FV.grd stable_regions.grd landmask_land.grd
	gmt grdmath FV.grd stable_regions.grd MUL landmask_land.grd MUL = $@

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

#######################################

# Invert the stable_regions.grd file to get an active_regions.grd file.

active_regions.grd : stable_regions.grd
	gmt grdmath $< -1 MUL 1 ADD = $@

# Convert the stable_4.grd file into correct resolution and gridline node registration.

stable_regions.grd : stable_4.grd
	gmt grdsample -nl+t0.1 -I$(RES)s+e/$(RES)s+e -fg -R$(GLOBAL_REGION) $< -G$@

landmask_land.grd : 
	gmt grdlandmask -R$(GLOBAL_REGION) -I$(RES)s/$(RES)s -N0/1/0/1/0 -G$@

landmask_water.grd :
	gmt grdlandmask -N1/0/1/0/1 -R$(GLOBAL_REGION) -I$(RES)s/$(RES)s -G$@

../src/smooth :
	$(MAKE) -C ../src smooth

#######################################
# Make the plots.

ln_flin_plot : ln_flin_plot.png

FV_plot : FV_plot.png

clipmask_smooth : clipmask_smooth.png

final_mask : final_mask.png

final_mask_2 : final_mask_2.png

amp : amp.png

ln_flin_plot.png : ln_flin.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(AMP_CPT) -Bpx8Snew -Bpy4Wnes -K > ln_flin_plot.ps
	gmt psscale -D14/4.3/9/0.5 -Baf -C$(AMP_CPT) -O -K >> ln_flin_plot.ps
#	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> ln_flin_plot.ps
	rm gmt.history
	convert -rotate 90 ln_flin_plot.ps -strip ln_flin_plot.png

FV_plot.png : FV.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(AMP_CPT) -Bpx8Snew -Bpy4Wnes -K > FV_plot.ps
	gmt psscale -D14/4.3/9/0.5 -Baf -C$(AMP_CPT) -O -K >> FV_plot.ps
#	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> FV_plot.ps
	rm gmt.history
	convert -rotate 90 FV_plot.ps -strip FV_plot.png

clipmask_smooth.png : clipmask_smooth.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > clipmask_smooth.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> clipmask_smooth.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> clipmask_smooth.ps
	convert -trim -rotate 90 clipmask_smooth.ps clipmask_smooth.png

final_mask.png : final_mask.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > final_mask.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> final_mask.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> final_mask.ps
	convert -trim -rotate 90 final_mask.ps final_mask.png

final_mask_2.png : final_mask_2.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > final_mask_2.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> final_mask_2.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> final_mask_2.ps
	convert -trim -rotate 90 final_mask_2.ps final_mask_2.png

amp.png : amp.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > amp.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> amp.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> amp.ps
	convert -trim -rotate 90 amp.ps amp.png
