#ifndef POSIT_H
#define POSIT_H

#include <iostream>
#include "posit_types.h"

#include <cstdio>
#include <cmath>
#include <bitset>
#include <chrono>
#include <map>

/*
  Class POSIT
  keeps number, number_size, exp_size
  Performs arithemtic for the class
*/
class Posit {
private:
    POSIT_UTYPE number; // number
    int number_size;    // size of the number
    int exp_size;       // size of the exponent1 (which is the combination of regime and exponent2)
public:
    Posit(POSIT_UTYPE bits, int nbits, int es);
    Posit(int nbits, int es);
    mutable int operations_count = 0;
    mutable std::map<std::string, int> all_operations_count = {
      { "=", 0 },
      { "+", 0 },
      { "-", 0 },
      { "*", 0 },
      { "/", 0 },
      { "<", 0 },
      { "<=", 0 },
      { ">", 0 },
      { ">=", 0 },
      { "==", 0 },
      { "!=", 0 }
    };
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    Posit();
    Posit(int v);
    Posit(float v);
    Posit(double v);

    bool isZero() const;                  // check for 0
    bool isNar() const;                   // check for NaR
    bool isNeg() const;                   // check for negative

    int nbits() const;                    // size in bits
    int ss() const;                       // sign size in bits
    int rs() const;                       // regime size in bits
    int es() const;                       // exponent size in bits
    int fs() const;                       // fraction size in bits

    Posit zero() const;                   // 0
    Posit one() const;                    // 1
    Posit nar() const;                    // NaR

    Posit neg() const;                    // -x
    Posit rec() const;                    // 1 / x
    Posit sqrt() const;                   // sqrt(x)

    Posit add(const Posit& p) const;      // x + p
    Posit sub(const Posit& p) const;      // x - p
    Posit mul(const Posit& p) const;      // x * p
    Posit div(const Posit& p) const;      // x / p

    bool eq(const Posit& p) const;        // x == p
    bool gt(const Posit& p) const;        // x > p
    bool ge(const Posit& p) const;        // x >= p
    bool lt(const Posit& p) const;        // x < p
    bool le(const Posit& p) const;        // x <= p

    void set(Posit p);                    // x = p
    void set(float n);                    // x = n
    void set(double n);                   // x = n

    float to_float() const;               // n = x
    double to_double() const;             // n = x
    std::string toBinaryString() const;
    std::string positToBinary(const Posit& p) ;
    
    std::string get_sign_bit();
    std::string get_regime();
    std::string get_fractional();
    std::string get_exponent();

    // debug
    void setBits(POSIT_UTYPE bits);
    POSIT_UTYPE getBits();
    std::bitset<32> getBitset() const;
    void setBitset(std::bitset<32> bits);
    void setCountOperations(int c);
    int getCountOperations();
    std::map<std::string, int> getAllOpsCount();
    double getTotalTime();

    void print();
    void printb();

    friend std::ostream& operator<<(std::ostream& os, const Posit& p);
    friend std::istream& operator>>(std::istream& is, Posit& p);
};


Posit operator+(const Posit& a, const Posit& b);
Posit operator-(const Posit& a, const Posit& b);
Posit operator*(const Posit& a, const Posit& b);
Posit operator/(const Posit& a, const Posit& b);

Posit operator-(const Posit& a);

bool operator<(const Posit&a , const Posit& b);
bool operator<=(const Posit&a , const Posit& b);
bool operator>(const Posit&a , const Posit& b);
bool operator>=(const Posit&a , const Posit& b);
bool operator==(const Posit&a , const Posit& b);
bool operator!=(const Posit&a , const Posit& b);

#endif /* POSIT_H */
