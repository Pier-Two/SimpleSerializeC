#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "snappy_decode.h"
#include "ssz_serialize.h"
#include "ssz_deserialize.h"
#include "ssz_constants.h"
#include "ssz_merkle.h"
#include "yaml_parser.h"

#ifndef TESTS_DIR
#define TESTS_DIR "tests/fixtures/general/phase0/ssz_generic/boolean"
#endif

typedef struct {
    char folder_name[256];
    char folder_path[1024];
    char message[1024];
} FailureDetail;

#define MAX_FAILURES 1024

FailureDetail failures[MAX_FAILURES];
int failure_count = 0;
int total_valid_tests = 0;
int valid_passed = 0;
int valid_failed = 0;
int total_invalid_tests = 0;
int invalid_passed = 0;
int invalid_failed = 0;

void record_failure(const char *folder_name, const char *folder_path, const char *message)
{
    if (failure_count < MAX_FAILURES)
    {
        snprintf(failures[failure_count].folder_name, sizeof(failures[failure_count].folder_name), "%s", folder_name);
        snprintf(failures[failure_count].folder_path, sizeof(failures[failure_count].folder_path), "%s", folder_path);
        snprintf(failures[failure_count].message, sizeof(failures[failure_count].message), "%s", message);
        failure_count++;
    }
}

unsigned char *snappy_decode(const unsigned char *compressed_data, size_t compressed_size, size_t *decoded_size)
{
    size_t uncompressed_length;
    snappy_status status = snappy_uncompressed_length((const char *)compressed_data, compressed_size, &uncompressed_length);
    if (status != SNAPPY_OK)
    {
        fprintf(stderr, "Error: snappy_uncompressed_length failed with status %d\n", status);
        return NULL;
    }
    unsigned char *decoded = malloc(uncompressed_length);
    if (!decoded)
    {
        perror("malloc");
        return NULL;
    }
    status = snappy_uncompress((const char *)compressed_data, compressed_size, (char *)decoded, &uncompressed_length);
    if (status != SNAPPY_OK)
    {
        fprintf(stderr, "Error: snappy_uncompress failed with status %d\n", status);
        free(decoded);
        return NULL;
    }
    *decoded_size = uncompressed_length;
    return decoded;
}

unsigned char *read_file(const char *filepath, size_t *size_out)
{
    FILE *fp = fopen(filepath, "rb");
    if (!fp)
        return NULL;
    if (fseek(fp, 0, SEEK_END) != 0)
    {
        fclose(fp);
        return NULL;
    }
    long filesize = ftell(fp);
    if (filesize < 0)
    {
        fclose(fp);
        return NULL;
    }
    rewind(fp);
    unsigned char *buffer = malloc(filesize);
    if (!buffer)
    {
        fclose(fp);
        return NULL;
    }
    size_t read_bytes = fread(buffer, 1, filesize, fp);
    if (read_bytes != (size_t)filesize)
    {
        free(buffer);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    *size_out = (size_t)filesize;
    return buffer;
}

void process_serialized_file(const char *folder_name, const char *folder_path, const char *serialized_file_path)
{
    total_valid_tests++;
    size_t comp_size = 0;
    unsigned char *comp_data = read_file(serialized_file_path, &comp_size);
    if (!comp_data)
    {
        valid_failed++;
        record_failure(folder_name, folder_path, "Failed to read data from serialized file");
        return;
    }
    size_t dec_size = 0;
    unsigned char *expected_data = snappy_decode(comp_data, comp_size, &dec_size);
    free(comp_data);
    if (!expected_data)
    {
        valid_failed++;
        record_failure(folder_name, folder_path, "Failed to decode Snappy data from serialized file");
        return;
    }
    bool value;
    ssz_error_t des_err = ssz_deserialize_boolean(expected_data, dec_size, &value);
    if (des_err != SSZ_SUCCESS)
    {
        valid_failed++;
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Deserialization error: %d", des_err);
            record_failure(folder_name, folder_path, msg);
        }
        free(expected_data);
        return;
    }
    uint8_t *out_buf = malloc(1);
    if (!out_buf)
    {
        valid_failed++;
        record_failure(folder_name, folder_path, "Memory allocation failed for output buffer");
        free(expected_data);
        return;
    }
    size_t ser_size = 1;
    /* Note the change here: passing the address of value */
    ssz_error_t ser_err = ssz_serialize_boolean(&value, out_buf, &ser_size);
    if (ser_err != SSZ_SUCCESS)
    {
        valid_failed++;
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Serialization error: %d", ser_err);
            record_failure(folder_name, folder_path, msg);
        }
        free(out_buf);
        free(expected_data);
        return;
    }
    if (ser_size != dec_size)
    {
        valid_failed++;
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Size mismatch: expected %zu, got %zu", dec_size, ser_size);
            record_failure(folder_name, folder_path, msg);
        }
        free(out_buf);
        free(expected_data);
        return;
    }
    if (memcmp(out_buf, expected_data, ser_size) != 0)
    {
        valid_failed++;
        record_failure(folder_name, folder_path, "Content mismatch in re-serialized output");
        free(out_buf);
        free(expected_data);
        return;
    }
    uint8_t padded[SSZ_BYTES_PER_CHUNK];
    memset(padded, 0, SSZ_BYTES_PER_CHUNK);
    if (ser_size > 0)
        padded[0] = out_buf[0];
    uint8_t merkle_root[SSZ_BYTES_PER_CHUNK];
    ssz_error_t merkle_err = ssz_merkleize(padded, 1, 1, merkle_root);
    if (merkle_err != SSZ_SUCCESS)
    {
        valid_failed++;
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Merkleization error: %d", merkle_err);
            record_failure(folder_name, folder_path, msg);
        }
        free(out_buf);
        free(expected_data);
        return;
    }
    char meta_yaml_path[1024];
    strncpy(meta_yaml_path, serialized_file_path, sizeof(meta_yaml_path));
    meta_yaml_path[sizeof(meta_yaml_path) - 1] = '\0';
    char *last_slash = strrchr(meta_yaml_path, '/');
    if (last_slash != NULL)
        strcpy(last_slash + 1, "meta.yaml");
    else
        snprintf(meta_yaml_path, sizeof(meta_yaml_path), "meta.yaml");
    size_t yaml_size = 0;
    uint8_t *yaml_data = read_yaml_field(meta_yaml_path, "root", &yaml_size);
    if (!yaml_data)
    {
        valid_failed++;
        record_failure(folder_name, folder_path, "Failed to read 'root' field from meta.yaml");
        free(out_buf);
        free(expected_data);
        return;
    }
    if (yaml_size != SSZ_BYTES_PER_CHUNK)
    {
        valid_failed++;
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Meta.yaml 'root' field size mismatch: expected %d, got %zu", SSZ_BYTES_PER_CHUNK, yaml_size);
            record_failure(folder_name, folder_path, msg);
        }
        free(yaml_data);
        free(out_buf);
        free(expected_data);
        return;
    }
    if (memcmp(yaml_data, merkle_root, SSZ_BYTES_PER_CHUNK) != 0)
    {
        valid_failed++;
        record_failure(folder_name, folder_path, "Meta.yaml 'root' field does not match Merkle root");
        free(yaml_data);
        free(out_buf);
        free(expected_data);
        return;
    }
    valid_passed++;
    free(yaml_data);
    free(out_buf);
    free(expected_data);
}

