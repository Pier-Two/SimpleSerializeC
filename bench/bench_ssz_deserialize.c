#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "bench.h"
#include "ssz_deserialize.h"

#define BENCH_ITER_WARMUP_UINTN 50000
#define BENCH_ITER_MEASURED_UINTN 100000
#define BENCH_ITER_WARMUP_BOOLEAN 50000
#define BENCH_ITER_MEASURED_BOOLEAN 100000
#define BENCH_ITER_WARMUP_BITVECTOR 5000
#define BENCH_ITER_MEASURED_BITVECTOR 10000
#define BENCH_ITER_WARMUP_BITLIST 5000
#define BENCH_ITER_MEASURED_BITLIST 10000
#define BENCH_ITER_WARMUP_VECTOR 5000
#define BENCH_ITER_MEASURED_VECTOR 10000
#define BENCH_ITER_WARMUP_LIST 5000
#define BENCH_ITER_MEASURED_LIST 10000

typedef struct
{
    size_t bit_size;
    uint8_t value[32];
} ssz_uintN_test_t;

typedef struct
{
    bool bits[262144];
    size_t num_bits;
} ssz_bitvector_test_t;

typedef struct
{
    bool bits[524288];
    size_t num_bits;
} ssz_bitlist_test_t;

typedef struct
{
    uint8_t items[16384];
    size_t element_count;
} ssz_vector_test_uint8;

typedef struct
{
    uint16_t items[16384];
    size_t element_count;
} ssz_vector_test_uint16;

typedef struct
{
    uint32_t items[16384];
    size_t element_count;
} ssz_vector_test_uint32;

typedef struct
{
    uint64_t items[16384];
    size_t element_count;
} ssz_vector_test_uint64;

typedef struct
{
    uint8_t items[16384 * 16];
    size_t element_count;
} ssz_vector_test_uint128;

typedef struct
{
    uint8_t items[8192 * 32];
    size_t element_count;
} ssz_vector_test_uint256;

typedef struct
{
    bool items[65536];
    size_t element_count;
} ssz_vector_test_bool;

typedef struct
{
    uint8_t items[16384];
    size_t element_count;
} ssz_list_test_uint8;

typedef struct
{
    uint16_t items[16384];
    size_t element_count;
} ssz_list_test_uint16;

typedef struct
{
    uint32_t items[16384];
    size_t element_count;
} ssz_list_test_uint32;

typedef struct
{
    uint64_t items[8192];
    size_t element_count;
} ssz_list_test_uint64;

typedef struct
{
    uint8_t items[16384 * 16];
    size_t element_count;
} ssz_list_test_uint128;

typedef struct
{
    uint8_t items[8192 * 32];
    size_t element_count;
} ssz_list_test_uint256;

typedef struct
{
    bool items[65536];
    size_t element_count;
} ssz_list_test_bool;

static void test_uint8_deserialize(void *user_data)
{
    ssz_uintN_test_t *test_data = (ssz_uintN_test_t *)user_data;
    uint8_t out_val;
    ssz_deserialize_uint8(test_data->value, test_data->bit_size / 8, &out_val);
}

static void test_uint16_deserialize(void *user_data)
{
    ssz_uintN_test_t *test_data = (ssz_uintN_test_t *)user_data;
    uint16_t out_val;
    ssz_deserialize_uint16(test_data->value, test_data->bit_size / 8, &out_val);
}

static void test_uint32_deserialize(void *user_data)
{
    ssz_uintN_test_t *test_data = (ssz_uintN_test_t *)user_data;
    uint32_t out_val;
    ssz_deserialize_uint32(test_data->value, test_data->bit_size / 8, &out_val);
}

static void test_uint64_deserialize(void *user_data)
{
    ssz_uintN_test_t *test_data = (ssz_uintN_test_t *)user_data;
    uint64_t out_val;
    ssz_deserialize_uint64(test_data->value, test_data->bit_size / 8, &out_val);
}

