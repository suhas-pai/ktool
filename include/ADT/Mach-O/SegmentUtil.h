//
//  ADT/Mach-O/SegmentUtil.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include "SegmentInfo.h"

namespace MachO {
    struct SectionNamePair {
        std::string_view SegmentName;
        std::initializer_list<std::string_view> SectionList;
    };

    struct ConstLoadCommandStorage;
    struct SegmentInfoCollection {
    public:
        enum class Error {
            None,

            InvalidSegment,
            OverlappingSegments,

            InvalidSectionList,
            InvalidSection,

            OverlappingSections
        };
    protected:
        std::vector<std::unique_ptr<SegmentInfo>> List;
        explicit SegmentInfoCollection() noexcept = default;

        void
        ParseFromLoadCommands(const ConstLoadCommandStorage &LoadCmdStorage,
                              bool Is64Bit,
                              Error *ErrorOut) noexcept;
    public:
        [[nodiscard]] static auto
        Open(const ConstLoadCommandStorage &LoadCmdStorage,
             bool Is64Bit,
             Error *ErrorOut) noexcept
                -> SegmentInfoCollection;

        [[nodiscard]] static auto
        OpenSegmentInfoWithName(const ConstLoadCommandStorage &LoadCmdStorage,
                                bool Is64Bit,
                                std::string_view Name,
                                Error *ErrorOut) noexcept
            -> std::unique_ptr<SegmentInfo>;

        [[nodiscard]] static auto
        OpenSectionInfoWithName(const ConstLoadCommandStorage &LoadCmdStorage,
                                bool Is64Bit,
                                std::string_view SegmentName,
                                std::string_view SectionName,
                                std::unique_ptr<SegmentInfo> *SegmentOut,
                                Error *ErrorOut) noexcept
            -> std::unique_ptr<SectionInfo>;

        [[nodiscard]] inline auto &front() const noexcept {
            return *this->List.front().get();
        }

        [[nodiscard]] inline auto &back() const noexcept {
            return *this->List.back().get();
        }

        [[nodiscard]] auto
        GetInfoForName(std::string_view Name) const noexcept
            -> const SegmentInfo *;

        [[nodiscard]] auto
        FindSectionWithName(std::string_view SegmentName,
                            std::string_view Name) const noexcept
            -> const SectionInfo *;

        [[nodiscard]] auto
        FindSegmentContainingAddress(uint64_t Address) const noexcept
            -> const SegmentInfo *;

        [[nodiscard]] auto
        FindSectionWithName(
            const std::initializer_list<SectionNamePair> &List) const noexcept
                -> const SectionInfo *;

        [[nodiscard]] auto
        GetSectionWithIndex(uint64_t SectionIndex) const noexcept
            -> const SectionInfo *;

        [[nodiscard]] auto
        GetDataForVirtualAddr(uint8_t *Map,
                              uint64_t Addr,
                              uint64_t Size,
                              uint8_t **EndOut = nullptr) const noexcept
            -> uint8_t *;

        [[nodiscard]] auto
        GetDataForVirtualAddr(const uint8_t *Map,
                              uint64_t Addr,
                              uint64_t Size,
                              const uint8_t **EndOut = nullptr) const noexcept
            -> const uint8_t *;

        [[nodiscard]] auto
        GetDataForVirtualAddrIgnoreSections(
            uint8_t *Map,
            uint64_t Addr,
            uint64_t Size,
            uint8_t **EndOut = nullptr) const noexcept
                -> uint8_t *;

        [[nodiscard]] auto
        GetDataForVirtualAddrIgnoreSections(
            const uint8_t *Map,
            uint64_t Addr,
            uint64_t Size,
            const uint8_t **EndOut = nullptr) const noexcept
                -> const uint8_t *;

        template <Concepts::NotConst T>
        [[nodiscard]] inline auto
        GetPtrForVirtualAddr(uint8_t *const Map,
                             const uint64_t Addr,
                             const uint64_t Size = sizeof(T),
                             T **const EndOut = nullptr) const noexcept
        {
            const auto Data =
                GetDataForVirtualAddr(Map,
                                      Addr,
                                      Size,
                                      reinterpret_cast<uint8_t **>(EndOut));

            return reinterpret_cast<T *>(Data);
        }

        template <typename T>
        [[nodiscard]] inline auto
        GetPtrForVirtualAddr(const uint8_t *const Map,
                             const uint64_t Addr,
                             const uint64_t Size = sizeof(T),
                             T **const EndOut = nullptr) const noexcept
        {
            const auto Data =
                GetDataForVirtualAddr(
                    Map,
                    Addr,
                    Size,
                    reinterpret_cast<const uint8_t **>(EndOut));

            return reinterpret_cast<T *>(Data);
        }

        template <Concepts::NotConst T>
        [[nodiscard]] inline auto
        GetPtrForVirtualAddrIgnoreSections(
             uint8_t *const Map,
             const uint64_t Addr,
             const uint64_t Size = sizeof(T),
             T **const EndOut = nullptr) const noexcept
        {
            const auto Data =
                GetDataForVirtualAddrIgnoreSections(
                    Map,
                    Addr,
                    Size,
                    reinterpret_cast<uint8_t **>(EndOut));

            return reinterpret_cast<T *>(Data);
        }

        template <typename T>
        [[nodiscard]] inline auto
        GetPtrForVirtualAddrIgnoreSections(
            const uint8_t *const Map,
            const uint64_t Addr,
            const uint64_t Size = sizeof(T),
            T **const EndOut = nullptr) const noexcept
        {
            const auto Data =
                GetDataForVirtualAddrIgnoreSections(
                    Map,
                    Addr,
                    Size,
                    reinterpret_cast<const uint8_t **>(EndOut));

            return reinterpret_cast<T *>(Data);
        }

        [[nodiscard]] inline auto size() const noexcept {
            return this->List.size();
        }

        [[nodiscard]] inline auto &at(const uint64_t Index) const noexcept {
            assert(IndexOutOfBounds(Index, this->size()));
            return *this->List.at(Index).get();
        }

        [[nodiscard]] inline auto atOrNull(const uint64_t Index) const noexcept
            -> const SegmentInfo *
        {
            if (IndexOutOfBounds(Index, this->size())) {
                return nullptr;
            }

            return this->List.at(Index).get();
        }

        [[nodiscard]] inline auto empty() const noexcept {
            return this->List.empty();
        }

        [[nodiscard]] inline auto begin() const noexcept {
            return this->List.cbegin();
        }

        [[nodiscard]] inline auto end() const noexcept {
            return this->List.cend();
        }
    };

    auto
    CollectSegmentInfoList(const ConstLoadCommandStorage &LoadCmdStorage,
                           bool Is64Bit,
                           SegmentInfoCollection &CollectionOut) noexcept
        -> SegmentInfoCollection::Error;
}
