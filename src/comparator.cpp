/*
 * @author: Viktor Shishmarev
 * @date: 12.10.2025
 * @description:
 */

#include "comparator.h"

namespace tbt {
	bool ByteVectorLess(const ByteVector& leftHand, const ByteVector& rightHand) noexcept {
		const std::size_t leftSize = leftHand.size(), rightSize = rightHand.size();

		for (std::size_t iterate = 0; iterate < leftSize && iterate < rightSize; ++iterate) {
			if (leftHand[iterate] < rightHand[iterate]) return true;
			if (leftHand[iterate] > rightHand[iterate]) return false;
		}

		return leftSize < rightSize;
	}

	bool ByteVectorEqual(const ByteVector& leftHand, const ByteVector& rightHand) noexcept{
		if (leftHand.size() != rightHand.size()) {
			return false;
		}

		for (std::size_t iterate = 0; iterate < leftHand.size(); ++iterate) {
			if (leftHand[iterate] != rightHand[iterate]) {
				return false;
			}
		}

		return true;
	}
}