void process_invalid_serialized_file(const char *folder_name, const char *folder_path, const char *serialized_file_path)
{
    total_invalid_tests++;
    size_t comp_size = 0;
    unsigned char *comp_data = read_file(serialized_file_path, &comp_size);
    if (!comp_data)
    {
        invalid_failed++;
        record_failure(folder_name, folder_path, "Failed to read data from serialized file");
        return;
    }
    size_t dec_size = 0;
    unsigned char *decoded_data = snappy_decode(comp_data, comp_size, &dec_size);
    free(comp_data);
    if (!decoded_data)
    {
        invalid_failed++;
        record_failure(folder_name, folder_path, "Failed to decode Snappy data from serialized file");
        return;
    }
    bool value;
    ssz_error_t des_err = ssz_deserialize_boolean(decoded_data, dec_size, &value);
    if (des_err == SSZ_SUCCESS)
    {
        invalid_failed++;
        record_failure(folder_name, folder_path, "Unexpected success: deserialization did not fail as expected");
    }
    else
    {
        invalid_passed++;
    }
    free(decoded_data);
}

int main(void)
{
    char valid_dir_path[1024];
    snprintf(valid_dir_path, sizeof(valid_dir_path), "%s/valid", TESTS_DIR);
    DIR *dir = opendir(valid_dir_path);
    if (!dir)
    {
        perror("opendir");
        return EXIT_FAILURE;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char folder_path[1024];
        snprintf(folder_path, sizeof(folder_path), "%s/%s", valid_dir_path, entry->d_name);
        char serialized_file_path[1024];
        snprintf(serialized_file_path, sizeof(serialized_file_path), "%s/serialized.ssz_snappy", folder_path);
        process_serialized_file(entry->d_name, folder_path, serialized_file_path);
    }
    closedir(dir);
    char invalid_dir_path[1024];
    snprintf(invalid_dir_path, sizeof(invalid_dir_path), "%s/invalid", TESTS_DIR);
    DIR *dir_invalid = opendir(invalid_dir_path);
    if (!dir_invalid)
    {
        perror("opendir");
        return EXIT_FAILURE;
    }
    while ((entry = readdir(dir_invalid)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char folder_path[1024];
        snprintf(folder_path, sizeof(folder_path), "%s/%s", invalid_dir_path, entry->d_name);
        char serialized_file_path[1024];
        snprintf(serialized_file_path, sizeof(serialized_file_path), "%s/serialized.ssz_snappy", folder_path);
        process_invalid_serialized_file(entry->d_name, folder_path, serialized_file_path);
    }
    closedir(dir_invalid);
    printf("\nValid tests: %d passed, %d failed, out of %d\n", valid_passed, valid_failed, total_valid_tests);
    printf("Invalid tests: %d passed, %d failed, out of %d\n", invalid_passed, invalid_failed, total_invalid_tests);
    if (failure_count > 0)
    {
        for (int i = 0; i < failure_count; i++)
        {
            printf("Folder %s: FAILED - %s (Path: %s)\n", failures[i].folder_name, failures[i].message, failures[i].folder_path);
        }
    }
    return EXIT_SUCCESS;
}
