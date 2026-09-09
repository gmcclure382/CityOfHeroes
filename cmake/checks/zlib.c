#include <zlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CHROMIUM_ZLIB_NO_CHROMECONF
#error Missing transitive zlib compile definition
#endif

#define CHECK(x) do { if (!(x)) { \
	fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
	const size_t length = 1024 * 1024 + 77;
	unsigned char *input = malloc(length);
	unsigned char *output = malloc(length + 1);
	uLongf capacity = compressBound((uLong)length);
	unsigned char *compressed = malloc(capacity);
	CHECK(input && output && compressed);
	uint32_t state = 0x12345678;
	for (size_t i = 0; i < length; ++i) {
		state ^= state << 13;
		state ^= state >> 17;
		state ^= state << 5;
		input[i] = i % 4096 < 2048 ? (unsigned char)i : state;
	}
	CHECK(!strcmp(zlibVersion(), "1.2.11"));
	CHECK(adler32(1, (const Bytef *)"Wikipedia", 9) == 0x11e60398UL);
	CHECK(crc32(0, (const Bytef *)"123456789", 9) == 0xcbf43926UL);
	for (int level = 0; level <= 9; ++level) {
		uLongf packed = capacity;
		CHECK(compress2(compressed, &packed, input, length, level) == Z_OK);
		uLongf restored = length;
		CHECK(uncompress(output, &restored, compressed, packed) == Z_OK);
		CHECK(restored == length && !memcmp(input, output, length));
		compressed[packed - 1] ^= 1;
		restored = length;
		CHECK(uncompress(output, &restored, compressed, packed)
			== Z_DATA_ERROR);
	}
	gzFile file = gzopen("zlib-review-smoke.gz", "wb9");
	CHECK(file);
	CHECK(gzbuffer(file, 4096) == 0);
	CHECK(gzwrite(file, input, length) == (int)length);
	CHECK(gzclose(file) == Z_OK);
	file = gzopen("zlib-review-smoke.gz", "rb");
	CHECK(file);
	CHECK(gzread(file, output, length + 1) == (int)length);
	CHECK(!memcmp(input, output, length));
	CHECK(gzeof(file));
	CHECK(gzrewind(file) == 0);
	CHECK(gzseek(file, length - 32, SEEK_SET) == (z_off_t)length - 32);
	CHECK(gzread(file, output, 32) == 32);
	CHECK(!memcmp(input + length - 32, output, 32));
	CHECK(gzclose(file) == Z_OK);
	CHECK(remove("zlib-review-smoke.gz") == 0);
	free(compressed);
	free(output);
	free(input);
	puts("PASS zlib 1.2.11: levels 0-9, checksum failures, gzip seek/roundtrip");
	return 0;
}
