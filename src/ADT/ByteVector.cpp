//
//  ADT/ByteVector.cpp
//  ktool
//
//  Created by Suhas Pai on 6/2/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <algorithm>
#include <cstring>

#include "ADT/ByteVector.h"
#include "ADT/Range.h"

ByteVector::ByteVector(const uint64_t Capacity) noexcept {
    Reserve(Capacity);
}

ByteVector::~ByteVector() noexcept {
    delete[] DataBegin;
}

uint64_t ByteVector::getAllocSizeForCapacity(const uint64_t Capacity) noexcept {
    auto ThisCapacity = this->capacity();
    if (ThisCapacity == 0) {
        return Capacity;
    }

    while (ThisCapacity < Capacity) {
        ThisCapacity *= 2;
    }

    return ThisCapacity;
}

void ByteVector::grow() noexcept {
    this->growTo(this->getAllocSizeForCapacity(this->capacity() + 1));
}

void ByteVector::growTo(const uint64_t Capacity) noexcept {
    const auto NewDataBegin = new uint8_t[Capacity];
    std::copy(DataBegin, DataEnd, NewDataBegin);

    delete[] DataBegin;

    this->AllocEnd = NewDataBegin + Capacity;
    this->DataEnd = NewDataBegin + (DataEnd - DataBegin);
    this->DataBegin = NewDataBegin;
}

void ByteVector::growIfNecessaryForSize(const uint64_t Size) noexcept {
    if (this->freeSpace() >= Size) {
        return;
    }

    this->growTo(this->getAllocSizeForCapacity(this->size() + Size));
}

auto ByteVector::Reserve(const uint64_t Capacity) noexcept
    -> decltype(*this)
{
    growTo(Capacity);
    return *this;
}

auto
ByteVector::Add(const uint8_t *const Bytes, const uint64_t Size) noexcept
    -> decltype(*this)
{
    growIfNecessaryForSize(Size);

    memcpy(DataEnd, Bytes, Size);
    DataEnd += Size;

    return *this;
}

auto
ByteVector::Add(const uint8_t *const Begin, uint8_t *const End) noexcept
    -> decltype(*this)
{
    return Add(Begin, (End - Begin));
}

auto
ByteVector::Remove(uint8_t *const Loc, const uint64_t Size) noexcept
    -> decltype(*this)
{
    return Remove(Loc, Loc + Size);
}

auto
ByteVector::Remove(uint8_t *const Loc, uint8_t *const End) noexcept
    -> decltype(*this)
{
    const auto ThisRange = Range::CreateWithEnd(DataBegin, DataEnd);
    const auto LocRange = Range::CreateWithEnd(Loc, End);

    assert(ThisRange.contains(LocRange));

    const auto RemovalSize = (DataEnd - End);
    memmove(Loc, End, RemovalSize);

    DataEnd -= RemovalSize;
    return *this;
}

auto
ByteVector::Remove(const uint64_t Idx, const uint64_t Size) noexcept
    -> decltype(*this)
{
    return Remove(DataBegin + Idx, DataBegin + Idx + Size);
}

