#
# Compiler flags
#
CC	 = g++
CFLAGS = -Wall -std=c++11 -DUNICODE=1
LFLAGS = -lgdiplus -lgdi32 -luser32 -lkernel32 -lcomctl32 -l:lib/libunicows.a
RES	= windres

#
# Project files
#
SRCS = main.cpp resources.rc
OBJS_1 = $(SRCS:.cpp=.o)
OBJS = $(OBJS_1:.rc=.res)
EXE  = cppshot.exe
OBJDIR = obj
BINDIR = bin
SRCDIR = src

#
# Debug build settings
#
DBGDIR = debug
DBGEXE = $(BINDIR)/$(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(OBJDIR)/$(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXE = $(BINDIR)/$(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(OBJDIR)/$(RELDIR)/, $(OBJS))
RELCFLAGS = -O2 -s -DNDEBUG -mwindows

.PHONY: all clean debug prep release remake

# Default build
all: prep release

#
# Debug rules
#
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CC) $(DBGCFLAGS) -o $(DBGEXE) $^ $(LFLAGS) 

$(OBJDIR)/$(DBGDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

$(OBJDIR)/$(DBGDIR)/%.res: $(SRCDIR)/%.rc
	$(RES) -J rc -O coff -i $< -o $@

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CC) $(RELCFLAGS) -o $(RELEXE) $^ $(LFLAGS) 

$(OBJDIR)/$(RELDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

$(OBJDIR)/$(RELDIR)/%.res: $(SRCDIR)/%.rc
	$(RES) -J rc -O coff -i $< -o $@

#
# Other rules
#
prep:
	@cmd /c "if not exist $(OBJDIR)\\$(DBGDIR) mkdir $(OBJDIR)\\$(DBGDIR)"
	@cmd /c "if not exist $(OBJDIR)\\$(RELDIR) mkdir $(OBJDIR)\\$(RELDIR)"
	@cmd /c "if not exist $(BINDIR)\\$(DBGDIR) mkdir $(BINDIR)\\$(DBGDIR)"
	@cmd /c "if not exist $(BINDIR)\\$(RELDIR) mkdir $(BINDIR)\\$(RELDIR)"

remake: clean all

clean:
	powershell -command "$$files = '$(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS)'.split()|where {$$_}; Foreach ($$item in $$files) { Remove-Item $$item }; exit 0"