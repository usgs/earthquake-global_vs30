include ~/.vs30/Constants.mk

LIBPATH = -L$(GMTLIB) -L$(CDFLIB)
INCPATH = -I$(GMTINC) -I$(CDFINC)
LINKOPT += $(STATIC) -lgmt -lnetcdf -lm

.PHONY: all clean veryclean

all : smooth insert_grd grad2vs30

clean :
	$(RM) smooth insert_grd grad2vs30 getpar.o

veryclean : clean

smooth : smooth.c getpar.o
	cc -o $@ $^ $(INCPATH) $(LIBPATH) $(LINKOPT)

insert_grd : insert_grd.c getpar.o
	cc -o $@ $^ $(INCPATH) $(LIBPATH) $(LINKOPT)

grad2vs30 : grad2vs30.c getpar.o
	cc -o $@ $^ $(INCPATH) $(LIBPATH) $(LINKOPT)

getpar.o : getpar.c libget.h
	cc -c getpar.c