static void test_uint128_deserialize(void *user_data)
{
    ssz_uintN_test_t *test_data = (ssz_uintN_test_t *)user_data;
    uint8_t out_val[16];
    ssz_deserialize_uint128(test_data->value, test_data->bit_size / 8, out_val);
}

static void test_uint256_deserialize(void *user_data)
{
    ssz_uintN_test_t *test_data = (ssz_uintN_test_t *)user_data;
    uint8_t out_val[32];
    ssz_deserialize_uint256(test_data->value, test_data->bit_size / 8, out_val);
}

static void test_boolean_deserialize(void *user_data)
{
    bool value = *(bool *)user_data;
    uint8_t in_buf[1];
    in_buf[0] = value ? 1 : 0;
    bool out_val;
    ssz_deserialize_boolean(in_buf, sizeof(in_buf), &out_val);
}

static void test_bitvector_deserialize(void *user_data)
{
    ssz_bitvector_test_t *test_data = (ssz_bitvector_test_t *)user_data;
    uint8_t in_buf[32768];
    memset(in_buf, 0xFF, sizeof(in_buf));
    bool out_bits[262144];
    ssz_deserialize_bitvector(in_buf, sizeof(in_buf), test_data->num_bits, out_bits);
}

static void test_bitlist_deserialize(void *user_data)
{
    ssz_bitlist_test_t *test_data = (ssz_bitlist_test_t *)user_data;
    uint8_t in_buf[65537];
    memset(in_buf, 0xFF, sizeof(in_buf));
    bool out_bits[524288];
    size_t actual_bits;
    ssz_deserialize_bitlist(in_buf, sizeof(in_buf), test_data->num_bits, out_bits, &actual_bits);
}

static void test_vector_uint8_deserialize(void *user_data)
{
    ssz_vector_test_uint8 *test_data = (ssz_vector_test_uint8 *)user_data;
    uint8_t in_buf[65536];
    memcpy(in_buf, test_data->items, test_data->element_count * sizeof(uint8_t));
    uint8_t out_elements[16384];
    ssz_deserialize_vector_uint8(in_buf, sizeof(in_buf), test_data->element_count, out_elements);
}

static void test_vector_uint16_deserialize(void *user_data)
{
    ssz_vector_test_uint16 *test_data = (ssz_vector_test_uint16 *)user_data;
    uint8_t in_buf[65536];
    memcpy(in_buf, test_data->items, test_data->element_count * sizeof(uint16_t));
    uint16_t out_elements[16384];
    ssz_deserialize_vector_uint16(in_buf, sizeof(in_buf), test_data->element_count, out_elements);
}

static void test_vector_uint32_deserialize(void *user_data)
{
    ssz_vector_test_uint32 *test_data = (ssz_vector_test_uint32 *)user_data;
    uint8_t in_buf[65536];
    memcpy(in_buf, test_data->items, test_data->element_count * sizeof(uint32_t));
    uint32_t out_elements[16384];
    ssz_deserialize_vector_uint32(in_buf, sizeof(in_buf), test_data->element_count, out_elements);
}

static void test_vector_uint64_deserialize(void *user_data)
{
    ssz_vector_test_uint64 *test_data = (ssz_vector_test_uint64 *)user_data;
    uint8_t in_buf[131072];
    memcpy(in_buf, test_data->items, test_data->element_count * sizeof(uint64_t));
    uint64_t out_elements[16384];
    ssz_deserialize_vector_uint64(in_buf, sizeof(in_buf), test_data->element_count, out_elements);
}

static void test_vector_uint128_deserialize(void *user_data)
{
    ssz_vector_test_uint128 *test_data = (ssz_vector_test_uint128 *)user_data;
    uint8_t in_buf[262144];
    memcpy(in_buf, test_data->items, test_data->element_count * 16);
    uint8_t out_elements[16384 * 16];
    ssz_deserialize_vector_uint128(in_buf, sizeof(in_buf), test_data->element_count, out_elements);
}

