#
# Makefile to generate a global slope-based Vs30 file
#
include ../Constants.mk

# Don't mess with this -- the blank lines are necessary
define N


endef

.PHONY: all clean veryclean

OLD_GRD = $(wildcard ../global_vs30_*.grd)

ifneq ($(strip $(OLD_GRD)),)
all : 
	$(warning Skipping Slope processing: The global Vs30 directory\
                $Nalready contains a global grid file with one or more regional stamps.\
                $NIf you want to start fresh, remove '$(OLD_GRD)')
else
all : ../global_vs30.grd
endif

../global_vs30.grd : global_vs30.grd
	cp global_vs30.grd ..

global_vs30.grd : global_grad.grd global_landmask.grd cratons_smooth.grd \
                  ../src/grad2vs30 vs30.par
	../src/grad2vs30 par=vs30.par water=$(WATER)

clean :
	$(RM) cratons.bil cratons.hdr cratons.grd cratons_smooth.grd \
              global_landmask.grd elev/gmted_global.bil gmted_global.grd \
              global_grad.grd cratons_pixel.grd

veryclean : clean
	$(RM) -rf elev
	$(RM) -rf cratons
	$(RM) global_vs30.grd

#######################
#
# Create the slope file from the DEM (the -G option isn't necessary on 
# newer versions of GMT):
#
global_grad.grd : gmted_global.grd
	grdgradient gmted_global.grd -Lg -M -D -Sglobal_grad.grd -Gjunk.grd -V
	$(RM) junk.grd

#
# Convert .bil to pixel registered .grd, then convert it to gridline 
# node registration:
#
gmted_global.grd : elev/gmted_global.bil
	xyz2grd -Zh -F -R$(GLOBAL_REGION) -I$(RES)c elev/gmted_global.bil \
                -Ggmted_global_pixel.grd
	grdsample gmted_global_pixel.grd -Ggmted_global.grd -T -fg
	$(RM) gmted_global_pixel.grd

#
# Convert the DEM to .bil:
#
elev/gmted_global.bil : elev/md30_grd
	gdal_translate -of EHdr elev/md30_grd elev/gmted_global.bil

#
# Unzip the DEM; we have to touch the dir and files because they
# are older than the archive that contains them, forcing make
# to always repeat this (and all following) step(s)
#
elev/md$(RES)_grd : elev/md$(RES)_grd.zip
	(cd elev && unzip md$(RES)_grd.zip && touch -c md$(RES)_grd && \
         touch md$(RES)_grd/* )

#
# Get the DEM:
#
elev/md$(RES)_grd.zip :
	mkdir -p elev;
	$(CURL) http://topotools.cr.usgs.gov/gmted_viewer/data/Grid_ZipFiles/md$(RES)_grd.zip > elev/md$(RES)_grd.zip 

#
# Create the landmask:
#
global_landmask.grd : 
	grdlandmask -V -R$(GLOBAL_REGION) -I$(RES)c -Gglobal_landmask.grd -Df

#
# Smooth the craton file
#
cratons_smooth.grd : cratons.grd ../src/smooth
	../src/smooth par=smooth.par


#
# Convert the craton.bil to a pixel registered .grd file, then convert that to gridline 
# node registration; use -Qn to keep everything at 0 and 1:
#
cratons.grd : cratons.bil
	xyz2grd -R$(GLOBAL_REGION) -I$(RES)c -Zc -F cratons.bil -Gcratons_pixel.grd
	grdsample cratons_pixel.grd -Gcratons.grd -T -fg -Qn
	$(RM) cratons_pixel.grd

#
# Create the craton file from the shape files:
#
cratons.bil : cratons/cratons.shp
	gdal_rasterize -burn 1 -of EHdr -init 0 -te $(GXMIN) $(GYMIN) $(GXMAX) $(GYMAX) -tr $(RES_DD) $(RES_DD) -ot Byte cratons/cratons.shp cratons.bil

cratons/cratons.shp : cratons.tgz
	tar xvmf cratons.tgz

cratons.tgz :
	echo "File cratons.tgz must be supplied -- cannot make"


#
# Make the C programs
#
../src/smooth :
	$(MAKE) -C ../src smooth

../src/grad2vs30 :
	$(MAKE) -C ../src grad2vs30
