/*
 * @author: Viktor Shishmarev
 * @date: 12.10.2025
 * @description: Comparator utilities for byte-vector based keys and values.
 * Provides lexicographic comparison on unsigned bytes and equality check.
 */

#pragma once

#include <cstdint>
#include <vector>

namespace tbt{

    using ByteVector = std::vector<uint8_t>;

    /*
     * Compare two byte vectors lexicographically using unsigned byte semantics.
     * Parameters:
     *   - leftHand: first operand
     *   - rightHand: second operand
     * Returns:
     *   - true if leftHand is strictly less than rightHand in lexicographic order; false otherwise.
     * Edge cases:
     *   - Empty arrays: an empty array is less than any non-empty array; two empty arrays are not less than each other.
     *   - Unsigned semantics: bytes ≥ 128 compare greater than any smaller unsigned value.
     */
    bool ByteVectorLess(const ByteVector& leftHand, const ByteVector& rightHand) noexcept;

    /*
     * Check equality of two byte vectors.
     * Parameters:
     *   - leftHand: first operand
     *   - rightHand: second operand
     * Returns:
     *   - true if vectors have the same size and identical elements; false otherwise.
     */
    bool ByteVectorEqual(const ByteVector& leftHand, const ByteVector& rightHand) noexcept;
}