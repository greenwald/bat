# Root variables
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs) -lMinuit -lPostScript
ROOTGLIBS    := $(shell root-config --glibs)

# Programs
CXX          = g++
CXXFLAGS     = -g -Wall -fPIC -Wno-deprecated -O2
LD           = g++
LDFLAGS      = -g -O2
SOFLAGS      = -shared

AR           = ar
ARFLAGS      = rc
RANLIB       = ranlib

RM           = rm -f
MV           = mv
ECHO         = echo
CINT         = rootcint

# Assign or Add variables
CXXFLAGS    += $(ROOTCFLAGS)
CXXFLAGS    += -I./src -I./
LIBS        += $(ROOTLIBS) -L. -lcuba
GLIBS       += $(ROOTGLIBS) -L.

CXSRCS      = BCParameter.cxx \
		BCModel.cxx \
		BCGoFTest.cxx \
		BCModelOutput.cxx \
		BCDataPoint.cxx \
		BCDataSet.cxx \
		BCH1D.cxx \
		BCH2D.cxx \
		BCIntegrate.cxx \
		BCEngineMCMC.cxx \
		BCModelManager.cxx \
		BCHistogramFitter.cxx \
		BCEfficiencyFitter.cxx \
		BCGraphFitter.cxx \
		BCLog.cxx \
		BCMath.cxx \
		BCAux.cxx

LIBA         = libBAT.a
LIBSO        = libBAT.so

CXXSRCS      = $(patsubst %.cxx,src/%.cxx,$(CXSRCS))

DICTSRCS     = $(patsubst %.cxx,src/%Dict.cxx,$(CXSRCS))

DICTHDRS     = $(patsubst %.cxx,src/%Dict.h,$(CXSRCS))

CXXOBJS      = $(patsubst %.cxx,obj/%.o,$(CXSRCS))

DICTOBJS     = $(patsubst %.cxx,obj/%Dict.o,$(CXSRCS))

EXEOBJS      =

GARBAGE      = $(CXXOBJS) $(EXEOBJS)  src/*Dict.*  obj/*.o  $(LIBA) $(LIBSO) link.d

all : $(LIBSO)

libs : $(LIBA) $(LIBSO)

link.d : $(patsubst %.cxx,BAT/%.h,$(CXSRCS))
	$(CXX) -MM $(CXXFLAGS) $(CXXSRCS) > link.d;

include link.d

obj/%.o : src/%.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/%Dict.cxx : BAT/%.h
	@echo "generating dictionary "
	$(CINT) -f $@ -c $<

$(LIBSO) : $(CXXOBJS) $(DICTOBJS)
	$(CXX) $(SOFLAGS) $(LDFLAGS) $^ -o $(LIBSO)

$(LIBA) : $(CXXOBJS)
	$(AR)  $(ARFLAGS) $(LIBA) $(CXXOBJS)
	$(RANLIB) $(LIBA)

clean :
	$(RM) $(GARBAGE)

print :
	echo compiler  : $(CXX)
	echo compiler  : $(CXSRCS)
	echo c++ srcs  : $(CXXSRCS)
	echo c++ objs  : $(CXXOBJS)
	echo c++ flags : $(CXXFLAGS)
	echo libs      : $(LIBS)
	echo so flags  : $(SOFLAGS)

	echo rootlibs  : $(ROOTLIBS)
	echo rootglibs : $(ROOTGLIBS)