static void test_vector_uint256_deserialize(void *user_data)
{
    ssz_vector_test_uint256 *test_data = (ssz_vector_test_uint256 *)user_data;
    uint8_t in_buf[262144];
    memcpy(in_buf, test_data->items, test_data->element_count * 32);
    uint8_t out_elements[8192 * 32];
    ssz_deserialize_vector_uint256(in_buf, sizeof(in_buf), test_data->element_count, out_elements);
}

static void test_vector_bool_deserialize(void *user_data)
{
    ssz_vector_test_bool *test_data = (ssz_vector_test_bool *)user_data;
    uint8_t in_buf[65536];
    for (size_t i = 0; i < test_data->element_count; i++)
    {
        in_buf[i] = test_data->items[i] ? 1 : 0;
    }
    bool out_elements[65536];
    ssz_deserialize_vector_bool(in_buf, sizeof(in_buf), test_data->element_count, out_elements);
}

static void test_list_uint8_deserialize(void *user_data)
{
    ssz_list_test_uint8 *test_data = (ssz_list_test_uint8 *)user_data;
    uint8_t in_buf[65536];
    memcpy(in_buf, test_data->items, test_data->element_count * sizeof(uint8_t));
    uint8_t out_elements[16384];
    size_t actual_count;
    ssz_deserialize_list_uint8(in_buf, sizeof(in_buf), test_data->element_count, out_elements, &actual_count);
}

static void test_list_uint16_deserialize(void *user_data)
{
    ssz_list_test_uint16 *test_data = (ssz_list_test_uint16 *)user_data;
    uint8_t in_buf[65536];
    memcpy(in_buf, test_data->items, test_data->element_count * sizeof(uint16_t));
    uint16_t out_elements[16384];
    size_t actual_count;
    ssz_deserialize_list_uint16(in_buf, sizeof(in_buf), test_data->element_count, out_elements, &actual_count);
}

static void test_list_uint32_deserialize(void *user_data)
{
    ssz_list_test_uint32 *test_data = (ssz_list_test_uint32 *)user_data;
    uint8_t in_buf[65536];
    memcpy(in_buf, test_data->items, test_data->element_count * sizeof(uint32_t));
    uint32_t out_elements[16384];
    size_t actual_count;
    ssz_deserialize_list_uint32(in_buf, sizeof(in_buf), test_data->element_count, out_elements, &actual_count);
}

static void test_list_uint64_deserialize(void *user_data)
{
    ssz_list_test_uint64 *test_data = (ssz_list_test_uint64 *)user_data;
    uint8_t in_buf[65536];
    memcpy(in_buf, test_data->items, test_data->element_count * sizeof(uint64_t));
    uint64_t out_elements[8192];
    size_t actual_count;
    ssz_deserialize_list_uint64(in_buf, sizeof(in_buf), test_data->element_count, out_elements, &actual_count);
}

static void test_list_uint128_deserialize(void *user_data)
{
    ssz_list_test_uint128 *test_data = (ssz_list_test_uint128 *)user_data;
    uint8_t in_buf[262144];
    memcpy(in_buf, test_data->items, test_data->element_count * 16);
    uint8_t out_elements[16384 * 16];
    size_t actual_count;
    ssz_deserialize_list_uint128(in_buf, sizeof(in_buf), test_data->element_count, out_elements, &actual_count);
}

static void test_list_uint256_deserialize(void *user_data)
{
    ssz_list_test_uint256 *test_data = (ssz_list_test_uint256 *)user_data;
    uint8_t in_buf[262144];
    memcpy(in_buf, test_data->items, test_data->element_count * 32);
    uint8_t out_elements[8192 * 32];
    size_t actual_count;
    ssz_deserialize_list_uint256(in_buf, sizeof(in_buf), test_data->element_count, out_elements, &actual_count);
}

