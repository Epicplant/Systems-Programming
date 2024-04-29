# Bug 1

## A) How is your program acting differently than you expect it to?
- accept system call wouldn't allow for connections from test_httpconnection.cc
even though I was using the exact same port and address as the connect system call

## B) Brainstorm a few possible causes of the bug
- I might be missing some flags when getting addrinfo which could
potentially cause me to not create the same socket as the connection did
- I might not have been setting up the listener properly (may have missed
some type of binding, etc.)
- I might not be accepting with the right file descriptor (may have stored
file descriptor wrong for the listening_fd_)

## C) How you fixed the bug and why the fix was necessary
- Found out it was the flags issues at office hours. Didn't initially think
the flags would be a big issue since I loop through all possible sockets; however
I was creating a socket that had less restrctions than the connection which ultimately
made me pick a different one and caused the conflict.


# Bug 2

## A) How is your program acting differently than you expect it to?
- IsPathSafe from HttpUtils.cc Keeps returning false even in cases where it should be returning true

## B) Brainstorm a few possible causes of the bug
- I might be using boost::find incorrectly (may have picked the wrong value for "index not found" as a comparator)
- I may be passing in the wrong values for root_dir and test_file which is resulting in unexpected interacitons
- I may need to find the absolute path for the both the directory and the file

## C) How you fixed the bug and why the fix was necessary
- I fixed the path by finding the absolute path for both the directory and file. I thought that havng
the directory stay the same wouldn't mess up the comparison (as I didn't know that the directory path
could ALSO have ".." and "."). By finding the absolute path of the directory, I removed characters
that could cause the boost::find algorithm confusion (as boost::find only works here if the dir path
substring can be found within the file_path) which ultimately resulted in it working.


# Bug 3

## A) How is your program acting differently than you expect it to?
- The server intermittently exits while I am navigating through the program (so segfaults or anything)

## B) Brainstorm a few possible causes of the bug
- When I call GetRequest in HttpServer a false value might always be returned which causes
the program to break for every thread
- Multiple threads may not be being created which would result in the program breaking whenever
a browser tried to form a new connection
- I might be having a verify333 error somewhere in ServerSocket (tested and saw that Verify333's don't show up
if they are found there)

## C) How you fixed the bug and why the fix was necessary
- Turned out that I was closing my listening file descriptor at the end of Server Socket.cc so only
on thread was ever being made. This ultimately resulted in my server crashing whenever my browser tried
out different connections (which it occasionally did) and prevented multiple clients from accessing the
server at the same time. I fixed the problem by removing my closing statement.
