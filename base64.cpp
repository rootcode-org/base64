/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "base64.h"

static const char* charset_standard = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char* charset_urlsafe  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static const char lookup[] = {
62,-1,62,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,-1,-1,-1,-1,63,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};


// Helper function to determine length of output buffer when encoding
size_t Base64::calculate_encoded_length(size_t unencoded_length)
{
	// Return length of encoded data including padding
	return ((unencoded_length + 2) / 3) * 4;
}


// Helper function to determine length of output buffer when decoding
size_t Base64::calculate_decoded_length(const char* encoded_data, size_t encoded_length)
{
	// Anything less than 4 bytes is not legal
	if (encoded_length < 4) return 0;

	// To return an accurate length we have to check for padding at the end of the data
	size_t max_length = (encoded_length / 4) * 3;
	if (encoded_data[encoded_length - 2] == '=') return max_length - 2;
	if (encoded_data[encoded_length - 1] == '=') return max_length - 1;
	return max_length;
}


size_t Base64::encode(const unsigned char* input, size_t input_length, char* output, size_t output_length, bool urlsafe)
{
	// Make sure the output buffer is big enough
	if (output_length < calculate_encoded_length(input_length)) return 0;

	// Pick the base64 character set
	const char* charset;
	if (urlsafe) charset = charset_urlsafe;
	else         charset = charset_standard;

	// Encode 3 bytes at a time
	unsigned int i = 0, j = 0, v;
	for (; i < (input_length / 3) * 3; i += 3)
	{
		// Read 3 bytes of input
		v = (input[i] << 16) + (input[i + 1] << 8) + input[i + 2];

		// Write 4 characters as output
		output[j++] = charset[v >> 18];
		output[j++] = charset[v >> 12 & 0x3F];
		output[j++] = charset[v >> 6 & 0x3F];
		output[j++] = charset[v & 0x3F];
	}

	// Write remainder with padding
	unsigned int remainder = input_length % 3;
	if (remainder == 1) {
		v = input[i];
		output[j++] = charset[v >> 2];
		output[j++] = charset[v << 4 & 0x3F];
		output[j++] = '=';
		output[j++] = '=';
	} else if (remainder == 2) {
		v = (input[i] << 8) + input[i + 1];
		output[j++] = charset[v >> 10];
		output[j++] = charset[v >> 4 & 0x3F];
		output[j++] = charset[v << 2 & 0x3F];
		output[j++] = '=';
	}

	return j;
}


size_t Base64::decode(const char* input, size_t input_length, unsigned char* output, size_t output_length)
{
	// Make sure the output buffer is big enough
	if (output_length < calculate_decoded_length(input, input_length)) return 0;

	// Decode 4 characters at a time
	unsigned int i = 0, j = 0, k, v;
	for (; i < input_length; i += 4)
	{
		// Read up to 4 characters of input
		k = 0; v = 0;
		for (; k < 4; k++)
		{
			char c = input[i + k];
			if (c == '=') break;
			v += lookup[c - 43] << ((3-k) * 6);
		}

		// Write output according to how many bytes of input were read
		if (k > 1) output[j++] = v >> 16 & 0xff;
		if (k > 2) output[j++] = v >> 8 & 0xff;
		if (k > 3) output[j++] = v & 0xff;
		if (k != 4) break;
	}

	return j;
}
