// =============================================================
//  big_integer.cpp -- BigInteger class implementation
//
//  TASK: Implement all methods declared in big_integer.h
//  This stub file exists only so the project structure is clear.
//  Replace its contents with your implementation.
// =============================================================

#include "big_integer.h"
#include <algorithm>

// TODO: your code here

BigInteger::BigInteger() {
    digits_.push_back(0);
    negative_ = false;
}

BigInteger::BigInteger(int value) {
    *this = BigInteger(std::to_string(value));
}

BigInteger::BigInteger(long long value) {
    *this = BigInteger(std::to_string(value));
}

BigInteger::BigInteger(const std::string& str) {
    if (str.empty()) {
        digits_.push_back(0);
        negative_ = false;
        return;
    }

    size_t start = 0;
    if (str[0] == '-') {
        negative_ = true;
        start = 1;
    } else {
        negative_ = false;
    }

    // Безопасный обратный цикл с использованием size_t (без ворнингов)
    for (size_t i = str.length(); i > start; ) {
        --i;
        digits_.push_back(str[i] - '0');
    }

    while (digits_.size() > 1 && digits_.back() == 0) {
        digits_.pop_back();
    }

    if (digits_.size() == 1 && digits_[0] == 0) {
        negative_ = false;
    }
}

// ======================= Comparison ==========================

bool BigInteger::operator==(const BigInteger& rhs) const {
    if (negative_ != rhs.negative_) return false;
    if (digits_.size() != rhs.digits_.size()) return false;
    for (size_t i = 0; i < digits_.size(); ++i) {
        if (digits_[i] != rhs.digits_[i]) return false;
    }
    return true;
}

bool BigInteger::operator!=(const BigInteger& rhs) const {
    return !(*this == rhs);
}

bool BigInteger::operator<(const BigInteger& rhs) const {
    if (negative_ && !rhs.negative_) return true;
    if (!negative_ && rhs.negative_) return false;

    if (digits_.size() != rhs.digits_.size()) {
        if (negative_) {
            return digits_.size() > rhs.digits_.size();
        } else {
            return digits_.size() < rhs.digits_.size();
        }
    }

    for (size_t i = digits_.size(); i > 0; ) {
        --i;
        if (digits_[i] != rhs.digits_[i]) {
            if (negative_) {
                return digits_[i] > rhs.digits_[i];
            } else {
                return digits_[i] < rhs.digits_[i];
            }
        }
    }
    return false;
}

bool BigInteger::operator>(const BigInteger& rhs) const {
    return rhs < *this;
}

bool BigInteger::operator<=(const BigInteger& rhs) const {
    return !(*this > rhs);
}

bool BigInteger::operator>=(const BigInteger& rhs) const {
    return !(*this < rhs);
}

// ==================== Arithmetic ==============================

BigInteger BigInteger::operator+(const BigInteger& rhs) const {
    if (negative_ && !rhs.negative_) {
        BigInteger temp = *this;
        temp.negative_ = false;
        return rhs - temp;
    }
    if (!negative_ && rhs.negative_) {
        BigInteger temp = rhs;
        temp.negative_ = false;
        return *this - temp;
    }

    BigInteger result;
    result.digits_.clear();
    result.negative_ = negative_;

    int carry = 0;
    size_t n = std::max(digits_.size(), rhs.digits_.size());
    for (size_t i = 0; i < n || carry > 0; ++i) {
        int sum = carry;
        if (i < digits_.size()) sum += digits_[i];
        if (i < rhs.digits_.size()) sum += rhs.digits_[i];
        
        result.digits_.push_back(sum % 10);
        carry = sum / 10;
    }

    return result;
}

BigInteger BigInteger::operator-(const BigInteger& rhs) const {
    if (negative_ != rhs.negative_) {
        BigInteger temp = rhs;
        temp.negative_ = !temp.negative_;
        return *this + temp;
    }

    BigInteger abs_this = *this; abs_this.negative_ = false;
    BigInteger abs_rhs = rhs; abs_rhs.negative_ = false;

    if (abs_this < abs_rhs) {
        BigInteger result = abs_rhs - abs_this; 
        result.negative_ = !negative_;
        return result;
    }

    BigInteger result;
    result.digits_.clear();
    result.negative_ = negative_;

    int borrow = 0;
    for (size_t i = 0; i < digits_.size(); ++i) {
        int diff = digits_[i] - borrow;
        if (i < rhs.digits_.size()) {
            diff -= rhs.digits_[i];
        }
        
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.digits_.push_back(diff);
    }

    while (result.digits_.size() > 1 && result.digits_.back() == 0) {
        result.digits_.pop_back();
    }
    if (result.digits_.size() == 1 && result.digits_[0] == 0) {
        result.negative_ = false;
    }

    return result;
}

