#include "byte_stream.hh"
#include <iostream>
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ),  data_(""), total_bytes_pushed_(0), total_bytes_poped_(0), is_closed_(false) {}

void Writer::push( string data )
{
  uint64_t left_size = available_capacity();
  if ( data.size() > left_size ) {
    total_bytes_pushed_ += left_size;
    data_ += data.substr(0,left_size);
    return;
  }
  data_ += data;
  total_bytes_pushed_ += data.size();
}

void Writer::close()
{
  is_closed_ = true;

}

bool Writer::is_closed() const
{
  return is_closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_-(uint64_t)data_.size(); 
}

uint64_t Writer::bytes_pushed() const
{
  return total_bytes_pushed_; // Your code here.
}

string_view Reader::peek() const
{
  return std::string_view( data_.data(), data_.size());
}

void Reader::pop( uint64_t len )
{
  if(len <= data_.size()){
    total_bytes_poped_ += len;
    data_ = data_.substr( len );
    return;
  }
  total_bytes_poped_ += data_.size();
  data_ = "";
}

bool Reader::is_finished() const
{
  return is_closed_ && data_.empty();
}

uint64_t Reader::bytes_buffered() const
{
  return data_.size();
}

uint64_t Reader::bytes_popped() const
{
  return total_bytes_poped_;
}

