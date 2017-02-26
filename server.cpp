#include <iostream>

#include <zmq.hpp>
#include <zmq_functions.h>

#include <unistd.h>
#include <thread>
#include <vector>

/**
 * Worker thread. Here, each worker thread is just like
 * a single-threaded server in that it uses a REP socket. The
 * key difference is that the communication takes place within
 * inproc channels instead of tcp.
 *
 */
void work(void * ctx)
{
  zmq::context_t *context = (zmq::context_t *) ctx;

  zmq::socket_t socket(*context, ZMQ_REP);
  socket.connect("inproc://workers");

  zmq::message_t request;
  zmq::message_t reply;

  std::string reply_string = "World!";

  while(true) {
    socket.recv(&request);

    std::cout << std::this_thread::get_id() << "\n";

    ZMQFunctions::pack(reply, (void *) reply_string.c_str(), reply_string.length());

    socket.send(reply);
  }
}

int main()
{
  // zmq contexts are thread safe, so we're going to share this
  // context with the worker threads.
  zmq::context_t context(1);

  // The ROUTER socket is the entry point for clients. We bind this
  // to a tcp address. ROUTER sockets track their connected clients via
  // their unique idenfiers.
  zmq::socket_t client_socket(context, ZMQ_ROUTER);
  client_socket.bind("tcp://*:5555");

  // The DEALER socket talks to the workers over ipc.
  zmq::socket_t worker_socket(context, ZMQ_DEALER);
  worker_socket.bind("inproc://workers");

  std::size_t max_threads = std::thread::hardware_concurrency();

  std::vector<std::thread> threads;
  threads.reserve(max_threads);

  // Launch out worker threads.
  for ( std::uint8_t i = 0; i < max_threads; ++i ) {
    // Do we need to share the context? Can each thread have it's own?
    // It's probably needed to shared requests via the proxy.
    threads.push_back(std::thread(work, (void *) &context));
  }

  // The proxy connects the client and work sockets. It pulls
  // incoming requests, fairly, from all clients. This is done
  // using an internal queue structure. As requests are pulled off
  // they are distributed out to our workers. It is also smart enough
  // to route replies (sent from worker threads) back to their origins.
  zmq::proxy(client_socket, worker_socket, NULL);
}
