
# MCsim MAKEFILE

#Compiler and Linker
GXX          := g++

#The Target Binary Program
TARGET      := MCsim

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := src
BUILDDIR    := obj
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -std=c++11 -Wall -O3 -g -DSIMULATE_SINGLE_BANK_PRIVATIZATION
# Preprocessor Flags (Enable the PRINT)
CFLAGS		+= -DENABLE_PRINT

INC         := -I$(INCDIR)
INCDEP      := -I$(INCDIR)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

#Defauilt Make
all: directories $(TARGET)
	@echo 'Finished building target: $(TARGET)'
	@echo ' '

#Remake
remake: clean all

#Make the Directories
directories:
	@mkdir -p $(BUILDDIR)

#Clean Objects and Binaries
clean:
	@$(RM) -rf $(BUILDDIR)
	@$(RM) $(TARGET)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS)
	$(GXX) -o $(TARGET) $^

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(GXX) $(CFLAGS) $(INC) -c -o $@ $<
	@$(GXX) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp


#Non-File Targets
.PHONY: all remake clean