#include "s21_decimal.h"

// int main () {
//     s21_decimal x1 = {{40, 0, 0, 0}};
//     s21_decimal x2 = {{0, 345, 534, 0}};
//     // s21_decimal x3 = {{3, 0, 0, 0}};
//     s21_decimal result = {{50, 0, 0, 0}};
//     float a = -331.6;
//     float b = 346456.8;
//     int f, g;
//     //int i = 2;
//     // float g;
//     s21_from_float_to_decimal(a, &x1);
//     s21_from_float_to_decimal(b, &x2);
//     // s21_from_int_to_decimal(a, &x1);
//     // s21_from_int_to_decimal(b, &x2);
//     s21_from_decimal_to_int(x1, &f);
//     s21_from_decimal_to_int(x2, &g);
//     // s21_from_float_to_decimal(3.5, &x1);
//     // printf("BBBB%f\n", g);
//     // set_scale(&x1, 0);
//     // set_scale(&x2, 0);
//     printf("ERROR %d\n", s21_mod(x1, x2, &result));
//     // s21_from_decimal_to_float(result, &g);
//     // printf("RESULTF %f\n", g);
//     printf("RESULT %d   %d\n", f, g);
//     printf("SIGN %d\n", get_sign(result));
//     printf("SCALE %d\n", get_scale(result));
//     return 0;
// }

int get_bit(s21_decimal value, int position) {
    int place = position / 32;
    position %= 32;
    return !!(value.bits[place] & (1 << position));
}

void set_bit(s21_decimal *value, int position) {
    int place = position / 32;
    position %= 32;
    value->bits[place] |= (1 << position);
}

void set_bit_zero(s21_decimal *value, int position) {
    int place = position / 32;
    position %= 32;
    // printf("%d\n", !1);
    value->bits[place] &= ~(1 << position);
}

int get_scale(s21_decimal value) {
    value.bits[3] = value.bits[3] << 8;
    value.bits[3] = value.bits[3] >> 24;
    // printf("SCALE%d\n", value.bits[3]);
    return value.bits[3];
}

void set_scale(s21_decimal *src, int scale) {
    int sign = get_sign(*src);
    src->bits[3] = scale;
    src->bits[3] <<= 16;
    set_sign(src, sign);
}

int get_exp(float src) {
    int value = s21_from_float_to_binary(src);
    value = value >> 23;
    value -= 127;
    return value;
}

int get_sign(s21_decimal src) {
    int sign_mark = 0;
    if ((src.bits[3] >> 31)) {
        sign_mark = 1;
    }
    return sign_mark;
}

void set_sign(s21_decimal *src, int sign) {
    if (sign) {
        set_bit(src, 127);
    } else {
        set_bit_zero(src, 127);
    }
}

int get_first_one(s21_decimal value) {
    int index = 96;
    while (!get_bit(value, index)) {
        index--;
    }
    // printf("INDEX%d\n", index);
    return index;
}

unsigned int s21_from_float_to_binary(float src) {
    unsigned int result = *((unsigned int *) & src);
    // for (unsigned int mask = 0x80000000; mask; mask >>=1) {
    //     printf("%d", !!(result & mask));
    // }
    // printf("\n");
    return result;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
    int error_mark = 0;
    initial_num(dst);
    if (fabsf(src) < 1e-28 && fabsf(src) > 0) {
        error_mark = 1;
    } else {
        char str_src[100];
        int count_str = count_str_float(src, str_src), is_overfull = 0;
        s21_decimal ten = {0};
        s21_from_int_to_decimal(10, &ten);
        for (size_t i = 0; i < strlen(str_src); i++) {
            if (str_src[i] != '.' && str_src[i] != '-') {
                s21_decimal add = {0};
                s21_from_int_to_decimal(str_src[i] - '0', &add);
                add_simple(*dst, add, dst);
                is_overfull = s21_mul(*dst, ten, dst);
            }
        }
        if (!is_overfull) {
            div_simple(*dst, ten, dst);
        }
        set_sign(dst, src < 0);
        set_scale(dst, count_str);
    }
    return error_mark;
}

