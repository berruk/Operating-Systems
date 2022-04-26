#include <cstddef>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <immintrin.h>


void print_m256i(__m256i var)
{
    int32_t val[8];
    memcpy(val, &var, sizeof(val));

    std::cout << "Array: ";

    for (int i=0;i<8;i++)
        std::cout << val[i] << " ";

    std::cout << "\n";

}

void fill_table (uint32_t arr[8]) 
{
    for (int i=0;i<8;i++)     
        arr[i]= i; 
}

int main (int argc, char **argv)
{
    int to_search = std::stoi(argv[1]);

    alignas(32) uint32_t arr[8];
    fill_table(arr);

    __m256i reg = _mm256_load_si256((__m256i *)&arr[0]);                            //load packed array to vector
    print_m256i(reg);

    __m256i __constant = _mm256_set1_epi32(to_search);                              //define constant to compare
    __m256i __equal_result = _mm256_cmpeq_epi32 (reg, __constant);                

    unsigned equal_mask = _mm256_movemask_ps(_mm256_castsi256_ps(__equal_result)); 
    int index = log2(equal_mask);

    print_m256i(__equal_result);
    std::cout << "Index " << index << std::endl;                   
    

}
