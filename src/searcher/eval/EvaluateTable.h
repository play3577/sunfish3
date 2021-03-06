/* EvaluateTable.h
 *
 *  Created on: 2012/07/09
 *      Author: ryosuke
 */

#ifndef SUNFISH_EVALUATETABLE__
#define SUNFISH_EVALUATETABLE__

#include "EvaluateEntity.h"
#include "../table/HashTable.h"

namespace sunfish {

template <int KeyLength>
class EvaluateTable : public HashTable<EvaluateEntity<KeyLength>> {
public:
  using BaseType = HashTable<EvaluateEntity<KeyLength>>;

  EvaluateTable() : BaseType(KeyLength) {
  }
  EvaluateTable(const EvaluateTable&) = delete;
  EvaluateTable(EvaluateTable&&) = delete;

  bool get(uint64_t hash, Value& value) const {
    return BaseType::getEntity(hash).get(hash, value);
  }

  void set(uint64_t hash, const Value& value) {
    BaseType::getEntity(hash).set(hash, value);
  }
};

} // namespace sunfish

#endif // SUNFISH_EVALUATETABLE__