int count_str_float(float src, char* str_src) {
    int count_str = 0, k = 1;
    char str[100];
    snprintf(str, sizeof(str), "%f", src);
    for (int i = (int) strlen(str) - 1; i >= 0; i--) {
        if (str[i] == '0' && k == 1) {
            str[i] = '\0';
            continue;
        } else {
            k = -1;
        }
        if (str[i] == '.') {
            break;
        }
        count_str++;
    }
    strncpy(str_src, str, strlen(str) + 1);
    return count_str;
}

// int s21_from_float_to_decimal(float src, s21_decimal *dst) {
//     memset(dst, 0, sizeof(*dst));
//     int error_mark = 0;
//     int sign = 0;
//     int scale = 0;
//     s21_decimal ten;
//     s21_from_int_to_decimal(10, &ten);
//     if (!isnormal(src)) {
//         error_mark = 1;
//     } else {
//         if (src < 0) {
//             sign = 1;
//             src *= -1;
//         }
//         int exp = get_exp(src);
//         if (exp > 95) {
//             error_mark = 1;
//         } else {
//             if (exp < -95) {
//                 for (; !((int)src) && scale < 28; src*=10, scale++) {}
//             }
//             for (; !(((int)src)/10000000) && scale < 28; src*=10, scale++) {}
//             if (exp < -94) {
//                 error_mark = 1;
//             } else {
//                 exp = get_exp(src);
//                 unsigned int mask = 0x400000;
//                 set_bit(dst, exp);
//                 unsigned int src_int = s21_from_float_to_binary(src);
//                 for (int i = exp-1; mask; mask >>= 1, i--) {
//                     if (src_int & mask) {
//                         set_bit(dst, i);
//                     }
//                 }
//                 set_scale(dst, scale);
//             }
//         }
//     }
//     // if (get_scale(*dst)) {
//     //     while (scale) {
//     //         div_simple(*dst, ten, dst);
//     //         scale--;
//     //     }
//     // }
//     set_sign(dst, sign);
//     return error_mark;
// }

// int s21_from_decimal_to_float(s21_decimal src, float *dst) {
//     int error_mark = 0;
//     *dst = 0.0;
//     int scale = get_scale(src);
//     for (int i =0; i < 96; i++) {
//         if (get_bit(src, i)) {
//             *dst += pow(2, i);
//         }
//     }
//     for (; scale; scale--, *dst /= 10) {}
//     if (get_sign(src)) {
//         *dst *= -1;
//     }
//     return error_mark;
// }

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    int minus = get_sign(src);
    long double a = 0;
    s21_decimal res = {0}, ten = {0};
    s21_from_int_to_decimal(10, &ten);
    s21_truncate(src, &res);
    int sign;
    for (int i = 0; i < 96; i++) {
        sign = get_bit(res, i);
        a += sign * pow(2, i);
    }
    s21_decimal float_part = {0};
    s21_sub(src, res, &float_part);
    int scale = get_scale(src);
    long double f_part = 0;
    int count = 0;
    for (int i = 0; i < 96; i++) {
        sign = get_bit(float_part, i);
        count++;
        f_part += sign * pow(2, i);
    }
    *dst = f_part / pow(10, scale) + a;
    if (minus == 1) {
        *dst = -(*dst);
    }
    return 0;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
    memset(dst, 0, sizeof(*dst));
    int error_mark = 0;
    if (src < 0) {
        src = -src;
        set_sign(dst, 1);
    }
    dst->bits[0] = src;
    return error_mark;
}

