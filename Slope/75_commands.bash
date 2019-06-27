#!/bin/bash

# This script executes every command to make the 7.5 arcsecond global map.
#
# Run this from the Slope directory.
#
# If things go wrong, it's probably best to do it line by line to debug.
# I could only get this to work on a machine with 16GB of RAM. 8GB machine failed.
#

mkdir -p elev

curl https://edcintl.cr.usgs.gov/downloads/sciweb1/shared/topo//downloads/GMTED/Grid_ZipFiles/md75_grd.zip > elev/md75_grd.zip

cd elev

unzip md75_grd.zip

touch -c md75_grd

touch md75_grd/*

cd ..

tar xvmf cratons.tgz

#########################################################################################################
# Do the western half of the map:
#

gdal_translate -srcwin 0 0 88800 67200 -of EHdr elev/md75_grd elev/gmted_global_west.bil

gmt xyz2grd elev/gmted_global_west.bil -ZTLh -r -R-180/5/-56/84 -I7.5s -Ggmted_global_west_pixel.grd

gmt grdsample gmted_global_west_pixel.grd -Ggmted_global_west.grd -T -fg

gmt grdgradient gmted_global_west.grd -n+bg -fg -D -Sglobal_west_grad.grd -Gjunk.grd -V

gdal_rasterize -burn 1 -of EHdr -init 0 -tr 0.002083333333 0.002083333333 -te -180 -56 5 84 -ot Byte cratons/cratons_nshmp.shp cratons_west.bil

gmt xyz2grd -R-180/5/-56/84 -I7.5s -ZTLc -r cratons_west.bil -Gcratons_west_pixel.grd

gmt grdsample cratons_west_pixel.grd -Gcratons_west.grd -T -fg -nn

cd ../src

make

cd -

../src/smooth infile=cratons_west.grd fx=959 fy=959 outfile=cratons_west_smooth.grd

gmt grdlandmask -V -R-180/5/-56/84 -I7.5s -Gglobal_west_landmask.grd -Df

../src/grad2vs30 gradient_file=global_west_grad.grd craton_file=cratons_west_smooth.grd landmask_file=global_west_landmask.grd output_file=global_west_vs30.grd water=600

###########################################################################################################
# Now do the eastern half
#

gdal_translate -srcwin 84000 0 88800 67200 -of EHdr elev/md75_grd elev/gmted_global_east.bil

gmt xyz2grd -ZTLh -r -R-5/180/-56/84 -I7.5s elev/gmted_global_east.bil -Ggmted_global_east_pixel.grd

gmt grdsample gmted_global_east_pixel.grd -Ggmted_global_east.grd -T -fg

gmt grdgradient gmted_global_east.grd -n+bg -fg -D -Sglobal_east_grad.grd -Gjunk.grd -V

gdal_rasterize -burn 1 -of EHdr -init 0 -tr 0.002083333333 0.002083333333 -te -5 -56 180 84 -ot Byte cratons/cratons_nshmp.shp cratons_east.bil

gmt xyz2grd -R-5/180/-56/84 -I7.5s -ZTLc -r cratons_east.bil -Gcratons_east_pixel.grd

gmt grdsample cratons_east_pixel.grd -Gcratons_east.grd -T -fg -nn

../src/smooth infile=cratons_east.grd fx=959 fy=959 outfile=cratons_east_smooth.grd

gmt grdlandmask -V -R-5/180/-56/84 -I7.5s -Gglobal_east_landmask.grd -Df

../src/grad2vs30 gradient_file=global_east_grad.grd craton_file=cratons_east_smooth.grd landmask_file=global_east_landmask.grd output_file=global_east_vs30.grd water=600

###################################################################3
# Combine the maps
#

gmt grdcut global_east_vs30.grd -Geast_vs30.grd -R0/180/-56/84

gmt grdcut global_west_vs30.grd -Gwest_vs30.grd -R-180/0/-56/84

gmt grdpaste west_vs30.grd east_vs30.grd -Gglobal_vs30.grd
