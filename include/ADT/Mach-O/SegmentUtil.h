//
//  include/ADT/Mach-O/SegmentUtil.h
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <type_traits>

#include "ADT/MemoryMap.h"
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
        [[nodiscard]] static SegmentInfoCollection
        Open(const ConstLoadCommandStorage &LoadCmdStorage,
             bool Is64Bit,
             Error *ErrorOut) noexcept;

        [[nodiscard]] static std::unique_ptr<SegmentInfo>
        OpenSegmentInfoWithName(const ConstLoadCommandStorage &LoadCmdStorage,
                                bool Is64Bit,
                                const std::string_view &Name,
                                Error *ErrorOut) noexcept;

        [[nodiscard]] static std::unique_ptr<SectionInfo>
        OpenSectionInfoWithName(const ConstLoadCommandStorage &LoadCmdStorage,
                                bool Is64Bit,
                                const std::string_view &SegmentName,
                                const std::string_view &SectionName,
                                std::unique_ptr<SegmentInfo> *SegmentOut,
                                Error *ErrorOut) noexcept;

        [[nodiscard]] const SegmentInfo &front() const noexcept {
            return *List.front().get();
        }

        [[nodiscard]] const SegmentInfo &back() const noexcept {
            return *List.back().get();
        }

        [[nodiscard]] const SegmentInfo *
        GetInfoForName(const std::string_view &Name) const noexcept;

        [[nodiscard]] const SectionInfo *
        FindSectionWithName(const std::string_view &SegmentName,
                            const std::string_view &Name) const noexcept;

        [[nodiscard]] const SegmentInfo *
        FindSegmentContainingAddress(uint64_t Address) const noexcept;

        [[nodiscard]] const SectionInfo *
        FindSectionWithName(
            const std::initializer_list<SectionNamePair> &List) const noexcept;

        [[nodiscard]] const SectionInfo *
        GetSectionWithIndex(uint64_t SectionIndex) const noexcept;

        [[nodiscard]] uint8_t *
        GetDataForVirtualAddr(uint8_t *Map,
                              uint64_t Addr,
                              uint64_t Size,
                              uint8_t **EndOut = nullptr) const noexcept;

        [[nodiscard]] const uint8_t *
        GetDataForVirtualAddr(const uint8_t *Map,
                              uint64_t Addr,
                              uint64_t Size,
                              const uint8_t **EndOut = nullptr) const noexcept;

        [[nodiscard]] uint8_t *
        GetDataForVirtualAddrIgnoreSections(
            uint8_t *Map,
            uint64_t Addr,
            uint64_t Size,
            uint8_t **EndOut = nullptr) const noexcept;

        [[nodiscard]] const uint8_t *
        GetDataForVirtualAddrIgnoreSections(
            const uint8_t *Map,
            uint64_t Addr,
            uint64_t Size,
            const uint8_t **EndOut = nullptr) const noexcept;

        template <typename T, typename = std::enable_if_t<!std::is_const_v<T>>>
        [[nodiscard]] T *
        GetPtrForVirtualAddr(uint8_t *Map,
                             uint64_t Addr,
                             uint64_t Size = sizeof(T),
                             T **EndOut = nullptr) const noexcept
        {
            const auto Data =
                GetDataForVirtualAddr(Map,
                                      Addr,
                                      Size,
                                      reinterpret_cast<uint8_t **>(EndOut));

            return reinterpret_cast<T *>(Data);
        }

        template <typename T>
        [[nodiscard]] T *
        GetPtrForVirtualAddr(const uint8_t *Map,
                             uint64_t Addr,
                             uint64_t Size = sizeof(T),
                             T **EndOut = nullptr) const noexcept
        {
            const auto Data =
                GetDataForVirtualAddr(
                    Map,
                    Addr,
                    Size,
                    reinterpret_cast<const uint8_t **>(EndOut));

            return reinterpret_cast<T *>(Data);
        }

        template <typename T, typename = std::enable_if_t<!std::is_const_v<T>>>
        [[nodiscard]] T *
        GetPtrForVirtualAddrIgnoreSections(
             uint8_t *Map,
             uint64_t Addr,
             uint64_t Size = sizeof(T),
             T **EndOut = nullptr) const noexcept
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
        [[nodiscard]] T *
        GetPtrForVirtualAddrIgnoreSections(
            const uint8_t *Map,
            uint64_t Addr,
            uint64_t Size = sizeof(T),
            T **EndOut = nullptr) const noexcept
        {
            const auto Data =
                GetDataForVirtualAddrIgnoreSections(
                    Map,
                    Addr,
                    Size,
                    reinterpret_cast<const uint8_t **>(EndOut));

            return reinterpret_cast<T *>(Data);
        }

        [[nodiscard]]
        inline const SegmentInfo &at(uint64_t Index) const noexcept {
            assert(IndexOutOfBounds(Index, this->size()));
            return *List.at(Index).get();
        }

        [[nodiscard]]
        inline const SegmentInfo *atOrNull(uint64_t Index) const noexcept {
            if (IndexOutOfBounds(Index, this->size())) {
                return nullptr;
            }

            return List.at(Index).get();
        }

        [[nodiscard]] inline bool empty() const noexcept {
            return List.empty();
        }

        [[nodiscard]] inline uint64_t size() const noexcept {
            return List.size();
        }

        [[nodiscard]]
        inline decltype(List)::const_iterator begin() const noexcept {
            return List.cbegin();
        }

        [[nodiscard]]
        inline decltype(List)::const_iterator end() const noexcept {
            return List.cend();
        }
    };

    SegmentInfoCollection::Error
    CollectSegmentInfoList(const ConstLoadCommandStorage &LoadCmdStorage,
                           bool Is64Bit,
                           SegmentInfoCollection &CollectionOut) noexcept;
}