// int s21_from_decimal_to_int(s21_decimal src, int *dst) {
//     int error_mark = 0;
//     int scale = get_scale(src);
//     int index = get_first_one(src);
//     if (index > 31) {
//         if (scale) {
//             while (src.bits[2] && scale) {
//                 src.bits[2] /= 10;
//                 scale--;
//             }
//             while (src.bits[1] && scale) {
//                 src.bits[1] /= 10;
//                 scale--;
//             }
//             if (!src.bits[2] && !src.bits[1]) {
//                 *dst = src.bits[0];
//                 for (; scale; scale--, *dst /= 10) {}
//             }
//         } else {
//             error_mark = 1;
//         }
//     } else {
//         *dst = src.bits[0];
//         for (; scale; scale--, *dst /= 10) {}
//     }
//     return error_mark;
// }

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    int error_mark = 0;
    if (!dst) {
        error_mark = 1;
    } else {
        int scale = get_scale(src), sign;
        sign = (get_sign(src)) ? -1 : 1;
        s21_decimal ten;
        s21_from_int_to_decimal(10, &ten);
        while (scale > 0) {
            div_simple(src, ten, &src);
            scale--;
        }
        set_bit_zero(&src, 31);
        *dst = src.bits[0];
        *dst *= sign;
    }
    return error_mark;
}


/************* ARITHMETIC*****************/

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    memset(result, 0, sizeof(*result));
    int overflow = 0;
    int value_1_sign = get_sign(value_1);
    int value_2_sign = get_sign(value_2);
    normalize(&value_1, &value_2);
    int scale = get_scale(value_1);
    if (!(value_1_sign ^ value_2_sign)) {
        overflow = add_simple(value_1, value_2, result);
            set_sign(result, value_1_sign);
    } else if (s21_is_less_or_equal_simple(value_1, value_2)) {
        sub_simple(value_2, value_1, result);
        set_sign(result, value_2_sign);
    } else {
        sub_simple(value_1, value_2, result);
        set_sign(result, value_1_sign);
    }
    if (overflow && scale) {
        s21_bank_rounding(&value_1, 1);
        s21_bank_rounding(&value_2, 1);
        overflow = s21_add(value_1, value_2, result);
    } else {
        set_scale(result, scale);
    }
    return overflow;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    memset(result, 0, sizeof(*result));
    int overflow = 0;
    int value_2_sign = get_sign(value_2);
    set_sign(&value_2, !value_2_sign);
    overflow = s21_add(value_1, value_2, result);
    return overflow;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    memset(result, 0, sizeof(*result));
    int error_mark = 0;
    int sign = get_sign(value_1) ^ get_sign(value_2);
    int scale = get_scale(value_1) + get_scale(value_2);
    mul_simple(value_1, value_2, result);
    set_sign(result, sign);
    if (scale > 28) {
        s21_bank_rounding(&value_1, 1);
        s21_bank_rounding(&value_2, 1);
        error_mark = s21_mul(value_1, value_2, result);
    } else {
        set_scale(result, scale);
    }
    if (scale > 28) {
        error_mark = 2;
    }
    return error_mark;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    memset(result, 0, sizeof(*result));
    int error_mark = 0;
    s21_decimal zero = {0};
    int scale = 0;
    s21_decimal ten = {0};
    s21_from_int_to_decimal(10, &ten);
    int sign = get_sign(value_1) ^ get_sign(value_2);
    normalize(&value_1, &value_2);
    if (s21_is_equal_simple(value_2, zero)) {
        error_mark = 3;
    }
    if (s21_is_less_simple(value_1, value_2)) {
        while (scale < 7) {
            mul_simple(value_1, ten, &value_1);
            div_simple(value_1, value_2, result);
            scale++;
        }
    }
    div_simple(value_1, value_2, result);
    set_sign(result, sign);
    set_scale(result, scale);
    return error_mark;
}


int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    memset(result, 0, sizeof(*result));
    int error_mark = 0;
    s21_decimal zero = {0};
    if (s21_is_equal(value_2, zero)) {
        error_mark = 3;
    }
    normalize(&value_1, &value_2);
    *result = div_simple(value_1, value_2, &zero);
    if (get_sign(*result)) {
        error_mark = 1;
        memset(result, 0, sizeof(*result));
    }
    return error_mark;
}

