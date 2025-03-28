#include "reassembler.hh"
#include "debug.hh"
#include <iterator>
#include <fstream>
#include <vector>
#include <iostream>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  //keep track of if the last string received and the last expected index
  if(is_last_substring){
    is_last_received_ = true;
    last_index_ = first_index + data.size();
  }

  //if the ending index of the current string is smaller than the next expected byte, we can discard this string
  if(first_index + data.size() < next_byte_){
    return;
  }

  //1. check the boundary and only leave the valid parts of data
  uint64_t last_valid_index = next_byte_ + output_.writer().available_capacity() - 1;
  uint64_t new_start = std::max(next_byte_,first_index);
  uint64_t new_end = std::min(last_valid_index,first_index+data.size()-1);

  data = data.substr(new_start-first_index,new_end-new_start+1);


  //2. merge the data into the stream
  auto it = m.lower_bound(new_start);
  //merge current data with overlaping data whose start_index is larger or equal to first_index
  for(auto cur = it; cur != m.end(); ){
    if(cur->first <= new_end){
      //It's possible that the end of the cur is smaller than new_end, which may cause "new_end-cur->first+1" larger than cur->second.size().
      //This is a subtle error and we need to compare them and use the smaller one.
      uint64_t cut_idx = std::min<uint64_t>( new_end - cur->first + 1, cur->second.size() );
      data += cur->second.substr( cut_idx );
      new_end = std::max(new_end,cur->first + cur->second.size()-1);
      cur = m.erase(cur);
    }
    else{
      break;
    }
  }


  // merge current data with overlaping data whose start index is smaller than first index
  std::vector<uint64_t> to_erase;
  it = m.lower_bound(new_start);
  auto cur = std::make_reverse_iterator(it);

  for(; cur != m.rend(); ++cur ){
    if ( cur->first + cur->second.size() - 1 >= new_start) {
      //if the new data is within the cur data, then we can break
      if(new_end <= cur->first + cur->second.size()-1){
        data = cur->second;
        new_start = cur->first;
        to_erase.push_back(cur->first);
        break;
      }
      uint64_t cut_size = std::max<uint64_t>(new_start - cur->first,0);
      data = cur->second.substr( 0, cut_size ) + data;
      new_start = std::min(new_start,cur->first);
      to_erase.push_back(cur->first);
    } else {
      break;
    }
  }

  for ( auto key : to_erase ) {
    m.erase( key );
  }

  //add the updated data to the map
  m[new_start] = data;

  //3. check if the beginning of the map matches the next_byte_
  while(!m.empty() && m.begin()->first == next_byte_){
    output_.writer().push(m.begin()->second);
    next_byte_ += m.begin()->second.size();
    m.erase(m.begin());
  }

  if ( is_last_received_ && next_byte_ >= last_index_) {
    output_.writer().close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  // return static_cast<uint64_t>(pending_byte_size_);
  u_int64_t cnt = 0;
  for(auto it = m.begin(); it != m.end(); ++it){
    cnt += it->second.size();
  }
  return cnt;
}
