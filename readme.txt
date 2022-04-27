Name: Eylon Naamat
ID: 315303529
Name: Michael Matveev

Brief Explanation About The Code:
the stack implementation (single threaded - section 5) is stack.cpp, the server implemented in ex3 merge with the stack is server.cpp (multi thread - section 6+8, include the malloc, calloc and free implemented by us).
the client is client.cpp, the tests are tests.cpp.
we also implemented the bonus, we created a double ended queue instead of stack.

How To Run The Code:
first run the command make all in the terminal (after you in the directory), then type "./server" in order to run the server (sections 6+8).
then in other terminal (or terminals if you want multi clients) run the command "./client" and start typing commands for the stack.
if you want to check our tests you should exit everything, run the server (by the command "./server"), open another terminal, and run "./tests".
in order to check the basic stack run "./stack", and start typing commands for the stack.

if you want to exit from the server, we suggest pressing ctrl+c, we made a sig_handler for SIGINT which closes all the sockets, destroys the stack (and of course releasing memory), and closing the mutex.
if you want to close a client simply type in the clients terminal EXIT.