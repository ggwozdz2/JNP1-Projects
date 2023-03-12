#ifndef MONEYBAG_H_
#define MONEYBAG_H_

#include <cstdint>
#include <ostream>
#include <iostream>
#include <compare>

using namespace std;

namespace {
    // Constant for 2^64 - 1
    const uint64_t MAX_VALUE = 18446744073709551615u;
}

// Moneybag class 

class Moneybag { 

public:    
    using coin_number_t = uint64_t; 

    constexpr Moneybag(coin_number_t livre, coin_number_t solidus, coin_number_t denier):
    livre(livre), solidus(solidus), denier(denier){};
    constexpr coin_number_t livre_number() const;
    constexpr coin_number_t solidus_number() const;
    constexpr coin_number_t denier_number() const;

    inline friend ostream& operator<< (ostream& os, const Moneybag& moneybag);

    constexpr bool operator== (const Moneybag& other) const;

    constexpr  Moneybag& operator+= (const Moneybag& rhs);
    constexpr Moneybag& operator-= (const Moneybag& rhs);
    constexpr Moneybag& operator*= (coin_number_t rhs);
    constexpr friend Moneybag operator*(Moneybag::coin_number_t lhs, 
    					 const Moneybag& rhs);

    constexpr Moneybag operator+ (const Moneybag& other) const;
    constexpr Moneybag operator- (const Moneybag& other) const;
    constexpr Moneybag operator* (coin_number_t other) const;

    constexpr auto operator<=> (const Moneybag& other) const;
    constexpr explicit operator bool() const {
        return livre > 0 || solidus > 0 || denier > 0;
    }

private:
    coin_number_t livre;
    coin_number_t solidus;
    coin_number_t denier; 
};

// Value class

class Value {
public:
    using value_t = __uint128_t;

    Value() : denierV(0) {};
    explicit Value(Moneybag::coin_number_t deniers) : denierV(deniers) {};
    explicit Value(const Moneybag& moneybag) {
  	    value_t result = 0;
     	result += (((value_t(20) * moneybag.livre_number()) + 
                  moneybag.solidus_number()) * value_t(12)) + moneybag.denier_number();
    	this->denierV = result;
    }

    constexpr value_t get_value() const;
    constexpr bool operator==(const Value &other) const;
    constexpr bool operator==(uint64_t number) const;
    constexpr auto operator<=>(const Value& other) const;
    constexpr auto operator<=>(uint64_t number) const; 

    inline explicit operator std::string() const;

private:
    value_t denierV;
};

// Moneybag implementation

constexpr Moneybag::coin_number_t Moneybag::livre_number() const {
        return this->livre;
}

constexpr Moneybag::coin_number_t Moneybag::solidus_number() const {
        return this->solidus;
}
    
constexpr Moneybag::coin_number_t Moneybag::denier_number() const {
        return this->denier;
}

inline ostream& operator<< (ostream& os, const Moneybag& moneybag) {
    string result = "(";
    result += to_string(moneybag.livre_number());
    result += " livr";
    if (moneybag.livre_number() != 1)
        result += "es";
    result += ", " + to_string(moneybag.solidus_number());
    result += " solidus";
    if (moneybag.solidus_number() != 1)
        result += "es";
    result += ", " + to_string(moneybag.denier_number());
    result += " denier";
    if (moneybag.denier_number() != 1)
        result += "s";
    result += ")";
    os << result;
    return os;
}


constexpr bool Moneybag::operator==(const Moneybag& other) const {
    return (this->livre == other.livre && this->solidus == other.solidus
            && this->denier == other.denier);
}

constexpr Moneybag & Moneybag::operator+=(const Moneybag &rhs) {
    if (MAX_VALUE - livre < rhs.livre)
        throw out_of_range("Cannot add livres, it exceeds max possible value");
    if (MAX_VALUE - solidus < rhs.solidus)
        throw out_of_range("Cannot add soliduses, it exceeds max possible value");
    if (MAX_VALUE - denier < rhs.denier)
        throw out_of_range("Cannot add deniers, it exceeds max possible value");    
    livre += rhs.livre;
    solidus += rhs.solidus;
    denier += rhs.denier;
    return *this;
}

