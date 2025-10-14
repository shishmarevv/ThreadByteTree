/*
 * @author: Viktor Shishmarev
 * @date: 12.10.2025
 * @description:
 */

#pragma once

#include <cstdint>
#include <vector>

namespace tbt{

    using ByteVector = std::vector<uint8_t>;

    /*
     * @parameters:
     *      leftHand - left hand operand
     *      rightHand - right hand operand
     * @returns:
     *      bool - true if leftHand is strictly less than rightHand in lexicographic order; false otherwise;
     * @edge cases:
     *      Empty arrays: an empty array is less than any non-empty array; two empty arrays are not less than each other.
     *      Unsigned semantics: bytes ≥ 128 compare greater than any smaller unsigned value.
     */

    bool ByteVectorCompare(const ByteVector& leftHand, const ByteVector& rightHand) noexcept;
}