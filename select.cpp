#include <netinet/in.h>
#include <unistd.h>

// Assume sockfd is the server socket descriptor
if (FD_ISSET(sockfd, &tempReadSet)) {
    // Event occurred on the server socket (new connection)
    // Accept the connection and handle it

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int newsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (newsockfd == -1) {
        // Handle accept error
        perror("accept");
    } else 
	{
        // Connection accepted successfully
        // Now you have a new socket descriptor (newsockfd) for the client connection

        // You can retrieve information about the client's address and port if needed
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);
        std::cout << "Accepted connection from " << clientIP << ":" << clientPort << std::endl;

        // Now you can handle the client connection, e.g., read data, process requests, send responses
        // Example:
        char buffer[1024];
        ssize_t bytesRead = recv(newsockfd, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            // Handle receive error
            perror("recv");
        } else if (bytesRead == 0) {
            // Connection closed by client
            std::cout << "Connection closed by client" << std::endl;
        } else {
            // Process received data
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            std::cout << "Received data from client: " << buffer << std::endl;
            // Example: Send a response back to the client
            const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
            send(newsockfd, response, strlen(response), 0);
        }

        // Close the connection when done
        close(newsockfd);
    }

    // Add the new socket descriptor to the read set
    FD_SET(newsockfd, &readSet);

    // Update maxfd if necessary
    if (newsockfd > maxfd) {
        maxfd = newsockfd;
    }
} else {
    // Event occurred on a client socket (data to read)
    // Handle data from client
    // Code to read and process data...
}