BigInteger BigInteger::operator*(const BigInteger& rhs) const {
    BigInteger result;
    result.digits_.assign(digits_.size() + rhs.digits_.size(), 0);
    result.negative_ = (negative_ != rhs.negative_);

    for (size_t i = 0; i < digits_.size(); ++i) {
        int carry = 0;
        for (size_t j = 0; j < rhs.digits_.size() || carry > 0; ++j) {
            long long cur = result.digits_[i + j] + 
                            digits_[i] * 1LL * (j < rhs.digits_.size() ? rhs.digits_[j] : 0) + carry;
            result.digits_[i + j] = cur % 10;
            carry = cur / 10;
        }
    }

    while (result.digits_.size() > 1 && result.digits_.back() == 0) {
        result.digits_.pop_back();
    }
    if (result.digits_.size() == 1 && result.digits_[0] == 0) {
        result.negative_ = false;
    }

    return result;
}

BigInteger BigInteger::operator/(const BigInteger& rhs) const {
    BigInteger result;
    result.digits_.clear();
    result.negative_ = (negative_ != rhs.negative_);

    BigInteger current("0");
    BigInteger abs_rhs = rhs;
    abs_rhs.negative_ = false;

    for (size_t i = digits_.size(); i > 0; ) {
        --i;
        if (!(current.digits_.size() == 1 && current.digits_[0] == 0)) {
            current.digits_.insert(current.digits_.begin(), digits_[i]);
        } else {
            current.digits_[0] = digits_[i];
        }

        int x = 0;
        int l = 0, r = 10;
        while (l <= r) {
            int m = (l + r) / 2;
            BigInteger temp = abs_rhs * BigInteger(m);
            if (temp <= current) {
                x = m;
                l = m + 1;
            } else {
                r = m - 1;
            }
        }
        
        result.digits_.push_back(x);
        current = current - (abs_rhs * BigInteger(x));
    }

    std::reverse(result.digits_.begin(), result.digits_.end());

    if (result.digits_.empty()) {
        result.digits_.push_back(0);
    }

    while (result.digits_.size() > 1 && result.digits_.back() == 0) {
        result.digits_.pop_back();
    }
    if (result.digits_.size() == 1 && result.digits_[0] == 0) {
        result.negative_ = false;
    }

    return result;
}

BigInteger BigInteger::operator%(const BigInteger& rhs) const {
    BigInteger result = *this - (*this / rhs) * rhs;
    return result;
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs) {
    *this = *this + rhs;
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& rhs) {
    *this = *this - rhs;
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs) {
    *this = *this * rhs;
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& rhs) {
    *this = *this / rhs;
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& rhs) {
    *this = *this % rhs;
    return *this;
}

// ====================== Unary ================================

BigInteger BigInteger::operator-() const {
    BigInteger result = *this;
    if (!(result.digits_.size() == 1 && result.digits_[0] == 0)) {
        result.negative_ = !result.negative_;
    }
    return result;
}

BigInteger& BigInteger::operator++() {
    *this += BigInteger(1);
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger temp = *this;
    *this += BigInteger(1);
    return temp;
}

BigInteger& BigInteger::operator--() {
    *this -= BigInteger(1);
    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger temp = *this;
    *this -= BigInteger(1);
    return temp;
}

// ======================== Misc ===============================

std::string BigInteger::to_string() const {
    std::string str;
    if (negative_) {
        str += '-';
    }
    for (size_t i = digits_.size(); i > 0; ) {
        --i;
        str += std::to_string(digits_[i]);
    }
    return str;
}

bool BigInteger::is_zero() const {
    return (digits_.size() == 1 && digits_[0] == 0);
}

bool BigInteger::is_negative() const {
    return negative_;
}

BigInteger::operator bool() const {
    return !is_zero();
}

// ======================== I/O ================================

std::ostream& operator<<(std::ostream& os, const BigInteger& value) {
    os << value.to_string();
    return os;
}

std::istream& operator>>(std::istream& is, BigInteger& value) {
    std::string str;
    is >> str;
    value = BigInteger(str);
    return is;
}