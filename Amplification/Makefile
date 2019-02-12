# This Makefile creates an amplification map from the global Vs30 map. 

# This will eventually work for all periods and PGA/PGV

include ~/.vs30/Constants.mk

GLOBAL_REGION = -180/180/-56/84
<<<<<<< HEAD
SMALLER_REGION = -128/-112/52/58

all : amplification_blend.grd

plots : combo_glob FLIP_combo_glob final_mask final_mask_2 fix_line_3 amplification_blend

# Each line in the make clean protocol is a different major step in creating the Amplification map. 

clean : clean_plots
	$(RM) landmask_land.grd landmask_water.grd \
	active_regions.grd \
	le_V1.grd FV_le_V1.grd le_V2_gt_V1.grd FV_le_V2_gt_V1.grd gt_V2.grd FV_gt_V2.grd FV.grd stable_glob.grd \
	le_Vref.grd ln_flin_le.grd gt_Vref.grd ln_flin_gt.grd ln_flin.grd active_glob.grd \
	combo_glob.grd \
	FLIP_active_glob.grd FLIP_stable_glob.grd FLIP_combo_glob.grd \
	clipmask.grd clipmask_smooth.grd mask_a.grd landmask_smooth.grd new_mask.grd new_mask_mul_landmask.grd new_mask_mul_landmask_add_a.grd final_mask.grd \
	clipmask_2.grd clipmask_smooth_2.grd mask_a_2.grd landmask_smooth.grd new_mask_2.grd new_mask_mul_landmask_2.grd new_mask_mul_landmask_add_a_2.grd final_mask_2.grd \
	pre_weights.grd weights.grd \
	fix_line.grd fix_line_2.grd fix_line_3.grd \
	amplification_blend.grd
=======
SMALLER_REGION = -130/-110/50/60

all : amp.grd

plots : ln_flin_plot FV_plot clipmask_smooth final_mask final_mask_2 amp

clean : clean_plots
	$(RM) clipmask.grd clipmask_smooth.grd final_mask.grd mask_a.grd new_mask* landmask_smooth.grd *.aux.xml \
        amp_values.grd flin_lt.grd gmt.history ln_flin*.grd *_Vref.grd *FV* \
        gt_V2.grd le_V1.grd le_V2_gt_V1.grd FLIP_* combo_glob.grd amp.grd clipmask_* final_mask_2.grd \
        fix_line* landmask* mask_a_2* pre_weights.grd weights.grd stable_regions.grd active_regions.grd \
        active_glob.grd stable_glob.grd 
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

clean_plots : 
	$(RM) *.ps *.png

veryclean : clean

<<<<<<< HEAD
##################################################################
# Make the final Amplification map.

amplification_blend.grd : ../src/insert_grd FLIP_combo_glob.grd \
	combo_glob.grd fix_line_3.grd
	../src/insert_grd gin=FLIP_combo_glob.grd gout=$@ \
		grid1=combo_glob.grd gmask1=fix_line_3.grd

##################################################################
# Fix the artificial line that is somehow put in when combining the two weighted clipping masks.
# Make anything less than 0.5 equal to 0.5 and set water back to 0.
=======
##################################
# Make the final Amplification map.

amp.grd : fix_line_3.grd combo_glob.grd
	gmt grdmath fix_line_3.grd combo_glob.grd MUL = $@

##################################
# Combine the two clipping masks into one.

# Make anything less than 0.5 = 0.5 and set water back to 0.
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

fix_line_3.grd : landmask_land.grd fix_line_2.grd
	gmt grdmath landmask_land.grd fix_line_2.grd MUL = $@

fix_line_2.grd : fix_line.grd weights.grd
	gmt grdmath weights.grd 0.5 GT weights.grd MUL fix_line.grd ADD = $@

fix_line.grd : weights.grd
	gmt grdmath $< 0.5 LT 0.5 MUL = $@

<<<<<<< HEAD
##########################################################################
# Create the weighted clipping mask by combining the two masks into one.

=======
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
weights.grd : pre_weights.grd final_mask_2.grd active_regions.grd
	gmt grdmath final_mask_2.grd active_regions.grd MUL pre_weights.grd ADD = $@

pre_weights.grd : final_mask.grd stable_regions.grd
	gmt grdmath final_mask.grd stable_regions.grd MUL = $@

<<<<<<< HEAD
########################################################################
# Make the SECOND weighted clipping mask.

final_mask_2.grd : new_mask_mul_landmask_add_a_2.grd
	gmt grdmath $< DUP 0 GE MUL = $@

=======
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

>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
new_mask_mul_landmask_add_a_2.grd : mask_a_2.grd new_mask_mul_landmask_2.grd
	gmt grdmath mask_a_2.grd new_mask_mul_landmask_2.grd ADD 1 SUB = $@