static void test_list_bool_deserialize(void *user_data)
{
    ssz_list_test_bool *test_data = (ssz_list_test_bool *)user_data;
    uint8_t in_buf[65536];
    for (size_t i = 0; i < test_data->element_count; i++)
    {
        in_buf[i] = test_data->items[i] ? 1 : 0;
    }
    bool out_elements[65536];
    size_t actual_count;
    ssz_deserialize_list_bool(in_buf, sizeof(in_buf), test_data->element_count, out_elements, &actual_count);
}

static void run_uintN_benchmarks(void)
{
    ssz_uintN_test_t test_data;
    test_data.bit_size = 8;
    memset(test_data.value, 0xFF, 1);
    {
        bench_stats_t stats = bench_run_benchmark(test_uint8_deserialize, &test_data, BENCH_ITER_WARMUP_UINTN, BENCH_ITER_MEASURED_UINTN);
        bench_print_stats("Benchmark ssz_deserialize_uint8", &stats);
    }
    test_data.bit_size = 16;
    memset(test_data.value, 0xFF, 2);
    {
        bench_stats_t stats = bench_run_benchmark(test_uint16_deserialize, &test_data, BENCH_ITER_WARMUP_UINTN, BENCH_ITER_MEASURED_UINTN);
        bench_print_stats("Benchmark ssz_deserialize_uint16", &stats);
    }
    test_data.bit_size = 32;
    memset(test_data.value, 0xFF, 4);
    {
        bench_stats_t stats = bench_run_benchmark(test_uint32_deserialize, &test_data, BENCH_ITER_WARMUP_UINTN, BENCH_ITER_MEASURED_UINTN);
        bench_print_stats("Benchmark ssz_deserialize_uint32", &stats);
    }
    test_data.bit_size = 64;
    memset(test_data.value, 0xFF, 8);
    {
        bench_stats_t stats = bench_run_benchmark(test_uint64_deserialize, &test_data, BENCH_ITER_WARMUP_UINTN, BENCH_ITER_MEASURED_UINTN);
        bench_print_stats("Benchmark ssz_deserialize_uint64", &stats);
    }
    test_data.bit_size = 128;
    memset(test_data.value, 0xFF, 16);
    {
        bench_stats_t stats = bench_run_benchmark(test_uint128_deserialize, &test_data, BENCH_ITER_WARMUP_UINTN, BENCH_ITER_MEASURED_UINTN);
        bench_print_stats("Benchmark ssz_deserialize_uint128", &stats);
    }
    test_data.bit_size = 256;
    memset(test_data.value, 0xFF, 32);
    {
        bench_stats_t stats = bench_run_benchmark(test_uint256_deserialize, &test_data, BENCH_ITER_WARMUP_UINTN, BENCH_ITER_MEASURED_UINTN);
        bench_print_stats("Benchmark ssz_deserialize_uint256", &stats);
    }
}

static void run_boolean_benchmarks(void)
{
    bool booleans[] = {false, true};
    for (int i = 0; i < 2; i++)
    {
        bench_stats_t stats = bench_run_benchmark(test_boolean_deserialize, &booleans[i], BENCH_ITER_WARMUP_BOOLEAN, BENCH_ITER_MEASURED_BOOLEAN);
        char label[64];
        snprintf(label, sizeof(label), "Benchmark ssz_deserialize_boolean %s", booleans[i] ? "true" : "false");
        bench_print_stats(label, &stats);
    }
}

static void run_bitvector_benchmarks(void)
{
    ssz_bitvector_test_t test_data;
    test_data.num_bits = 262144;
    for (size_t i = 0; i < test_data.num_bits; i++)
    {
        test_data.bits[i] = true;
    }
    bench_stats_t stats = bench_run_benchmark(test_bitvector_deserialize, &test_data, BENCH_ITER_WARMUP_BITVECTOR, BENCH_ITER_MEASURED_BITVECTOR);
    bench_print_stats("Benchmark ssz_deserialize_bitvector", &stats);
}

