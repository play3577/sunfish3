/* SeeEntity.h
 *
 * Kubo Ryosuke
 */

#ifndef SUNFISH_SEEENTITY__
#define SUNFISH_SEEENTITY__

#include "../eval/Value.h"
#include "core/def.h"
#include <cassert>

namespace sunfish {

template <int KeyLength>
class SeeEntity {
private:
  static CONSTEXPR_CONST uint64_t ValueTypeShift = KeyLength - 2;
  static CONSTEXPR_CONST uint64_t ValueMask = (1ULL << ValueTypeShift) - 1ULL;
  static CONSTEXPR_CONST uint64_t ValueTypeMask = 3ULL << ValueTypeShift;
  static CONSTEXPR_CONST uint64_t HashMask = ~(ValueMask | ValueTypeMask);

  static CONSTEXPR_CONST int ValueInf = 1U << (ValueTypeShift - 1);

  static CONSTEXPR_CONST uint64_t Exact = 0ULL << ValueTypeShift;
  static CONSTEXPR_CONST uint64_t Upper = 1ULL << ValueTypeShift;
  static CONSTEXPR_CONST uint64_t Lower = 2ULL << ValueTypeShift;

  static_assert((ValueTypeMask & ValueMask) == 0, "invalid");
  static_assert((ValueTypeMask & HashMask) == 0, "invalid");
  static_assert((ValueMask & HashMask) == 0, "invalid");
  static_assert((ValueTypeMask | ValueMask | HashMask) == ~0ULL, "invalid");
  static_assert(ValueInf >= 30000, "invalid");

  uint64_t data_;

  static int32_t convertValue(int32_t value) {
    return ValueInf - value;
  }

public:

  SeeEntity() {
    static_assert(sizeof(data_) == sizeof(uint64_t), "invalid data size");
    init();
  }

  void init() {
    data_ = 0ull;
  }

  void init(unsigned) {
    init();
  }

  bool get(uint64_t hash, Value& value, const Value& alpha, const Value& beta) const {
    uint64_t temp = data_;
    if ((temp & HashMask) == (hash & HashMask)) {
      uint64_t valueType = temp & ValueTypeMask;
      value = convertValue((int32_t)(temp & ValueMask));
      assert(value > -ValueInf);
      assert(value < ValueInf);
      if (valueType == Exact) {
        return true;
      } else if (valueType == Lower && value >= beta) {
        return true;
      } else if (valueType == Upper && value <= alpha) {
        return true;
      }
    }
    return false;
  }

  void set(uint64_t hash, const Value& value, const Value& alpha, const Value& beta) {
    uint64_t valueType;
    if (value >= beta) {
      valueType = Lower;
    } else if (value > alpha) {
      valueType = Exact;
    } else {
      valueType = Upper;
    }

    assert(value > -ValueInf);
    assert(value < ValueInf);
    uint32_t v = (uint32_t)convertValue(value.int32());
    assert((v & ~ValueMask) == 0U);
    uint64_t temp = (hash & HashMask) | valueType | v;
    data_ = temp;
  }
};

} // namespace sunfish

#endif // SUNFISH_SEEENTITY__
