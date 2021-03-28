/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

namespace Base64 {
	size_t calculate_encoded_length(size_t unencoded_length);
	size_t calculate_decoded_length(const char* encoded_data, size_t encoded_length);
	size_t encode(const unsigned char* input, size_t input_length, char* output, size_t output_length, bool urlsafe=false);
	size_t decode(const char* input, size_t input_length, unsigned char* output, size_t output_length);
}
