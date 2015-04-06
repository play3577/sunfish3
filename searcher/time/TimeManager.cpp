/* TimeManager.cpp
 *
 * Kubo Ryosuke
 */

#include "TimeManager.h"
#include "core/def.h"
#include <cassert>

namespace sunfish {

	void TimeManager::init() {
		_depth = 0;
	}

	void TimeManager::nextDepth() {
		_depth++;
		assert(_depth < Tree::StackSize);
	}

	void TimeManager::startDepth() {
		_stack[_depth].firstMove = Move::empty();
		_stack[_depth].firstValue = -Value::Inf;
	}

	void TimeManager::addMove(Move move, Value value) {
		if (value > _stack[_depth].firstValue) {
			_stack[_depth].firstMove = move;
			_stack[_depth].firstValue = value;
		}
	}

	bool TimeManager::isEasy(double limit, double elapsed) {
		CONSTEXPR int easyDepth = 5;

		if (_depth <= easyDepth) {
			return false;
		}

		const auto& easy = _stack[_depth-easyDepth];
		const auto& prev = _stack[_depth-1];
		const auto& curr = _stack[_depth];

		double r = elapsed / std::max(std::min(limit, 3600.0) * 0.30, 3.0);

		if (curr.firstValue >= easy.firstValue - (256 * r) && curr.firstValue <= easy.firstValue + (512 * r) &&
				curr.firstValue >= prev.firstValue - (64 * r) && curr.firstValue <= prev.firstValue + (256 * r)) {
			return true;
		}

		if (curr.firstMove == easy.firstMove && curr.firstMove == prev.firstMove &&
				curr.firstValue >= easy.firstValue - (256 * r) && curr.firstValue <= easy.firstValue + (512 * r) &&
				curr.firstValue >= prev.firstValue - (128 * r) && curr.firstValue <= prev.firstValue + (256 * r)) {
			return true;
		}

		if (curr.firstMove == prev.firstMove &&
				curr.firstValue >= prev.firstValue && curr.firstValue <= prev.firstValue + (256 * r)) {
			return true;
		}

		if (curr.firstMove == easy.firstMove &&
				curr.firstValue >= easy.firstValue && curr.firstValue <= easy.firstValue + (128 * r)) {
			return true;
		}

		return false;
	}

}
