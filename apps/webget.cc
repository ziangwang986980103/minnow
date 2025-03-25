#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{

  TCPSocket tcp_socket;
  const Address peer { host, "http" };

  tcp_socket.connect(peer);
  string write_buffer = "GET " + path + " HTTP/1.1\r\n";
  write_buffer += "Host: " + host + "\r\n";
  write_buffer += "Connection: close\r\n";
  write_buffer += "\r\n";
  cout << write_buffer << endl;
  tcp_socket.write(write_buffer);

  string response;
  while(!tcp_socket.eof()){
    string read_buffer;
    tcp_socket.read(read_buffer);
    if(!read_buffer.empty()){
      response += read_buffer;
    }
  }
  cout << response << endl;
  tcp_socket.close();
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
