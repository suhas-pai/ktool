//
//  src/ADT/Mach-O/BindUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 6/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "BindUtil.h"

namespace MachO {
    static std::string *
    GetPtrForSymbol(
        const std::string_view &Symbol,
        BindActionCollection::SymbolListType &SymbolList) noexcept
    {
        const auto Hash = std::hash<std::string_view>()(Symbol);
        const auto Iter = SymbolList.find(Hash);

        if (Iter != SymbolList.end()) {
            return Iter->second.get();
        }

        const auto InsertPtr =
            SymbolList.insert({
                Hash,
                std::make_unique<std::string>(Symbol)
            }).first->second.get();

        return InsertPtr;
    }

    template <BindInfoKind BindKind, typename T>
    static bool
    CollectActionListFromTrie(
        const T &BindList,
        const SegmentInfoCollection &SegmentCollection,
        BindActionCollection::SymbolListType &SymbolList,
        BindActionCollection::ActionListType &ActionList,
        const LocationRange &Range,
        BindActionCollection::ParseError *ParseErrorOut,
        BindActionCollection::Error *ErrorOut) noexcept
    {
        auto Segment = static_cast<const SegmentInfo *>(nullptr);
        auto SegmentIndex = static_cast<int64_t>(-1);
        auto SegmentAddress = uint64_t();

        const auto End = BindList.end();
        auto Symbol = static_cast<std::string *>(nullptr);

        for (auto Iter = BindList.begin(); Iter != End; Iter++) {
            const auto &Info = *Iter;
            const auto ParseError = Info.getError();

            if (!Info.canIgnoreError(ParseError)) {
                if (ParseErrorOut != nullptr) {
                    *ParseErrorOut = ParseError;
                }

                return false;
            }

            const auto &Action = Info.getAction();
            if (SegmentIndex != Action.SegmentIndex) {
                Segment = SegmentCollection.atOrNull(Action.SegmentIndex);
                SegmentIndex = Action.SegmentIndex;
                SegmentAddress = Segment->getMemoryRange().getBegin();
            }

            const auto FullAddr = SegmentAddress + Action.AddrInSeg;
            const auto &ActionListIter = ActionList.find(FullAddr);

            if (!Range.empty()) {
                if (!Range.containsLocation(FullAddr)) {
                    continue;
                }
            }

            if (Action.NewSymbolName) {
                Symbol = GetPtrForSymbol(Action.SymbolName, SymbolList);
            }

            if (ActionListIter != ActionList.end()) {
                if (*ActionListIter->second == Action) {
                    continue;
                }

                if (ErrorOut != nullptr) {
                    using Enum = BindActionCollection::Error;
                    *ErrorOut = Enum::MultipleBindsForAddress;
                }

                return false;
            }

            const auto NewAction = BindActionCollection::Info {
                .Kind = BindKind,
                .WriteKind = Action.WriteKind,
                .Addend = Action.Addend,
                .DylibOrdinal = Action.DylibOrdinal,
                .Symbol = Symbol,
                .SegmentIndex = Action.SegmentIndex,
                .SegOffset = Action.SegOffset,
                .Address = FullAddr,
                .OpcodeAddress = Iter.getOffset(BindList.getBegin()),
                .AddrInSeg = Action.AddrInSeg,
                .NewSymbolName = Action.NewSymbolName,
                .Flags = Action.Flags
            };

            ActionList.insert({
                FullAddr,
                std::make_unique<BindActionCollection::Info>(
                    std::move(NewAction))
            });
        }

        return true;
    }

    BindActionCollection &
    BindActionCollection::Parse(const SegmentInfoCollection &SegmentCollection,
                                BindActionList *BindList,
                                LazyBindActionList *LazyBindList,
                                WeakBindActionList *WeakBindList,
                                const LocationRange &Range,
                                ParseError *ParseErrorOut,
                                Error *ErrorOut) noexcept
    {
        auto ParseResult = false;
        if (BindList != nullptr) {
            ParseResult =
                CollectActionListFromTrie<BindInfoKind::Normal>(
                    *BindList,
                    SegmentCollection,
                    SymbolList,
                    ActionList,
                    Range,
                    ParseErrorOut,
                    ErrorOut);

            if (!ParseResult) {
                return *this;
            }
        }

        if (LazyBindList != nullptr) {
            ParseResult =
                CollectActionListFromTrie<BindInfoKind::Lazy>(*LazyBindList,
                                                              SegmentCollection,
                                                              SymbolList,
                                                              ActionList,
                                                              Range,
                                                              ParseErrorOut,
                                                              ErrorOut);

            if (!ParseResult) {
                return *this;
            }
        }

        if (WeakBindList != nullptr) {
            ParseResult =
                CollectActionListFromTrie<BindInfoKind::Weak>(*WeakBindList,
                                                              SegmentCollection,
                                                              SymbolList,
                                                              ActionList,
                                                              Range,
                                                              ParseErrorOut,
                                                              ErrorOut);

            if (!ParseResult) {
                return *this;
            }
        }

        return *this;
    }

    const BindActionCollection::Info *
    BindActionCollection::GetInfoForAddress(uint64_t Address) const noexcept {
        const auto Iter = ActionList.find(Address);
        if (Iter != ActionList.end()) {
            return Iter->second.get();
        }

        return nullptr;
    }

    const std::string *
    BindActionCollection::GetSymbolForAddress(uint64_t Address) const noexcept {
        if (const auto *Info = GetInfoForAddress(Address)) {
            return Info->Symbol;
        }

        return nullptr;
    }
}
