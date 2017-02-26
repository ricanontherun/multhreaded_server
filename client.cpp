#include <iostream>

#include <zmq.hpp>
#include <zmq_functions.h>

int main()
{
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);

  socket.connect("tcp://localhost:5555");

  zmq::message_t reply_message;
  zmq::message_t request_message;

  const char * request = "hello";
  ZMQFunctions::pack(request_message, (void *) request, 5);

  std::cout << "Sending request '" << request << "'\n";
  socket.send(request_message);

  socket.recv(&reply_message);
  std::string reply_string;
  ZMQFunctions::extract(reply_message, reply_string);

  std::cout << "Received reply '" << reply_string << "'\n";
}
