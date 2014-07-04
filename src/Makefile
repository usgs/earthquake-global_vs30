include ../Constants.mk

LIBINC = -L$(GMTLIB) -L$(CDFLIB)
LINKOPT += -lgmt -lnetcdf

.PHONY: all clean veryclean

all : smooth insert_grd grad2vs30

clean :
	$(RM) smooth insert_grd grad2vs30 getpar.o

veryclean : clean

smooth : smooth.c getpar.o
	cc -o $@ $^ -I$(GMTINC) $(LIBINC) $(LINKOPT)

insert_grd : insert_grd.c getpar.o
	cc -o $@ $^ -I$(GMTINC) $(LIBINC) $(LINKOPT)

grad2vs30 : grad2vs30.c getpar.o
	cc -o $@ $^ -I$(GMTINC) $(LIBINC) $(LINKOPT)

getpar.o : getpar.c libget.h
	cc -c getpar.c
