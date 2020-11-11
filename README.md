# Assigment 6 - Socket Programming in C

Compile the client and server executables and move them to the required directories for testing.

The server runs in an infinite loop, waiting for connections from the client and then servicing those connections. However, it can only service one client at a time.
The client can use the following commands:

`get <file1> <file2> ...`: Fetch the listed files from the server. If a file with the same name already exists in the client's working directory, then that file is overwritten by the data sent from the server. If the server doesn't have the file requested by the client, and error message is printed on the client's side.

`exit`: Makes the client exit and the server then waits for a new connection.

`kill`: Makes the client as well as the server exit.

Any other command given to the client yields an error message on the client side itself.