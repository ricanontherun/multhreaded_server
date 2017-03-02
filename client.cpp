#include <iostream>
#include <thread>

#include <zmq.hpp>
#include <zmq_functions.h>
#include "queue.hpp"

ThreadSafeQueue<std::string> queue;

void ClientWorker(void * ctx)
{
  zmq::context_t * context = (zmq::context_t *) ctx;
  zmq::socket_t socket(*context, ZMQ_REQ);

  zmq::message_t request;
  zmq::message_t reply;
  std::string string = "Hello!!!";

  while (true)
  {
    if ( !queue.empty() ) {
      std::string address = queue.pop();

      socket.connect(address);

      ZMQFunctions::pack(request, (void *) string.c_str(), string.length());

      socket.send(request);

      socket.recv(&reply);

      std::cout << "received reply from server\n";
    }
  }
}

int main()
{
  // Definitely want to reuse the same context.
  zmq::context_t context(1);

  // Create our threads.
  std::thread thread1(ClientWorker, (void *)&context);
  std::thread thread2(ClientWorker, (void *)&context);
  std::thread thread3(ClientWorker, (void *)&context);
  std::thread thread4(ClientWorker, (void *)&context);

  // This loop simply waits for an entered character, and fills the queue with 20 identical
  // addresses. This tests the server's capability to handle many concurrent requests.
  while (true) {
    getchar();

    for ( int i = 0; i < 20; ++i ) {
      queue.push("tcp://159.203.180.106:5555");
    }
  }
}
