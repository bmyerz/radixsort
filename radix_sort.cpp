#include <cstring>
#include <iostream>
#include <cstdint>
#include <limits>
#include <cstdlib>

template <typename T>
void array_print(T* a, int len) {
for (int i=0; i<len; i++) {
  std::cout << a[i] << ",";
}
std::cout << std::endl;
}

int compare(const void* a, const void* b) {
      return *(uint64_t*)a - *(uint64_t*)b; 
}


void radix_sort(uint64_t* a, size_t len, int depth=0) {
  const uint64_t depth_limit = 2;

  // at some point just use a comparison sort
  if (depth == depth_limit) { 
    qsort(a, len, sizeof(uint64_t), compare);
    return;
  }

  //std::cout << "sorting depth " << depth << std::endl;

  // MSD radix sort
  // Each "digit" will be one byte
  // We have 16 buckets, one for each byte value
  const uint64_t mask = 0xF000000000000000 >> (depth*4);
  const size_t nbuckets = 16;

  uint64_t output[len];
  uint64_t counts[nbuckets];
  uint64_t starts[nbuckets];

  // count the number of elements per bucket
  memset(counts, 0, nbuckets*sizeof(uint64_t));
  for (int i=0; i<len; i++) {
    uint64_t index = (a[i]&mask)>>((15-depth)*4);
    counts[index]++;
  }

  //std::cout << "counts: ";
  //array_print(counts, nbuckets);

  // compute bucket starts
  starts[0] = 0;
  for (int i=1; i<nbuckets; i++) {
    starts[i] = starts[i-1] + counts[i-1];
  } 

  // copy elements into buckets
  for (int i=0; i<len; i++) {
    uint64_t index = (a[i]&mask)>>((15-depth)*4);
    output[starts[index]++] = a[i];
  }

  // sort each bucket
  for (int i=0; i<nbuckets; i++) {
    // start and len; remember starts got incremented
    uint64_t s = starts[i] - counts[i];
    uint64_t l = counts[i];

    radix_sort(&output[s], l, depth+1);
  }

  // copy back
  std::memcpy(a, output, len*sizeof(uint64_t));
}

// Conversion of IEEE double precision floating point to ordered integer
// from http://stereopsis.com/radix.html
static inline uint64_t FloatFlip(double f)
{
  uint64_t fi = *(reinterpret_cast<uint64_t*>(&f));
  // if negative, then flip all bits, else flip only sign bit
  uint64_t mask = -((int64_t)(fi >> 63)) | 0x8000000000000000;
  return fi ^ mask;
}

static inline double IFloatFlip(uint64_t i)
{
  uint64_t mask = ((i >> 63) - 1) | 0x8000000000000000;
  uint64_t result = i ^ mask;
  return *(reinterpret_cast<double*>(&result));
}

int main() {
  uint64_t a[10] = {
4,
50,
41,
88,
102,
105,
1411,
1025,
3000,
1
}; 
  array_print(a, 10);

  radix_sort(a, 10);

  array_print(a, 10);

  uint64_t d_i[11];
  double d[11] = {
    0,
    -1,
    1,
    std::numeric_limits<double>::max(),
    std::numeric_limits<double>::lowest(),
    std::numeric_limits<double>::min(),
    4111e10,
    4112e10,
    4112e9,
    4112e11,
    -4112e10
  };
  
  array_print(d, 11);
  for (int i=0; i<11; i++) {
    d_i[i] = FloatFlip(d[i]);
  }

  radix_sort(d_i, 11);

  for (int i=0; i<11; i++) {
    d[i] = IFloatFlip(d_i[i]);
  }
  array_print(d, 11);

}
  