static void run_bitlist_benchmarks(void)
{
    ssz_bitlist_test_t test_data;
    test_data.num_bits = 524288;
    for (size_t i = 0; i < test_data.num_bits; i++)
    {
        test_data.bits[i] = true;
    }
    bench_stats_t stats = bench_run_benchmark(test_bitlist_deserialize, &test_data, BENCH_ITER_WARMUP_BITLIST, BENCH_ITER_MEASURED_BITLIST);
    bench_print_stats("Benchmark ssz_deserialize_bitlist", &stats);
}

static void run_vector_benchmarks(void)
{
    ssz_vector_test_uint8 test_u8;
    test_u8.element_count = 16384;
    for (size_t i = 0; i < test_u8.element_count; i++)
    {
        test_u8.items[i] = (uint8_t)(i & 0xFF);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_vector_uint8_deserialize, &test_u8, BENCH_ITER_WARMUP_VECTOR, BENCH_ITER_MEASURED_VECTOR);
        bench_print_stats("Benchmark ssz_deserialize_vector_uint8", &stats);
    }
    ssz_vector_test_uint16 test_u16;
    test_u16.element_count = 16384;
    for (size_t i = 0; i < test_u16.element_count; i++)
    {
        test_u16.items[i] = (uint16_t)(i & 0xFFFF);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_vector_uint16_deserialize, &test_u16, BENCH_ITER_WARMUP_VECTOR, BENCH_ITER_MEASURED_VECTOR);
        bench_print_stats("Benchmark ssz_deserialize_vector_uint16", &stats);
    }
    ssz_vector_test_uint32 test_u32;
    test_u32.element_count = 16384;
    for (size_t i = 0; i < test_u32.element_count; i++)
    {
        test_u32.items[i] = (uint32_t)(i * 1234567U);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_vector_uint32_deserialize, &test_u32, BENCH_ITER_WARMUP_VECTOR, BENCH_ITER_MEASURED_VECTOR);
        bench_print_stats("Benchmark ssz_deserialize_vector_uint32", &stats);
    }
    ssz_vector_test_uint64 test_u64;
    test_u64.element_count = 16384;
    for (size_t i = 0; i < test_u64.element_count; i++)
    {
        test_u64.items[i] = 0xFFFFFFFFFFFFFFFFULL;
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_vector_uint64_deserialize, &test_u64, BENCH_ITER_WARMUP_VECTOR, BENCH_ITER_MEASURED_VECTOR);
        bench_print_stats("Benchmark ssz_deserialize_vector_uint64", &stats);
    }
    ssz_vector_test_uint128 test_u128;
    test_u128.element_count = 16384;
    for (size_t i = 0; i < test_u128.element_count; i++)
    {
        uint8_t *ptr = &test_u128.items[i * 16];
        memset(ptr, (int)(i & 0xFF), 16);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_vector_uint128_deserialize, &test_u128, BENCH_ITER_WARMUP_VECTOR, BENCH_ITER_MEASURED_VECTOR);
        bench_print_stats("Benchmark ssz_deserialize_vector_uint128", &stats);
    }
    ssz_vector_test_uint256 test_u256;
    test_u256.element_count = 8192;
    for (size_t i = 0; i < test_u256.element_count; i++)
    {
        uint8_t *ptr = &test_u256.items[i * 32];
        memset(ptr, (int)(i & 0xFF), 32);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_vector_uint256_deserialize, &test_u256, BENCH_ITER_WARMUP_VECTOR, BENCH_ITER_MEASURED_VECTOR);
        bench_print_stats("Benchmark ssz_deserialize_vector_uint256", &stats);
    }
    ssz_vector_test_bool test_bool;
    test_bool.element_count = 65536;
    for (size_t i = 0; i < test_bool.element_count; i++)
    {
        test_bool.items[i] = (i % 2 == 0);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_vector_bool_deserialize, &test_bool, BENCH_ITER_WARMUP_VECTOR, BENCH_ITER_MEASURED_VECTOR);
        bench_print_stats("Benchmark ssz_deserialize_vector_bool", &stats);
    }
}