void normalize(s21_decimal *value_1, s21_decimal *value_2) {
    int scale_1 = get_scale(*value_1);
    int scale_2 = get_scale(*value_2);
    int sign_1 = get_sign(*value_1);
    int sign_2 = get_sign(*value_2);
    int v1 = value_1->bits[3];
    int v2 = value_2->bits[3];
    int min_scale = (scale_1 < scale_2) ? scale_1 : scale_2;
    int max_scale = (scale_1 > scale_2) ? scale_1 : scale_2;
    s21_decimal* min_val = (scale_1 < scale_2) ? value_1 : value_2;
    s21_decimal* max_val = (scale_1 > scale_2) ? value_1 : value_2;
    s21_decimal ten;
    s21_from_int_to_decimal(10, &ten);
    while (min_scale != max_scale) {
        if (min_scale < 28 && !mul_simple(*min_val, ten, min_val)) {
            min_scale++;
            set_scale(min_val, min_scale);
        } else {
            s21_bank_rounding(max_val, max_scale - min_scale);
        break;
        }
    }
    value_1->bits[3] = v1;
    value_2->bits[3] = v2;
    set_scale(min_val, min_scale);
    set_scale(max_val, min_scale);
    set_sign(value_1, sign_1);
    set_sign(value_2, sign_2);
}

void s21_bank_rounding(s21_decimal *value, int count) {
    s21_decimal temp = {0};
    int system_bit = value->bits[3];
    int exp = get_scale(*value) - count;
    while (count > 0) {
        s21_decimal ten = {0}, one = {0}, two = {0}, two_res = {0};
        s21_from_int_to_decimal(10, &ten);
        s21_from_int_to_decimal(1, &one);
        s21_from_int_to_decimal(2, &two);
        s21_decimal dec_mod = div_simple(*value, ten, value);
        if (dec_mod.bits[0] > 5) {
            add_simple(*value, one, value);
        } else if (dec_mod.bits[0] == 5) {
            two_res = div_simple(*value, two, &temp);
            if (s21_is_equal_simple(one, two_res)) {
                add_simple(*value, one, value);
            }
        }
        count--;
    }
    value->bits[3] = system_bit;
    set_scale(value, exp);
}

int shift_left(s21_decimal *value) {
    int overflow = get_bit(*value, 95);
    for (int i = 95; i > 0; i--) {
        if (get_bit(*value, i-1)) {
            set_bit(value, i);
        } else {
            set_bit_zero(value, i);
        }
    }
    set_bit_zero(value, 0);
return overflow;
}

int shift_right(s21_decimal *value) {
    int overflow = get_bit(*value, 1);
    for (int i = 0; i < 96; i++) {
        if (get_bit(*value, i+1)) {
            set_bit(value, i);
        } else {
            set_bit_zero(value, i);
        }
    }
    set_bit_zero(value, 0);
return overflow;
}

/************* ARITHMETIC SIMPLE *****************/

int add_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    // printf("%d\n", value_1.bits[0]);
    memset(result, 0, sizeof(*result));
    int rank = 0;
    for (int i = 0; i < 96; i++) {
        int bit_value_1 = get_bit(value_1, i);
        int bit_value_2 = get_bit(value_2, i);
        if (bit_value_1 ^ bit_value_2 ^ rank) {
            set_bit(result, i);
        } else {
            set_bit_zero(result, i);
        }
        rank = (bit_value_1 && bit_value_2) ||
            (bit_value_1 && rank) || (bit_value_2 && rank);
    }
    return rank;
}

int sub_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int error_mark = 0;
    memset(result, 0, sizeof(*result));
    for (int i = 0; i < 96; i++) {
        int bit_value_1 = get_bit(value_1, i);
        int bit_value_2 = get_bit(value_2, i);
        if (bit_value_1 ^ bit_value_2) {
            set_bit(result, i);
        }
        if (!bit_value_1 && bit_value_2) {
            int k = i + 1;
            while ((bit_value_1 = !get_bit(value_1, k)) && k < 96) {
                set_bit(&value_1, k);
                k++;
            }
            if (k == 96) {
                error_mark = 1;
                memset(result, 0, sizeof(*result));
            }
            set_bit_zero(&value_1, k);
        }
    }
    return error_mark;
}

