 #
 # Copyright @2023 Christopher Roy. All rights reserved. Permission is
 # hereby granted to students registered for Unversity of Washgnton
 # CSE 333 for use solely during Fall Quarter 2023 for purposes of
 # the course. No other use, copying, distributon, or modification
 # is permitted wthout prior written consent. Copyrights for
 # third-party components of this work must be honored. Instructors
 # interested in reusing these course materials should contact the
 # author.
 #
 # Name: Christopher Roy
 # Email: cr0y@uw.edu
 #

# define useful flags to cc/ld/etc.
CFLAGS = g++ -g -Wall -std=c++17

# compile everything
ex7: ex7.o Vector.o
	$(CFLAGS) -o ex7 ex7.o Vector.o

ex7.o: ex7.cc Vector.h
	$(CFLAGS) -c ex7.cc 

Vector.o: Vector.cc Vector.h
	$(CFLAGS) -c Vector.cc

clean:
	rm -rf *.o *~ ex7