static void run_list_benchmarks(void)
{
    ssz_list_test_uint8 test_u8;
    test_u8.element_count = 16384;
    for (size_t i = 0; i < test_u8.element_count; i++)
    {
        test_u8.items[i] = (uint8_t)(i & 0xFF);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_list_uint8_deserialize, &test_u8, BENCH_ITER_WARMUP_LIST, BENCH_ITER_MEASURED_LIST);
        bench_print_stats("Benchmark ssz_deserialize_list_uint8", &stats);
    }
    ssz_list_test_uint16 test_u16;
    test_u16.element_count = 16384;
    for (size_t i = 0; i < test_u16.element_count; i++)
    {
        test_u16.items[i] = (uint16_t)(i & 0xFFFF);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_list_uint16_deserialize, &test_u16, BENCH_ITER_WARMUP_LIST, BENCH_ITER_MEASURED_LIST);
        bench_print_stats("Benchmark ssz_deserialize_list_uint16", &stats);
    }
    ssz_list_test_uint32 test_u32;
    test_u32.element_count = 16384;
    for (size_t i = 0; i < test_u32.element_count; i++)
    {
        test_u32.items[i] = (uint32_t)(i * 1234567U);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_list_uint32_deserialize, &test_u32, BENCH_ITER_WARMUP_LIST, BENCH_ITER_MEASURED_LIST);
        bench_print_stats("Benchmark ssz_deserialize_list_uint32", &stats);
    }
    ssz_list_test_uint64 test_u64;
    test_u64.element_count = 8192;
    for (size_t i = 0; i < test_u64.element_count; i++)
    {
        test_u64.items[i] = 0xFFFFFFFFFFFFFFFFULL;
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_list_uint64_deserialize, &test_u64, BENCH_ITER_WARMUP_LIST, BENCH_ITER_MEASURED_LIST);
        bench_print_stats("Benchmark ssz_deserialize_list_uint64", &stats);
    }
    ssz_list_test_uint128 test_u128;
    test_u128.element_count = 16384;
    for (size_t i = 0; i < test_u128.element_count; i++)
    {
        uint8_t *ptr = &test_u128.items[i * 16];
        memset(ptr, (int)(i & 0xFF), 16);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_list_uint128_deserialize, &test_u128, BENCH_ITER_WARMUP_LIST, BENCH_ITER_MEASURED_LIST);
        bench_print_stats("Benchmark ssz_deserialize_list_uint128", &stats);
    }
    ssz_list_test_uint256 test_u256;
    test_u256.element_count = 8192;
    for (size_t i = 0; i < test_u256.element_count; i++)
    {
        uint8_t *ptr = &test_u256.items[i * 32];
        memset(ptr, (int)(i & 0xFF), 32);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_list_uint256_deserialize, &test_u256, BENCH_ITER_WARMUP_LIST, BENCH_ITER_MEASURED_LIST);
        bench_print_stats("Benchmark ssz_deserialize_list_uint256", &stats);
    }
    ssz_list_test_bool test_bool;
    test_bool.element_count = 65536;
    for (size_t i = 0; i < test_bool.element_count; i++)
    {
        test_bool.items[i] = (i % 2 == 0);
    }
    {
        bench_stats_t stats = bench_run_benchmark(test_list_bool_deserialize, &test_bool, BENCH_ITER_WARMUP_LIST, BENCH_ITER_MEASURED_LIST);
        bench_print_stats("Benchmark ssz_deserialize_list_bool", &stats);
    }
}

static void run_all_benchmarks(void)
{
    run_uintN_benchmarks();
    run_boolean_benchmarks();
    run_bitvector_benchmarks();
    run_bitlist_benchmarks();
    run_vector_benchmarks();
    run_list_benchmarks();
}

int main(void)
{
    run_all_benchmarks();
    return 0;
}