int mul_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    memset(result, 0, sizeof(*result));
    s21_decimal temp_result;
    memset(&temp_result, 0, sizeof(temp_result));
    int overflow = 0;
    for (int i = 0; i < 96 && !overflow; i++) {
        if (get_bit(value_2, i)) {
            temp_result = value_1;
        }
        for (int j = 0; j < i; j++) {
            if (shift_left(&temp_result)) {
                overflow = 1;
                j = i;
            }
        }
        if (add_simple(temp_result, *result, result)) {
            overflow = 1;
            memset(result, 0, sizeof(*result));
        }
        memset(&temp_result, 0, sizeof(temp_result));
    }
    return overflow;
}


s21_decimal div_simple(s21_decimal dec1,
    s21_decimal dec2, s21_decimal* result) {
    int error_mark = 0;
    if (result) initial_num(result);
    s21_decimal fmod = {0};
    s21_decimal temp = {0};
    s21_decimal zero = {0};
    s21_decimal dec1_copy = dec1;
    // printf("AAAA%d\n", get_sign(dec2));
    if (s21_is_equal(zero, dec2)) {
        error_mark = 1;
    } else {
        if (s21_is_greater_or_equal_simple(dec1, dec2)) {
            set_bit(&temp, 0);
            while (!s21_is_less_simple(dec1, dec2)) {
                s21_decimal copy_dec2 = dec2;
                while (s21_is_greater_simple(dec1, copy_dec2)) {
                    shift_left(&copy_dec2);
                    shift_left(&temp);
                }
                if (s21_is_greater_simple(copy_dec2, dec1)) {
                    shift_right(&copy_dec2);
                    shift_right(&temp);
                }
                sub_simple(dec1, copy_dec2, &dec1);
                add_simple(*result, temp, result);
                initial_num(&temp);
                set_bit(&temp, 0);
            }
            s21_decimal temp1;
            initial_num(&temp);
            initial_num(&temp1);
            mul_simple(*result, dec2, &temp);
            sub_simple(dec1_copy, temp, &temp1);
            if (s21_is_greater_simple(temp1, dec2)) {
                set_bit(result, 0);
            }
        }
        mul_simple(*result, dec2, &temp);
        sub_simple(dec1_copy, temp, &fmod);
        if (error_mark) {
            set_sign(&fmod, 1);
        }
    }
    return fmod;
}

void invert_bit(int *value, int position) {
    *value ^= (1 << position);
}

void initial_num(s21_decimal* value) {
    for (int i = 0; i < 128; i++) {
        set_bit_zero(value, i);
    }
}

/************* COMPARSION OPERATORS SIMPLE *****************/

int s21_is_less_simple(s21_decimal value_1, s21_decimal value_2) {
    int is_less = 0;
    for (int i = 95; i >= 0; i--) {
        if (get_bit(value_1, i) ^ get_bit(value_2, i)) {
            is_less = get_bit(value_2, i);
            break;
        }
    }
    return is_less;
}
int s21_is_equal_simple(s21_decimal value_1, s21_decimal value_2) {
    int is_equal = 1;
    for (int i = 95; i >= 0; i--) {
        if (get_bit(value_1, i) != get_bit(value_2, i)) {
            is_equal = 0;
            break;
        }
    }
    return is_equal;
}

int s21_is_greater_or_equal_simple(s21_decimal value_1, s21_decimal value_2) {
    return s21_is_greater_simple(value_1, value_2)
        || s21_is_equal_simple(value_1, value_2);
}

int s21_is_greater_simple(s21_decimal dec1, s21_decimal dec2) {
  return !s21_is_less_or_equal_simple(dec1, dec2);
}

int s21_is_less_or_equal_simple(s21_decimal dec1, s21_decimal dec2) {
  return s21_is_less_simple(dec1, dec2) ||
         s21_is_equal_simple(dec1, dec2);
}
// void print_decimal_bin(s21_decimal a) {
//     for (int i = BITS - 1; i >=0; i--) {
//         int n = ALL_BIT - 1;
//         printf("|%sbits[%d]%s|", COLOR_BLUE, i, COLOR_END);
//         if (i == BITS - 1) {
//             printf("%s%d%s", COLOR_GREEN, get_bit(a, 127), COLOR_END);
//             n--;
//             while (n >= 0) {
//                 int b = 0;
//                 if ((n < START_EXP_BIT && n >= ZERO_BIT)
//                     || (n > END_EXP_BIT && n < SIGN_BIT)) {
//                     if ((b = get_bit(a, n)) == 0)
//                         printf("%s%d%s", COLOR_ORANGE, b, COLOR_END);
//                     else
//                         printf("%s%d%s", COLOR_RED, b, COLOR_END);
//                 } else {
//                     if (get_bit(a, n) == 1)
//                         printf("%s%d%s", COLOR_CYAN,
                                // get_bit(a, n), COLOR_END);
