#
# Project files
#

SRCS = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
SRCS := $(filter-out %_Test.cpp, $(SRCS))

SRCS_T = $(wildcard src/*_Test.cpp) $(wildcard src/**/*_Test.cpp)

OBJS = $(SRCS:.cpp=.o)
OBJS_T = $(SRCS_T:.cpp=.o)
EXE  = SecMon

#
# Compiler flags
#
CXX	 = g++
# std::enable_shared_for_this requires -Wno-non-virtual-dtor
CXXFLAGS = -Wall -Werror -Wextra -std=c++14 -Weffc++ -Wno-non-virtual-dtor -fmessage-length=0
%.o : %.cpp

#
# Linker flags
#
LINKER_FLAGS = -L/usr/local/lib/ -lpthread
LINKER_FLAGS += -lopencv_video -lopencv_videoio -lopencv_imgcodecs -lopencv_core -lopencv_imgproc -lopencv_objdetect
LINKER_FLAGS += -lboost_system -lboost_date_time -lboost_thread
LINKER_FLAGS += -lgflags -lglog -lopenblas
#LINKER_FLAGS += -lcaffe -lglog -lgflags -lprotobuf -lm -lhdf5_hl -lhdf5 -lopenblas

TSTLINKER_FLAGS = $(LINKER_FLAGS) -l:gtest_main.a


#
# Debug build settings
#
DBGDIR = debug
DBGDEPDIR = .depend/$(DBGDIR)
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCXXFLAGS = -pthread -DDEBUG -O0 -g3 -c -march=native -mtune=native

#
# Unit test build settings
#
TSTDIR = test
TSTDEPDIR = .depend/$(TSTDIR)
TSTEXE = $(TSTDIR)/$(EXE)_Test
TSTOBJS = $(filter-out debug/src/SecurityMonitor.o,$(DBGOBJS))
TSTOBJS += $(addprefix $(TSTDIR)/, $(OBJS_T))
#TSTOBJS += $(OBJS_T)
TSTCXXFLAGS = -pthread -DDEBUG -O0 -g3

#
# Release build settings
#
RELDIR = release
RELDEPDIR = .depend/$(RELDIR)
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
#RELCXXFLAGS = -pthread -DNDEBUG -O3 -c -fmessage-length=0 -march=native -mtune=native -mmmx -msse -msse2 -msse3 -msse4.1 -mmovbe -mfpmath=sse -ffast-math -fomit-frame-pointer -ffunction-sections
RELCXXFLAGS = -pthread -DNDEBUG -Ofast -c -march=native -mtune=native
.PHONY: all clean debug prep release remake test

.SECONDARY:

#GPU settings
ifeq ($(shell which nvcc),) # No 'nvcc' found
CXXFLAGS += -DCPU_ONLY
else
CXXFLAGS += -I/usr/local/cuda/targets/x86_64-linux/include
LINKER_FLAGS += -fopenmp
#LINKER_FLAGS += -lopencv_gpu
endif


# Default build
all: prep debug
#all: prep debug test


# Dependencies generation
DEPDIR = .depend
DBGDEPFLAGS = -MT $@ -MMD -MP -MF $(DBGDEPDIR)/$*.d
TSTDEPFLAGS = -MT $@ -MMD -MP -MF $(TSTDEPDIR)/$*.d
RELDEPFLAGS = -MT $@ -MMD -MP -MF $(RELDEPDIR)/$*.d

-include $(addprefix $(DEPDIR)/, $(DBGOBJS:.o=.d)))
-include $(addprefix $(DEPDIR)/, $(TSTOBJS:.o=.d)))
-include $(addprefix $(DEPDIR)/, $(RELOBJS:.o=.d)))

$(DBGDEPDIR)/%.d: ;
$(TSTDEPDIR)/%.d: ;
$(RELDEPDIR)/%.d: ;
#include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))



#
# Debug rules
#
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CXX) $(CXXFLAGS) -o $(DBGEXE) $^ $(LINKER_FLAGS)

$(DBGDIR)/%.o: %.cpp $(DBGDEPDIR)/%.d
	mkdir -p $(dir $@)
	mkdir -p $(dir .depend/$@)
	$(CXX) -c $(CXXFLAGS) $(DBGDEPFLAGS) $(DBGCXXFLAGS) -o $@ $<

$(DBGDIR)/:
	mkdir -p $@


#
# Test rules
#
test: $(TSTEXE)
	$(TSTEXE)

$(TSTEXE): $(TSTOBJS)
	$(CXX) $(CXXFLAGS) -o $(TSTEXE) $^ $(TSTLINKER_FLAGS)

$(TSTDIR)/%.o: %.cpp $(TSTDEPDIR)/%.d
	mkdir -p $(dir $@)
	mkdir -p $(dir .depend/$@)
	$(CXX) -c $(CXXFLAGS) $(TSTDEPFLAGS) $(TSTCXXFLAGS) -o $@ $<

$(TSTDIR)/:
	mkdir -p $@

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CXX) $(CXXFLAGS) -o $(RELEXE) $^ $(LINKER_FLAGS)

$(RELDIR)/%.o: %.cpp $(RELDEPDIR)/%.d
	mkdir -p $(dir $@)
	mkdir -p $(dir .depend/$@)
	$(CXX) -c $(CXXFLAGS) $(RELDEPFLAGS) $(RELCXXFLAGS) -o $@ $<

$(RELDIR)/:
	mkdir -p $@


#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR) $(RELDIR) $(TSTDIR) $(DEPDIR)

remake: clean all

clean:
	rm -rf $(DBGDIR) $(RELDIR) $(TSTDIR) $(DEPDIR)
