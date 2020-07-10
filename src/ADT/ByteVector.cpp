//
//  src/ADT/ByteVector.cpp
//  ktool
//
//  Created by Suhas Pai on 6/2/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <algorithm>
#include <cstring>

#include "ByteVector.h"
#include "LocationRange.h"
#include "RelativeRange.h"

ByteVector::ByteVector(uint64_t Capacity) noexcept {
    Reserve(Capacity);
}

ByteVector::~ByteVector() noexcept {
    delete[] DataBegin;
}

uint64_t ByteVector::getAllocSizeForCapacity(uint64_t Capacity) noexcept {
    auto ThisCapacity = capacity();
    if (ThisCapacity == 0) {
        return Capacity;
    }

    while (ThisCapacity < Capacity) {
        ThisCapacity *= 2;
    }

    return ThisCapacity;
}

void ByteVector::grow() noexcept {
    growTo(getAllocSizeForCapacity(capacity()));
}

void ByteVector::growTo(uint64_t Capacity) noexcept {
    const auto NewDataBegin = new uint8_t[Capacity];
    std::copy(DataBegin, DataEnd, NewDataBegin);

    delete[] DataBegin;

    this->AllocEnd = NewDataBegin + Capacity;
    this->DataEnd = NewDataBegin + (DataEnd - DataBegin);
    this->DataBegin = NewDataBegin;
}

void ByteVector::growIfNecessaryForSize(uint64_t Size) noexcept {
    if (freeSpace() >= Size) {
        return;
    }

    growTo(getAllocSizeForCapacity(size() + Size));
}

ByteVector &ByteVector::Reserve(uint64_t Capacity) noexcept {
    growTo(Capacity);
    return *this;
}

ByteVector &ByteVector::Add(const uint8_t *Bytes, uint64_t Size) noexcept {
    growIfNecessaryForSize(Size);

    memcpy(DataEnd, Bytes, Size);
    DataEnd += Size;

    return *this;
}

ByteVector &ByteVector::Add(const uint8_t *Begin, uint8_t *End) noexcept {
    return Add(Begin, (End - Begin));
}

ByteVector &ByteVector::Remove(uint8_t *Loc, uint64_t Size) noexcept {
    return Remove(Loc, Loc + Size);
}

ByteVector &ByteVector::Remove(uint8_t *Loc, uint8_t *End) noexcept {
    const auto ThisRange = LocationRange::CreateWithEnd(DataBegin, DataEnd);
    const auto LocRange = LocationRange::CreateWithEnd(Loc, End);

    assert(ThisRange.contains(LocRange));

    const auto RemovalSize = (DataEnd - End);
    memmove(Loc, End, RemovalSize);

    DataEnd -= RemovalSize;
    return *this;
}

ByteVector &ByteVector::Remove(uint64_t Idx, uint64_t Size) noexcept {
    return Remove(DataBegin + Idx, DataBegin + Idx + Size);
}

