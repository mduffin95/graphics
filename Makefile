# project name (generate executable with this name)
TARGET   = Main

CC       = g++
# compiling flags here
CFLAGS   = -c -pipe -Wall -Wno-switch -ggdb -g3 -Ofast

LINKER   = g++ -o
# linking flags here
LFLAGS   = 

SDL_CFLAGS := $(shell sdl-config --cflags)
GLM_CFLAGS := -I$(GLMDIR)
SDL_LDFLAGS := $(shell sdl-config --libs)

# change these to proper directories where each file should be
SRCDIR   = Source
OBJDIR   = Objects
BINDIR   = Build

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f

print-%  : ; @echo $* = $($*)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(SDL_LDFLAGS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@ $(SDL_CFLAGS) $(GLM_CFLAGS)
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
