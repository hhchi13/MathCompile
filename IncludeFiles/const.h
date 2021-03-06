// Compiler from Wolfram Language to C++
// 
// Copyright 2019 Tianhuan Lu
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <limits>

#include "types.h"

namespace wl
{

constexpr auto const_null      = void_type{};
constexpr auto const_all       = all_type{};
constexpr auto const_none      = none_type{};
constexpr auto const_real      = real_type{};
constexpr auto const_integer   = integer_type{};
constexpr auto const_complex   = complex_type{};
constexpr auto const_string    = string_type{};
constexpr auto const_list      = list_type{};
constexpr auto const_byte      = byte_type{};
constexpr auto const_character = character_type{};
constexpr auto const_word      = word_type{};
constexpr auto const_record    = record_type{};
constexpr auto const_i         = complex<double>(0., 1.);
constexpr auto const_i_f       = complex<float>(0.f, 1.f);
constexpr auto const_true      = boolean(true);
constexpr auto const_false     = boolean(false);

constexpr auto const_whitespace            = _whitespace_type{};
constexpr auto const_number_string         = _number_string_type{};
constexpr auto const_word_character        = _word_character_type{};
constexpr auto const_digit_character       = _digit_character_type{};
constexpr auto const_hexadecimal_character = _hexadecimal_character_type{};
constexpr auto const_letter_character      = _letter_character_type{};
constexpr auto const_whitespace_character  = _whitespace_character_type{};
constexpr auto const_punctuation_character = _punctuation_character_type{};
constexpr auto const_word_boundary         = _word_boundary_type{};
constexpr auto const_start_of_line         = _start_of_line_type{};
constexpr auto const_end_of_line           = _end_of_line_type{};
constexpr auto const_start_of_string       = _start_of_string_type{};
constexpr auto const_end_of_string         = _end_of_string_type{};

constexpr auto const_pi           = double(3.1415926535897932385e+0);
constexpr auto const_e            = double(2.7182818284590452354e+0);
constexpr auto const_degree       = double(1.7453292519943295769e-2);
constexpr auto const_golden_ratio = double(1.6180339887498948482e+0);
constexpr auto const_golden_angle = double(2.3999632297286533222e+0);
constexpr auto const_euler_gamma  = double(5.7721566490153286061e-1);
constexpr auto const_catalan      = double(9.1596559417721901505e-1);
constexpr auto const_glaisher     = double(1.2824271291006226369e+0);
constexpr auto const_khinchin     = double(2.6854520010653064453e+0);

constexpr auto MaximumArrayRank = 16;
constexpr auto MaximumArgCount  = 16;

constexpr auto const_int_infinity  = std::numeric_limits<int64_t>::max();
constexpr auto const_real_infinity = std::numeric_limits<double>::max();

}