new_mask_mul_landmask_2.grd : new_mask_2.grd landmask_land.grd
	gmt grdmath new_mask_2.grd landmask_land.grd MUL = $@
<<<<<<< HEAD
=======
#
# new_mask.grd is plotted.
#
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

new_mask_2.grd : active_regions.grd landmask_smooth.grd
	gmt grdmath active_regions.grd DUP NOT landmask_smooth.grd MUL ADD = $@

<<<<<<< HEAD
mask_a_2.grd : clipmask_smooth_2.grd landmask_land.grd
	gmt grdmath clipmask_smooth_2.grd landmask_land.grd MUL = $@

=======
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

>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
clipmask_smooth_2.grd : clipmask_2.grd ../src/smooth
	../src/smooth infile=clipmask_2.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

clipmask_2.grd : landmask_water.grd active_regions.grd
	gmt grdmath landmask_water.grd active_regions.grd ADD 0 GT 1 AND = $@

<<<<<<< HEAD
########################################################################
# Make the FIRST weighted clipping mask.
# This is the same workflow as seen in Makefiles for Greece, Italy, etc.
# Please see the comments and plots in those Makefiles for more details.
=======
##################################
# Make the FIRST weighted clipping mask.

#
# final_mask.grd is plotted.
#
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

final_mask.grd : new_mask_mul_landmask_add_a.grd
	gmt grdmath $< DUP 0 GE MUL = $@

<<<<<<< HEAD
=======
#
# new_mask_mul_landmask_add_a.grd is plotted.
#

>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
new_mask_mul_landmask_add_a.grd : mask_a.grd new_mask_mul_landmask.grd
	gmt grdmath mask_a.grd new_mask_mul_landmask.grd ADD 1 SUB = $@

new_mask_mul_landmask.grd : new_mask.grd landmask_land.grd
	gmt grdmath new_mask.grd landmask_land.grd MUL = $@

<<<<<<< HEAD
=======
#
# new_mask.grd is plotted.
#

>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
new_mask.grd : stable_regions.grd landmask_smooth.grd
	gmt grdmath stable_regions.grd DUP NOT landmask_smooth.grd MUL ADD = $@

landmask_smooth.grd : landmask_land.grd ../src/smooth
	../src/smooth infile=landmask_land.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

<<<<<<< HEAD
mask_a.grd : clipmask_smooth.grd landmask_land.grd
	gmt grdmath clipmask_smooth.grd landmask_land.grd MUL = $@

=======
#
# mask_a.grd is plotted.
#

mask_a.grd : clipmask_smooth.grd landmask_land.grd
	gmt grdmath clipmask_smooth.grd landmask_land.grd MUL = $@

#
# clipmask_smooth.grd is plotted.
#

>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
clipmask_smooth.grd : clipmask.grd ../src/smooth
	../src/smooth infile=clipmask.grd fx=$(REGION_FX) fy=$(REGION_FY) outfile=$@

clipmask.grd : landmask_water.grd stable_regions.grd
	gmt grdmath landmask_water.grd stable_regions.grd ADD 0 GT 1 AND = $@

<<<<<<< HEAD
#########################################################################################
=======
##################################
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
# Make flipped maps - put the active amp. values in stable regions and vice versa.

FLIP_combo_glob.grd : FLIP_stable_glob.grd FLIP_active_glob.grd
	gmt grdmath FLIP_stable_glob.grd FLIP_active_glob.grd ADD = $@

FLIP_stable_glob.grd : FV.grd stable_regions.grd landmask_land.grd
	gmt grdmath stable_regions.grd 0 EQ FV.grd MUL landmask_land.grd MUL = $@

FLIP_active_glob.grd : ln_flin.grd stable_regions.grd landmask_land.grd
	gmt grdmath stable_regions.grd ln_flin.grd MUL landmask_land.grd MUL = $@

<<<<<<< HEAD
########################################################################################
# Combining the acitive and stable region maps without smoothing at the interface.
=======
##################################
# Combining the acitive and stable region maps.
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

combo_glob.grd : active_glob.grd stable_glob.grd
	gmt grdmath active_glob.grd stable_glob.grd ADD = $@

<<<<<<< HEAD
########################################################################################
# For active crustal regions there following equations are used to model 
# amplification values from variable Vs30.

# This is based on the methodology from Seyhan and Stewart (2014).

# Frist, just define the variables. Later, this should read in arrays and pick all
# the variables based on just period. For now, just do it manually.

# This is for the linear term, a non-linear term can be added later.
# We'll default to a 1.0 sec period for now.
=======
##################################
# For active crustal regions there following equations are used to model 
# amplification values from variable Vs30.

# Frist, just define the variables. Later, this should read in arrays and pick all
# the variables based on just period. 

# This is for the linear term.
# We'll default to a .2 sec period for now.
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

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

