#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>

typedef enum {
norm,
infinit,
negative_infinit,
not_a_number
} value_type_t;

#define _2(bit) (1 << bit)

#define BITS 4
#define BITS_IN_INT 32
#define INFO_BIT 3
#define BITS_COUNT 4
#define ALL_BIT (BITS_IN_INT * BITS_COUNT)
#define START_EXP_BIT 16
#define END_EXP_BIT 23
#define ZERO_BIT 0
#define SIGN_BIT 31
#define OK 0
#define ERR 1

#define COLOR_HEADER "\033[95m"
#define COLOR_BLUE "\033[94m"
#define COLOR_CYAN "\033[96m"
#define COLOR_GREEN "\033[92m"
#define COLOR_ORANGE "\033[93m"
#define COLOR_RED "\033[91m"
#define COLOR_END "\033[0m"

typedef struct {
    int bits[4];
} s21_decimal;

int get_bit(s21_decimal value_type_t, int position);
int get_scale(s21_decimal value);
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
unsigned int s21_from_float_to_binary(float src);
int get_exp(float src);
int get_sign(s21_decimal src);
void set_scale(s21_decimal *src, int scale);
void set_sign(s21_decimal *src, int sign);
void set_bit(s21_decimal *value, int position);
void set_bit_zero(s21_decimal *value, int position);
void invert_bit(int *value, int position);
int bits_minus(int bits);
int bit_on(int bits, int bit);
int bit_off(int bits, int bit);
s21_decimal nullify_all_decimal(s21_decimal *a);
int is_zero(s21_decimal a);
void print_decimal_bin(s21_decimal a);
void normalize(s21_decimal *value_1, s21_decimal *value_2);
void s21_bank_rounding(s21_decimal *value, int count);
int count_str_float(float src, char* str_src);

// arithmetic

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

//  COMPARSION

int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

// shift

int shift_left(s21_decimal *number);
int shift_right(s21_decimal *number);

// simple
s21_decimal negative_decimal(s21_decimal n);
int subtraction(s21_decimal a, s21_decimal b, s21_decimal* res);
int add_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int sub_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int mul_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
s21_decimal divide(s21_decimal val1, s21_decimal val2, s21_decimal* res);
s21_decimal div_simple(s21_decimal dec1, s21_decimal dec2, s21_decimal* result);
int s21_is_less_simple(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal_simple(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal_simple(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_simple(s21_decimal dec1, s21_decimal dec2);
int s21_is_less_or_equal_simple(s21_decimal dec1, s21_decimal dec2);

// init
void initial_num(s21_decimal* value);

//  other function
int s21_round(s21_decimal value, s21_decimal *result);
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

#endif  //  SRC_S21_DECIMAL_H_