constexpr Moneybag & Moneybag::operator-=(const Moneybag &rhs) {
    if (livre < rhs.livre_number()) {
        throw out_of_range("Cannot substract livres, it goes below 0");
    } 
    if (solidus < rhs.solidus_number()) {
        throw out_of_range("Cannot subtract soliduses, it goes below 0");
    } 
    if (denier < rhs.denier_number()) {
        throw out_of_range("Cannot subtract deniers, it goes below 0"); 
    } 
    livre -= rhs.livre;
    solidus -= rhs.solidus;
    denier -= rhs.denier;
    return *this;
}

constexpr Moneybag & Moneybag::operator*=(Moneybag::coin_number_t scalar) {	
    if (scalar == 0) {
        livre = 0;
        solidus = 0;
        denier = 0;
        return *this;
    }

    if (MAX_VALUE / scalar < livre) {
        throw out_of_range("Cannot multiply scalar by livres, it exceeds max value");
    } 
    if (MAX_VALUE / scalar < solidus) {
        throw out_of_range("Cannot multiply scalar by soliduses, it exceeds max value");
    } 
    if (MAX_VALUE / scalar < denier) {
        throw out_of_range("Cannot multiply scalar by deniers, it exceeds max value");    
    } 

    livre *= scalar;
    solidus *= scalar;
    denier *= scalar;
    return *this;
}

constexpr Moneybag Moneybag::operator+(const Moneybag &rhs) const {
    Moneybag tmp(*this);
    return tmp += rhs;
}

constexpr Moneybag Moneybag::operator-(const Moneybag &rhs) const {
    Moneybag tmp(*this);
    return tmp -= rhs;
}

constexpr Moneybag Moneybag::operator*(const Moneybag::coin_number_t scalar) const {
    Moneybag tmp(*this);
    return tmp *= scalar;
}

constexpr Moneybag operator*(Moneybag::coin_number_t lhs, const Moneybag &rhs) {
    Moneybag tmp = rhs;
    tmp *= lhs;
    return tmp;
}

constexpr auto Moneybag::operator<=>(const Moneybag &rhs) const {
    if (livre == rhs.livre && solidus == rhs.solidus && denier == rhs.denier) {
        return partial_ordering::equivalent;
    }
    if (livre <= rhs.livre && solidus <= rhs.solidus && denier <= rhs.denier) {
        return partial_ordering::less;
    }
    if (livre >= rhs.livre && solidus >= rhs.solidus && denier >= rhs.denier) {
        return partial_ordering::greater;
    }
    return partial_ordering::unordered;
}

// Value implementation

constexpr Value::value_t Value::get_value() const {
    return this->denierV;
}

inline Value::operator string() const {
    string result;
    Value::value_t help = this->denierV;
    if (help == 0)
    	return "0";

    while(help) {
        result += (char)('0' + (help % 10));
        help /= 10;
    }
    reverse(result.begin(), result.end());
    return result;
}

constexpr bool Value::operator==(const Value &other) const{
    return (this->denierV == other.get_value());
}

constexpr bool Value::operator==(const uint64_t number) const{
    return (this->denierV == number);
}

constexpr auto Value::operator<=>(const Value& other) const{
   if (denierV == other.denierV) {
        return partial_ordering::equivalent;
    }
    if (denierV < other.denierV) {
        return partial_ordering::less;
    }
    if (denierV > other.denierV) {
        return partial_ordering::greater;
    }
    return partial_ordering::unordered;
}

constexpr auto Value::operator<=>(const uint64_t number) const{
    if (denierV == number) {
        return partial_ordering::equivalent;
    }
    if (denierV < number) {
        return partial_ordering::less;
    }
    if (denierV > number) {
        return partial_ordering::greater;
    }
    return partial_ordering::unordered;
}

// Objects representing single coins

const Moneybag Livre = Moneybag(1, 0, 0);
const Moneybag Solidus = Moneybag(0, 1, 0);
const Moneybag Denier = Moneybag(0, 0, 1);

#endif // MONEYBAG_H_

