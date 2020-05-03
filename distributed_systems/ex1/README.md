# Distributed Systems

## Exercise Pronunciation

Write a concurrent C server that does the following calculations:

1. Finds the average of an array Y with n integers
1. Finds the min and max value of an array Y with n integers
1. Finds the product a*Y, where a is a floating point number and Y an array with n integers

Write a client program that communicates with the server via TCP AF_INET sockets.
The client will read the data from the keyboard repeatedly until the user decides to exit.
The server will calcute the results using a Remote Procedure Call to an RPC server.
