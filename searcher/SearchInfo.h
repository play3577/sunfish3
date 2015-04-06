/* SearchInfo.h
 * 
 * Kubo Ryosuke
 */

#ifndef __SUNFISH_SEARCHINFO__
#define __SUNFISH_SEARCHINFO__

#include "eval/Value.h"
#include "core/move/Move.h"
#include <cstdint>

namespace sunfish {

	struct SearchInfoBase {
		uint64_t failHigh;
		uint64_t failHighFirst;
		uint64_t failHighIsHash;
		uint64_t failHighIsKiller1;
		uint64_t failHighIsKiller2;
		uint64_t hashProbed;
		uint64_t hashHit;
		uint64_t hashExact;
		uint64_t hashLower;
		uint64_t hashUpper;
		uint64_t hashStore;
		uint64_t hashNew;
		uint64_t hashUpdate;
		uint64_t hashCollision;
		uint64_t hashReject;
		uint64_t mateProbed;
		uint64_t mateHit;
		uint64_t expand;
		uint64_t expandHashMove;
		uint64_t shekProbed;
		uint64_t shekSuperior;
		uint64_t shekInferior;
		uint64_t shekEqual;
		uint64_t nullMovePruning;
		uint64_t nullMovePruningTried;
		uint64_t futilityPruning;
		uint64_t extendedFutilityPruning;
		uint64_t expanded;
		uint64_t checkExtension;
		uint64_t onerepExtension;
		uint64_t recapExtension;
		uint64_t split;
		uint64_t node;
		uint64_t qnode;
	};

	struct SearchInfo : public SearchInfoBase {
		double time;
		double nps;
		Move move;
		Value eval;
		int lastDepth;
	};

}

#endif // __SUNFISH_SEARCHINFO__