<<<<<<< HEAD
###################################################################################
# For stable cratonic regions we use the following relationship, based on the
# methodology from Stewart et al. (2017).

# Corner frequencies V1 and V2, as well as c, are a function of period.
# For now, define these variables manually. In the future, they will be read in automatically. 
=======
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
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

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

<<<<<<< HEAD
######################################################################################
=======
#######################################

>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
# Invert the stable_regions.grd file to get an active_regions.grd file.

active_regions.grd : stable_regions.grd
	gmt grdmath $< -1 MUL 1 ADD = $@

<<<<<<< HEAD
######################################################################################
# Use grdlandmask to create masks where all land is 1 and water is 0 and vice versa.
=======
# Convert the stable_4.grd file into correct resolution and gridline node registration.

stable_regions.grd : stable_4.grd
	gmt grdsample -nl+t0.1 -I$(RES)s+e/$(RES)s+e -fg -R$(GLOBAL_REGION) $< -G$@
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

landmask_land.grd : 
	gmt grdlandmask -R$(GLOBAL_REGION) -I$(RES)s/$(RES)s -N0/1/0/1/0 -G$@

landmask_water.grd :
	gmt grdlandmask -N1/0/1/0/1 -R$(GLOBAL_REGION) -I$(RES)s/$(RES)s -G$@

<<<<<<< HEAD
######################################################################################
# Compile the insert_grd and smooth functions. 

../src/insert_grd :
	$(MAKE) -C ../src insert_grd

../src/smooth :
	$(MAKE) -C ../src smooth

######################################################################################
# Make the plots.

combo_glob : combo_glob.png

FLIP_combo_glob : FLIP_combo_glob.png
=======
../src/smooth :
	$(MAKE) -C ../src smooth

#######################################
# Make the plots.

ln_flin_plot : ln_flin_plot.png

FV_plot : FV_plot.png

clipmask_smooth : clipmask_smooth.png
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c

final_mask : final_mask.png

final_mask_2 : final_mask_2.png

<<<<<<< HEAD
fix_line_3 : fix_line_3.png

combo_glob : combo_glob.png

amplification_blend : amplification_blend.png

combo_glob.png : combo_glob.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(AMP_CPT) -Bpx8Snew -Bpy4Wnes -K > combo_glob.ps
	gmt psscale -D14/4.3/9/0.5 -Baf -C$(AMP_CPT) -O -K >> combo_glob.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> combo_glob.ps
	convert -rotate 90 combo_glob.ps -strip combo_glob.png

FLIP_combo_glob.png : FLIP_combo_glob.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(AMP_CPT) -Bpx8Snew -Bpy4Wnes -K > FLIP_combo_glob.ps
	gmt psscale -D14/4.3/9/0.5 -Baf -C$(AMP_CPT) -O -K >> FLIP_combo_glob.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> FLIP_combo_glob.ps
	convert -rotate 90 FLIP_combo_glob.ps -strip FLIP_combo_glob.png

final_mask.png : final_mask.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(NEW_WEIGHTS_CPT) -Ba2WSen -K > final_mask.ps
=======
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
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> final_mask.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> final_mask.ps
	convert -trim -rotate 90 final_mask.ps final_mask.png

final_mask_2.png : final_mask_2.grd
<<<<<<< HEAD
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(NEW_WEIGHTS_CPT) -Ba2WSen -K > final_mask_2.ps
=======
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(NEW_WEIGHTS_CPT) -Ba1WSen -K > final_mask_2.ps
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> final_mask_2.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> final_mask_2.ps
	convert -trim -rotate 90 final_mask_2.ps final_mask_2.png

<<<<<<< HEAD
fix_line_3.png : fix_line_3.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(NEW_WEIGHTS_CPT) -Ba2WSen -K > fix_line_3.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(NEW_WEIGHTS_CPT) -O -K >> fix_line_3.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> fix_line_3.ps
	convert -trim -rotate 90 fix_line_3.ps fix_line_3.png

amplification_blend.png : amplification_blend.grd
	gmt grdimage $< -JM9i -R$(SMALLER_REGION) -C$(AMP_CPT) -Ba2WSen -K > amplification_blend.ps
	gmt psscale -D24/4.3/9/0.5 -Ba1f1 -C$(AMP_CPT) -O >> amplification_blend.ps
	convert -trim -rotate 90 amplification_blend.ps amplification_blend.png
	rm gmt.history
=======
amp.png : amp.grd
	gmt grdimage $< -JM12 -R$(SMALLER_REGION) -C$(AMP_CPT) -Ba1WSen -K > amp.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(AMP_CPT) -O -K >> amp.ps
	gmt pscoast -JM12 -R$(SMALLER_REGION) -Df -O -N1 -N2 -W >> amp.ps
	convert -trim -rotate 90 amp.ps amp.png
>>>>>>> 5ba4b10b87adf1e5e23c7169433d34299213fe5c
