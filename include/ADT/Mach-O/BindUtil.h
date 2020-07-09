//
//  include/ADT/Mach-O/BindUtil.h
//  stool
//
//  Created by Suhas Pai on 6/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>
#include <unordered_map>

#include "BindInfo.h"

namespace MachO {
    struct BindActionCollection {
    public:
        struct Info {
            BindInfoKind Kind;
            BindWriteKind WriteKind;

            int64_t Addend = 0;
            int64_t DylibOrdinal = -1;

            std::string *Symbol;

            int64_t SegmentIndex = -1;
            uint64_t SegOffset = 0;

            uint64_t Address = 0;
            uint64_t OpcodeAddress = 0;
            uint64_t AddrInSeg = 0;

            bool NewSymbolName : 1;
            BindSymbolFlags Flags;

            [[nodiscard]] inline bool hasDylibOrdinal() const noexcept {
                return (DylibOrdinal != -1);
            }

            [[nodiscard]] inline
            bool operator==(const MachO::BindActionInfo &Info) const noexcept {
                if (Info.WriteKind != WriteKind) {
                    return false;
                }

                if (Info.Addend != Addend) {
                    return false;
                }

                if (Info.hasDylibOrdinal() && hasDylibOrdinal()) {
                    if (Info.DylibOrdinal != DylibOrdinal) {
                        return false;
                    }
                }

                if (Info.SymbolName != *Symbol) {
                    return false;
                }

                if (Info.SegmentIndex != SegmentIndex) {
                    return false;
                }

                if (Info.SegOffset != SegOffset) {
                    return false;
                }

                if (Info.AddrInSeg != AddrInSeg) {
                    return false;
                }

                if (Info.Flags != Flags) {
                    return false;
                }

                return true;
            }
        };

        enum class Error {
            None,
            MultipleBindsForAddress
        };

        using ParseError = BindOpcodeParseError;
    protected:
        std::unordered_map<size_t, std::unique_ptr<std::string>> SymbolList;
        std::unordered_map<uint64_t, std::unique_ptr<Info>> ActionList;
    public:
        BindActionCollection() noexcept = default;

        BindActionCollection &
        Parse(const SegmentInfoCollection &SegmentCollection,
              BindActionList *BindList,
              LazyBindActionList *LazyBindList,
              WeakBindActionList *WeakBindList,
              ParseError *ParseErrorOut,
              Error *ErrorOut) noexcept;

        [[nodiscard]] inline static BindActionCollection
        Open(const SegmentInfoCollection &SegmentCollection,
             BindActionList *BindList,
             LazyBindActionList *LazyBindList,
             WeakBindActionList *WeakBindList,
             ParseError *ParseErrorOut,
             Error *ErrorOut) noexcept
        {
            auto Collection = BindActionCollection();
            Collection.Parse(SegmentCollection,
                             BindList,
                             LazyBindList,
                             WeakBindList,
                             ParseErrorOut,
                             ErrorOut);

            return Collection;
        }

        [[nodiscard]]
        const Info *GetInfoForAddress(uint64_t Address) const noexcept;

        [[nodiscard]]
        const std::string *GetSymbolForAddress(uint64_t Address) const noexcept;
    };
}
