all:
	make -C Consola
	make -C CPU
	make -C Kernel
	make -C Memoria

clean:
	make clean -C Consola
	make clean -C CPU
	make clean -C Kernel
	make clean -C Memoria
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
#Esto de abajo es de Eclipse por defecto	
	
#PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#OBJS = TpKiss.o

#ifeq ($(BUILD_MODE),debug)
#	CFLAGS += -g
#else ifeq ($(BUILD_MODE),run)
#	CFLAGS += -O2
#else
#	$(error Build mode $(BUILD_MODE) not supported by this Makefile)
#endif

#all:	TpKiss

#TpKiss:	$(OBJS)
#	$(CXX) -o $@ $^

#%.o:	$(PROJECT_ROOT)%.cpp
#	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

#%.o:	$(PROJECT_ROOT)%.c
#	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

#clean:
#	rm -fr TpKiss $(OBJS)