//                     else
//                         printf("%d", get_bit(a, n));
//                 }
//                 n--;
//             }
//         } else {
//             while (n >= 0) {
//                 if (get_bit(a, n) == 1) {
//                     printf("%s%c%s", COLOR_RED, '1', COLOR_END);
//                 } else {
//                     printf("0");
//                 }
//                 n--;
//             }
//         }
//     }
// }

/************* COMPARSION *****************/

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int is_equal;
  int sign_1 = get_sign(value_1);
  int sign_2 = get_sign(value_2);
  s21_decimal zero = {0};
  if (sign_1 ^ sign_2) {
    is_equal = 0;
    if (s21_is_equal_simple(value_1, zero) &&
        s21_is_equal_simple(value_2, zero)) {
      is_equal = 1;
    }
  } else {
    normalize(&value_1, &value_2);
    is_equal = s21_is_equal_simple(value_1, value_2);
  }
  return is_equal;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
    int is_less = 0;
    int sign_1 = get_sign(value_1);
    int sign_2 = get_sign(value_2);
    if (!sign_1 && !sign_2) {
        normalize(&value_1, &value_2);
        is_less = s21_is_less_simple(value_1, value_2);
    } else if (sign_1 ^ sign_2) {
        is_less = (sign_1) ? 1 : 0;
    } else {
        normalize(&value_1, &value_2);
        is_less = !s21_is_less_simple(value_1, value_2);
    }
    return is_less;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_less(value_1, value_2) ||
         s21_is_equal(value_1, value_2);
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_less_or_equal(value_1, value_2);
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_greater(value_1, value_2) ||
         s21_is_equal(value_1, value_2);
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_equal(value_1, value_2);
}

/* OTHER FUNCTIONS */

int s21_truncate(s21_decimal value, s21_decimal *result) {
    int error_mark = 0;
    int sign = get_sign(value);
    int scale = get_scale(value);
    s21_decimal ten = {0};
    *result = value;
    s21_from_int_to_decimal(10, &ten);
    while (scale != 0) {
        error_mark = get_sign(div_simple(*result, ten, result));
        scale--;
    }
    set_sign(result, sign);
    return error_mark;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
    int error_mark;
    s21_decimal base = {0}, zero = {0}, one = {0};
    s21_from_int_to_decimal(10, &base);
    s21_from_int_to_decimal(0, &zero);
    s21_from_int_to_decimal(1, &one);
    int sign = get_sign(value);
    error_mark = s21_truncate(value, result);
    if (sign == 1 && !s21_is_equal(value, *result)) {
        add_simple(*result, one, result);
    }
    set_sign(result, sign);
    return error_mark;
}

int s21_round(s21_decimal value, s21_decimal *result) {
    int error_mark = 0;
    int sign = get_sign(value);
    s21_decimal ten = {0}, mod_of_one = {0}, one = {0};
    s21_decimal copy = value;
    error_mark = s21_truncate(value, result);
    if (!error_mark) {
        s21_from_int_to_decimal(10, &ten);
        s21_from_int_to_decimal(1, &one);
        int scale = get_scale(value);
        if (scale != 0)
            scale--;
        while (scale != 0) {
            div_simple(value, ten, &value);
            scale--;
        }
        s21_mod(value, ten, &mod_of_one);
        if (mod_of_one.bits[0] >= 5 && !s21_is_equal(copy, *result)) {
            add_simple(*result, one, result);
        }
        set_sign(result, sign);
    } else {
        memset(&result, 0, sizeof(result));
    }
    return error_mark;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
    *result = value;
    set_sign(result, !get_sign(value));
    return 0;
}
