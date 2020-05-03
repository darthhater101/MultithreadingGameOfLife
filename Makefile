
CFLAGS= -g
LDFLAGS= -lsfml-graphics -lsfml-window -lsfml-system -lpthread



OBJS=Source.o GameOfLife.o



%.o: %.cpp
	g++ $(CFLAGS) $(CPPFLAGS) -c $<

prog: $(OBJS)
	g++ $(OBJS) $(LDFLAGS) -o prog
