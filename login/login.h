// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LOGIN_LOGIN_H_
#define LOGIN_LOGIN_H_

#include <glome.h>

#include "ui.h"

// All exit codes from login_run/main
#define EXITCODE_USAGE 1
// obsolete: EXITCODE_REBOOT 2
// obsolete: EXITCODE_LOCKDOWN 3
#define EXITCODE_IO_ERROR 4
#define EXITCODE_INVALID_AUTHCODE 5
#define EXITCODE_INVALID_INPUT_SIZE 6
#define EXITCODE_INTERRUPTED 7
// obsolete: EXITCODE_LOCKDOWN_ERROR 8
#define EXITCODE_TIMEOUT 9
#define EXITCODE_PANIC 42

// How many bytes of authorization code do we require.
// Each byte has 6-bits of entropy due to Base64 encoding.
//
// For an auth code consisting of 48 bits of entropy with one second delays
// between attempts, the probability of sustaining a brute-force attack lasting
// a year is ~99.9999888%.
//
// This can be calculated using: (1-2**(-N))**(365*24*60*60/delay)
// where N is the number of bits of token’s entropy and delay is in seconds.
//
// We increase this a bit more and choose 60-bits of entropy.
#define MIN_ENCODED_AUTHCODE_LEN 10

// login_run executes the main login logic challenging the user for an
// authenticate code unless fallback authentication has been requested.
//
// On error, the error_tag is set to an error token which should NOT be freed.
int login_run(glome_login_config_t* config, const char** error_tag);

// Constructs the action requesting shell access as a given user.
//
// Caller is expected to free returned message.
// On error, the error_tag is set to an error token which should NOT be freed.
int shell_action(const char* user, char** action, size_t* action_len,
                 const char** error_tag);

// Construct a challenge given the key parameters, host ID, an action, and
// optionally a message prefix tag.
//
// The length of the message prefix tag is in bytes. Only tag sizes of multiples
// by 8 is supported.
//
// Caller is expected to free returned challenge.
// On error, the error_tag is set to an error token which should NOT be freed.
int request_challenge(const uint8_t service_key[GLOME_MAX_PUBLIC_KEY_LENGTH],
                      int service_key_id,
                      const uint8_t public_key[PUBLIC_KEY_LENGTH],
                      const char* host_id, const char* action,
                      const uint8_t prefix_tag[GLOME_MAX_TAG_LENGTH],
                      size_t prefix_tag_len, char** challenge,
                      int* challenge_len, const char** error_tag);

// Set the error_tag to the given error token and return the error code.
int failure(int code, const char** error_tag, const char* message);

// Store the identifier of the current machine in the buf array.
// On error, the error_tag is set to an error token which should NOT be freed.
int get_machine_id(char* buf, size_t buflen, const char** error_tag);

// Helper operations used by the GLOME login authentication.
struct pam_handle;
typedef struct pam_handle pam_handle_t;
void login_error(glome_login_config_t* config, pam_handle_t* pamh,
                 const char* format, ...);
void login_syslog(glome_login_config_t* config, pam_handle_t* pamh,
                  int priority, const char* format, ...);
int login_prompt(glome_login_config_t* config, pam_handle_t* pamh,
                 const char** error_tag, const char* message, char* input,
                 size_t input_size);

// Execute GLOME login authentication for login and PAM binaries.
int login_authenticate(glome_login_config_t* config, pam_handle_t* pamh,
                       const char** error_tag);

#endif  // LOGIN_LOGIN_H_
