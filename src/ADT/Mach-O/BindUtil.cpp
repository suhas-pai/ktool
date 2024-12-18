//
//  ADT/Mach-O/BindUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 6/17/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/BindUtil.h"

namespace MachO {
    static auto
    GetPtrForSymbol(const std::string_view Symbol,
                    BindActionCollection::SymbolListType &SymbolList) noexcept
        -> std::string *
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
    static auto
    CollectActionListFromTrie(const T &BindList,
                              const SegmentInfoCollection &SegmentCollection,
                              BindActionCollection::SymbolListType &SymbolList,
                              BindActionCollection::ActionListType &ActionList,
                              const Range &Range,
                              BindActionCollection::ParseError *ParseErrorOut,
                              BindActionCollection::Error *ErrorOut) noexcept
    {
        auto Segment = static_cast<const SegmentInfo *>(nullptr);
        auto SegmentIndex = static_cast<int64_t>(-1);
        auto SegmentAddress = uint64_t();

        const auto End = BindList.end();
        auto NewSymbol = true;

        for (auto Iter = BindList.begin(); Iter != End; Iter++) {
            const auto &Info = *Iter;
            const auto ParseError = Info.getError();

            switch (BindKind) {
                case BindInfoKind::Normal:
                    if (ParseError != BindOpcodeParseError::None) {
                        if (ParseErrorOut != nullptr) {
                            *ParseErrorOut = ParseError;
                        }

                        return false;
                    }

                    break;

                case BindInfoKind::Lazy:
                    if (ParseError != BindOpcodeParseError::None &&
                        ParseError != BindOpcodeParseError::NoWriteKind)
                    {
                        if (ParseErrorOut != nullptr) {
                            *ParseErrorOut = ParseError;
                        }

                        return false;
                    }

                    break;

                case BindInfoKind::Weak:
                    break;
            }

            const auto &Action = Info.getAction();
            if (Action.NewSymbolName) {
                NewSymbol = Action.NewSymbolName;
            }

            if (SegmentIndex != Action.SegmentIndex) {
                Segment = SegmentCollection.atOrNull(Action.SegmentIndex);
                SegmentIndex = Action.SegmentIndex;
                SegmentAddress = Segment->getMemoryRange().getBegin();
            }

            const auto FullAddr = SegmentAddress + Action.AddrInSeg;
            const auto &ActionListIter = ActionList.find(FullAddr);

            if (!Range.empty()) {
                if (!Range.hasLocation(FullAddr)) {
                    continue;
                }
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

            auto NewAction = BindActionCollection::Info();
            NewAction.setKind(BindKind)
                     .setWriteKind(Action.WriteKind)
                     .setAddend(Action.Addend)
                     .setDylibOrdinal(
                          static_cast<uint32_t>(Action.DylibOrdinal))
                     .setSegmentIndex(
                          static_cast<uint32_t>(Action.SegmentIndex))
                     .setSegmentOffset(Action.SegOffset)
                     .setAddress(FullAddr)
                     .setOpcodeAddress(Iter.getOffset(BindList.getBegin()))
                     .setAddrInSeg(Action.AddrInSeg)
                     .setIsNewSymbolName(NewSymbol)
                     .setFlags(Action.Flags);

            if (NewSymbol) {
                const auto Symbol =
                    GetPtrForSymbol(Action.SymbolName, SymbolList);

                NewAction.setSymbol(Symbol);
                NewSymbol = false;
            }

            ActionList.insert({
                FullAddr,
                std::make_unique<BindActionCollection::Info>(
                    std::move(NewAction))
            });
        }

        return true;
    }

    auto
    BindActionCollection::Parse(const SegmentInfoCollection &SegmentCollection,
                                const BindActionList *const BindList,
                                const LazyBindActionList *const LazyBindList,
                                const WeakBindActionList *const WeakBindList,
                                const Range &Range,
                                ParseError *const ParseErrorOut,
                                Error *const ErrorOut) noexcept
        -> decltype(*this)
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

    auto BindActionCollection::GetInfoForAddress(
        const uint64_t Address) const noexcept
            -> const BindActionCollection::Info *
    {
        const auto Iter = ActionList.find(Address);
        if (Iter != ActionList.end()) {
            return Iter->second.get();
        }

        return nullptr;
    }

    auto
    BindActionCollection::GetSymbolForAddress(uint64_t Address) const noexcept
        -> const std::string *
    {
        if (const auto *Info = GetInfoForAddress(Address)) {
            return &Info->getSymbolRef();
        }

        return nullptr;
    }
}
