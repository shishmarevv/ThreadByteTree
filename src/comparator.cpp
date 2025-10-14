/*
 * @author: Viktor Shishmarev
 * @date: 12.10.2025
 * @description:
 */

#include "comparator.h"

namespace tbt {
	bool ByteVectorCompare(const ByteVector& leftHand, const ByteVector& rightHand) noexcept {
		const std::size_t leftSize = leftHand.size(), rightSize = rightHand.size();

		for (std::size_t iterate = 0; iterate < leftSize && iterate < rightSize; ++iterate) {
			if (leftHand[iterate] < rightHand[iterate]) return true;
			if (leftHand[iterate] > rightHand[iterate]) return false;
		}

		return leftSize < rightSize;
	}
}