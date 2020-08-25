//
//  include/ADT/Mach-O/ExportTrie.h
//  ktool
//
//  Created by Suhas Pai on 5/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>

#include "ADT/BasicMasksHandler.h"
#include "ADT/EnumHelper.h"
#include "ADT/LargestIntHelper.h"
#include "ADT/LocationRange.h"
#include "ADT/MemoryMap.h"
#include "ADT/PointerErrorStorage.h"
#include "ADT/PointerOrError.h"

#include "LoadCommandsCommon.h"

namespace MachO {
    enum class ExportSymbolMasks : uint8_t {
        KindMask        = 0x3,
        WeakDefinition  = 0x4,
        Reexport        = 0x8,
        StubAndResolver = 0x10
    };

    using ExportSymbolMasksHandler =
        BasicMasksHandler<ExportSymbolMasks, uint8_t>;

    enum class ExportSymbolKind : uint8_t {
        Regular,
        ThreadLocal,
        Absolute
    };

    struct ExportTrieFlags : private ExportSymbolMasksHandler {
    public:
        constexpr ExportTrieFlags() noexcept = default;
        constexpr ExportTrieFlags(uint64_t Byte) noexcept
        : ExportSymbolMasksHandler(Byte) {}

        using Masks = ExportSymbolMasks;
        using Kind = ExportSymbolKind;

        [[nodiscard]] constexpr inline Kind getKind() const noexcept {
            return Kind(getValueForMask(Masks::KindMask));
        }

        [[nodiscard]] constexpr inline bool IsAbsolute() const noexcept {
            return (getKind() == ExportSymbolKind::Absolute);
        }

        [[nodiscard]] constexpr inline bool IsReexport() const noexcept {
            return hasValueForMask(Masks::Reexport);
        }

        [[nodiscard]] constexpr inline bool IsStubAndResolver() const noexcept {
            return hasValueForMask(Masks::StubAndResolver);
        }

        [[nodiscard]] constexpr inline bool IsThreadLocal() const noexcept {
            return (getKind() == ExportSymbolKind::ThreadLocal);
        }

        [[nodiscard]] constexpr inline bool IsWeak() const noexcept {
            return hasValueForMask(Masks::WeakDefinition);
        }

        constexpr inline ExportTrieFlags &setKind(Kind Value) noexcept {
            setValueForMask(Masks::KindMask, static_cast<uint64_t>(Value));
            return *this;
        }

        constexpr inline ExportTrieFlags &setIsAbsolute() noexcept {
            return setKind(ExportSymbolKind::Absolute);
        }

        constexpr
        inline ExportTrieFlags &setIsReexport(bool Value = true) noexcept {
            setValueForMask(Masks::Reexport, Value);
            return *this;
        }

        constexpr inline
        ExportTrieFlags &setIsStubAndResolver(bool Value = true) noexcept {
            setValueForMask(Masks::StubAndResolver, Value);
            return *this;
        }

        constexpr inline ExportTrieFlags &setIsThreadLocal() noexcept {
            return setKind(ExportSymbolKind::ThreadLocal);
        }

        constexpr
        inline ExportTrieFlags &setIsWeak(bool Value = true) noexcept {
            setValueForMask(Masks::WeakDefinition, Value);
            return *this;
        }

        constexpr inline ExportTrieFlags &clear() noexcept {
            this->Integer = 0;
            return *this;
        }
    };

    enum class ExportTrieExportKind {
        None,
        Regular,
        Absolute,
        Reexport,
        WeakDefinition,
        StubAndResolver,
        ThreadLocal
    };

    template <ExportTrieExportKind>
    struct ExportTrieExportKindInfo {};

    template <>
    struct ExportTrieExportKindInfo<ExportTrieExportKind::Regular> {
        constexpr static const auto Kind = ExportTrieExportKind::Regular;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_KIND_REGULAR");
        constexpr static const auto Description =
            std::string_view("Regular");
    };

    template <>
    struct ExportTrieExportKindInfo<ExportTrieExportKind::Absolute> {
        constexpr static const auto Kind = ExportTrieExportKind::Absolute;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_KIND_ABSOLUTE");
        constexpr static const auto Description =
            std::string_view("Absolute");
    };

    template <>
    struct ExportTrieExportKindInfo<ExportTrieExportKind::Reexport> {
        constexpr static const auto Kind = ExportTrieExportKind::Reexport;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_REEXPORT");
        constexpr static const auto Description =
            std::string_view("Re-export");
    };

    template <>
    struct ExportTrieExportKindInfo<ExportTrieExportKind::WeakDefinition> {
        constexpr static const auto Kind = ExportTrieExportKind::WeakDefinition;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION");
        constexpr static const auto Description =
            std::string_view("Weak-Definition");
    };

    template <>
    struct ExportTrieExportKindInfo<ExportTrieExportKind::StubAndResolver> {
        constexpr static const auto Kind =
            ExportTrieExportKind::StubAndResolver;

        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER");
        constexpr static const auto Description =
            std::string_view("Stub-Resolver");
    };

    template <>
    struct ExportTrieExportKindInfo<ExportTrieExportKind::ThreadLocal> {
        constexpr static const auto Kind = ExportTrieExportKind::ThreadLocal;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL");
        constexpr static const auto Description =
            std::string_view("Thread-Local");
    };

    [[nodiscard]] constexpr static const std::string_view &
    ExportTrieExportKindGetName(ExportTrieExportKind Kind) noexcept {
        using Enum = ExportTrieExportKind;
        switch (Kind) {
            case Enum::None:
                return EmptyStringValue;
            case Enum::Regular:
                return ExportTrieExportKindInfo<Enum::Regular>::Name;
            case Enum::Absolute:
                return ExportTrieExportKindInfo<Enum::Absolute>::Name;
            case Enum::Reexport:
                return ExportTrieExportKindInfo<Enum::Reexport>::Name;
            case Enum::WeakDefinition:
                return ExportTrieExportKindInfo<Enum::WeakDefinition>::Name;
            case Enum::StubAndResolver:
                return ExportTrieExportKindInfo<Enum::StubAndResolver>::Name;
            case Enum::ThreadLocal:
                return ExportTrieExportKindInfo<Enum::ThreadLocal>::Name;
        }
    }

    [[nodiscard]] constexpr static const std::string_view &
    ExportTrieExportKindGetDescription(ExportTrieExportKind Kind) noexcept {
        using Enum = ExportTrieExportKind;
        switch (Kind) {
            case Enum::None:
                return EmptyStringValue;
            case Enum::Regular:
                return ExportTrieExportKindInfo<Enum::Regular>::Description;
            case Enum::Absolute:
                return ExportTrieExportKindInfo<Enum::Absolute>::Description;
            case Enum::Reexport:
                return ExportTrieExportKindInfo<Enum::Reexport>::Description;
            case Enum::WeakDefinition:
                return
                    ExportTrieExportKindInfo<Enum::WeakDefinition>::Description;
            case Enum::StubAndResolver:
                return
                    ExportTrieExportKindInfo<Enum::StubAndResolver>
                        ::Description;
            case Enum::ThreadLocal:
                return ExportTrieExportKindInfo<Enum::ThreadLocal>::Description;
        }
    }

    [[nodiscard]] constexpr ExportTrieExportKind
    ExportTrieExportKindFromString(const std::string_view &String) noexcept {
        using Enum = ExportTrieExportKind;

        constexpr auto Kind = ExportTrieExportKind::None;
        switch (Kind) {
            case Enum::None:
            case Enum::Regular:
                if (String == ExportTrieExportKindGetName(Enum::Regular)) {
                    return Enum::Regular;
                }
            case Enum::Absolute:
                if (String == ExportTrieExportKindGetName(Enum::Absolute)) {
                    return Enum::Absolute;
                }
            case Enum::Reexport:
                if (String == ExportTrieExportKindGetName(Enum::Reexport)) {
                    return Enum::Reexport;
                }
            case Enum::WeakDefinition: {
                const auto &Expected =
                    ExportTrieExportKindGetName(Enum::WeakDefinition);

                if (String == Expected) {
                    return Enum::WeakDefinition;
                }
            }
            case Enum::StubAndResolver:{
                const auto &Expected =
                    ExportTrieExportKindGetName(Enum::StubAndResolver);

                if (String == Expected) {
                    return Enum::StubAndResolver;
                }
            }
            case Enum::ThreadLocal:
                if (String == ExportTrieExportKindGetName(Enum::ThreadLocal)) {
                    return Enum::ThreadLocal;
                }
        }

        return Enum::None;
    }

    [[nodiscard]]
    constexpr uint64_t ExportTrieExportKindGetLongestNameLength() noexcept {
        const auto Result =
            EnumHelper<ExportTrieExportKind>::GetLongestAssocLength(
                ExportTrieExportKindGetName);

        return Result;
    }

    [[nodiscard]] constexpr
    uint64_t ExportTrieExportKindGetLongestDescriptionLength() noexcept {
        const auto Result =
            EnumHelper<ExportTrieExportKind>::GetLongestAssocLength(
                ExportTrieExportKindGetDescription);

        return Result;
    }

    struct ExportTrieIterator;
    struct ExportTrieExportIterator;

    struct ExportTrieExportInfo {
    public:
        using Kind = ExportTrieExportKind;
    protected:
        std::string String;
        std::string ReexportImportName;

        union {
            uint32_t ReexportDylibOrdinal;

            struct {
                uint64_t ResolverStubAddress;
                uint64_t ImageOffset;
            };
        };

        ExportTrieFlags Flags;
    public:
        [[nodiscard]]
        constexpr inline std::string_view getString() const noexcept {
            return String;
        }

        [[nodiscard]] constexpr
        inline std::string_view getReexportImportName() const noexcept {
            assert(this->IsReexport());
            return ReexportImportName;
        }

        [[nodiscard]]
        constexpr inline uint32_t getReexportDylibOrdinal() const noexcept {
            assert(this->IsReexport());
            return ReexportDylibOrdinal;
        }

        [[nodiscard]]
        constexpr inline uint64_t getResolverStubAddress() const noexcept {
            assert(this->IsStubAndResolver());
            return ResolverStubAddress;
        }

        [[nodiscard]]
        constexpr inline uint64_t getImageOffset() const noexcept {
            assert(!this->IsReexport());
            return ImageOffset;
        }

        [[nodiscard]]
        constexpr inline ExportTrieFlags getFlags() const noexcept {
            return Flags;
        }

        [[nodiscard]]
        constexpr inline ExportSymbolKind getKind() const noexcept {
            return getFlags().getKind();
        }

        [[nodiscard]] constexpr inline bool IsAbsolute() const noexcept {
            return getFlags().IsAbsolute();
        }

        [[nodiscard]] constexpr inline bool IsReexport() const noexcept {
            return getFlags().IsReexport();
        }

        [[nodiscard]] constexpr inline bool IsStubAndResolver() const noexcept {
            return getFlags().IsStubAndResolver();
        }

        [[nodiscard]] constexpr inline bool IsThreadLocal() const noexcept {
            return getFlags().IsThreadLocal();
        }

        [[nodiscard]] constexpr inline bool IsWeak() const noexcept {
            return getFlags().IsWeak();
        }

        constexpr inline ExportTrieExportInfo &
        setString(const std::string &Value) noexcept {
            this->String = Value;
            return *this;
        }

        constexpr inline ExportTrieExportInfo &
        setReexportImportName(const std::string &Value) noexcept {
            this->ReexportImportName = Value;
            return *this;
        }

        constexpr inline
        ExportTrieExportInfo &setReexportDylibOrdinal(uint32_t Value) noexcept {
            assert(this->IsReexport());

            this->ReexportDylibOrdinal = Value;
            return *this;
        }

        constexpr inline
        ExportTrieExportInfo &setResolverStubAddress(uint64_t Value) noexcept {
            assert(this->IsStubAndResolver());

            this->ResolverStubAddress = Value;
            return *this;
        }

        constexpr
        inline ExportTrieExportInfo &setImageOffset(uint64_t Value) noexcept {
            assert(!this->IsReexport());

            this->ImageOffset = Value;
            return *this;
        }

        constexpr inline
        ExportTrieExportInfo &setFlags(const ExportTrieFlags &Value) noexcept {
            this->Flags = Value;
            return *this;
        }

        constexpr inline ExportTrieExportInfo &clearExclusiveInfo() noexcept {
            ImageOffset = 0;
            ReexportDylibOrdinal = 0;
            ResolverStubAddress = 0;

            Flags.clear();
            return *this;
        }
    };

    enum class ExportTrieParseError {
        None,

        InvalidUleb128,
        InvalidFormat,
        OverlappingRanges,

        EmptyExport,
        TooDeep
    };

    struct ExportTrieNodeInfo {
    protected:
        uint64_t Offset = 0;
        uint64_t Size = 0;
        uint8_t ChildCount = 0;
        std::string_view Prefix;
    public:
        [[nodiscard]] constexpr inline uint64_t getOffset() const noexcept {
            return Offset;
        }

        [[nodiscard]] constexpr inline uint64_t getSize() const noexcept {
            return Size;
        }

        [[nodiscard]] constexpr inline uint16_t getChildCount() const noexcept {
            return ChildCount;
        }

        [[nodiscard]]
        constexpr inline std::string_view getPrefix() const noexcept {
            return Prefix;
        }

        constexpr
        inline ExportTrieNodeInfo &setOffset(uint64_t Value) noexcept {
            this->Offset = Value;
            return *this;
        }

        constexpr
        inline ExportTrieNodeInfo &setSize(uint64_t Value) noexcept {
            this->Size = Value;
            return *this;
        }

        constexpr
        inline ExportTrieNodeInfo &setChildCount(uint16_t Value) noexcept {
            this->ChildCount = Value;
            return *this;
        }

        constexpr
        inline ExportTrieNodeInfo &setPrefix(std::string_view Value) noexcept {
            this->Prefix = Value;
            return *this;
        }

        [[nodiscard]] constexpr inline bool IsExport() const noexcept {
            return (Size != 0);
        }
    };

    struct ExportTrieStackInfo {
    public:
        using NodeInfo = ExportTrieNodeInfo;
    protected:
        NodeInfo Node;
        uint16_t ChildOrdinal = 0;
        std::vector<LocationRange>::size_type RangeListSize = 0;
    public:
        ExportTrieStackInfo() noexcept = default;
        ExportTrieStackInfo(const NodeInfo &Node) noexcept : Node(Node) {}

        [[nodiscard]]
        constexpr inline const NodeInfo &getNode() const noexcept {
            return Node;
        }

        [[nodiscard]] constexpr inline NodeInfo &getNode() noexcept {
            return Node;
        }

        [[nodiscard]]
        constexpr inline uint16_t getChildOrdinal() const noexcept {
            return ChildOrdinal;
        }

        [[nodiscard]] constexpr inline uint16_t &getChildOrdinal() noexcept {
            return ChildOrdinal;
        }

        [[nodiscard]] constexpr
        inline decltype(RangeListSize) getRangeListSize() const noexcept {
            return RangeListSize;
        }

        constexpr
        inline ExportTrieStackInfo &setChildOrdinal(uint16_t Value) noexcept {
            this->ChildOrdinal = Value;
            return *this;
        }

        constexpr inline ExportTrieStackInfo &
        setRangeListSize(decltype(RangeListSize) Value) noexcept {
            this->RangeListSize = Value;
            return *this;
        }
    };

    struct ExportTrieIterateInfo {
        using NodeInfo = ExportTrieNodeInfo;
        using StackInfo = ExportTrieStackInfo;
    protected:
        std::string String;

        std::vector<LocationRange> RangeList;
        std::vector<StackInfo> StackList;

        ExportTrieExportKind Kind;
        ExportTrieExportInfo Export;
    public:
        [[nodiscard]]
        constexpr inline const std::string &getString() const noexcept {
            return String;
        }

        [[nodiscard]] constexpr inline std::string &getString() noexcept {
            return String;
        }

        [[nodiscard]] constexpr
        inline const std::vector<LocationRange> &getRangeList() const noexcept {
            return RangeList;
        }

        [[nodiscard]]
        constexpr inline std::vector<LocationRange> &getRangeList() noexcept {
            return RangeList;
        }

        [[nodiscard]] constexpr
        inline const std::vector<StackInfo> &getStackList() const noexcept {
            return StackList;
        }

        [[nodiscard]]
        constexpr inline std::vector<StackInfo> &getStackList() noexcept {
            return StackList;
        }

        [[nodiscard]]
        constexpr inline ExportTrieExportKind getKind() const noexcept {
            return Kind;
        }

        [[nodiscard]] constexpr
        inline const ExportTrieExportInfo &getExportInfo() const noexcept {
            return Export;
        }

        [[nodiscard]]
        constexpr inline ExportTrieExportInfo &getExportInfo() noexcept {
            return Export;
        }

        constexpr inline
        ExportTrieIterateInfo &setKind(ExportTrieExportKind Value) noexcept {
            this->Kind = Value;
            return *this;
        }

        [[nodiscard]] inline StackInfo &getStack() noexcept {
            assert(!getStackList().empty());
            return getStackList().back();
        }

        [[nodiscard]] inline const StackInfo &getStack() const noexcept {
            assert(!getStackList().empty());
            return getStackList().back();
        }

        [[nodiscard]] inline NodeInfo &getNode() noexcept {
            return getStack().getNode();
        }

        [[nodiscard]] inline const NodeInfo &getNode() const noexcept {
            return getStack().getNode();
        }

        [[nodiscard]] constexpr inline bool IsExport() const noexcept {
            return (getKind() != ExportTrieExportKind::None);
        }

        [[nodiscard]] constexpr inline bool IsAbsolute() const noexcept {
            return (getKind() == ExportTrieExportKind::Absolute);
        }

        [[nodiscard]] constexpr inline bool IsReexport() const noexcept {
            return (getKind() == ExportTrieExportKind::Reexport);
        }

        [[nodiscard]] constexpr inline bool IsStubAndResolver() const noexcept {
            return (getKind() == ExportTrieExportKind::StubAndResolver);
        }

        [[nodiscard]] constexpr inline bool IsWeak() const noexcept {
            return (getKind() == ExportTrieExportKind::WeakDefinition);
        }

        [[nodiscard]] constexpr inline bool IsThreadLocal() const noexcept {
            return (getKind() == ExportTrieExportKind::ThreadLocal);
        }

        [[nodiscard]] inline uint8_t getDepthLevel() const noexcept {
            assert(!getStackList().empty());
            return getStackList().size();
        }
    };

    struct ExportTrieParseOptions {
        uint64_t RangeListReserveSize = 128;
        uint64_t StackListReserveSize = 16;
        uint64_t StringReserveSize = 128;
        uint64_t MaxDepth = 128;
    };

    struct ExportTrieIteratorEnd {};
    struct ExportTrieIterator {
    public:
        using Error = ExportTrieParseError;

        using NodeInfo = ExportTrieNodeInfo;
        using StackInfo = ExportTrieStackInfo;
        using IterateInfo = ExportTrieIterateInfo;

        using ParseOptions = ExportTrieParseOptions;
    protected:
        union {
            PointerErrorStorage<Error> ParseError;
            const uint8_t *Begin;
        };

        const uint8_t *End;
        uint64_t MaxDepth;

        std::unique_ptr<ExportTrieIterateInfo> Info;
        std::unique_ptr<StackInfo> NextStack;

        void SetupInfoForNewStack() noexcept;
        [[nodiscard]] bool MoveUptoParentNode() noexcept;

        Error ParseNode(const uint8_t *Begin, NodeInfo *InfoOut) noexcept;
        Error ParseNextNode(const uint8_t *& Ptr, NodeInfo *InfoOut) noexcept;
        Error Advance() noexcept;
    public:
        explicit
        ExportTrieIterator(
            const uint8_t *Begin,
            const uint8_t *End,
            const ParseOptions &Options = ParseOptions()) noexcept;

        [[nodiscard]] inline ExportTrieIterateInfo &getInfo() noexcept {
            return *Info;
        }

        [[nodiscard]]
        inline const ExportTrieIterateInfo &getInfo() const noexcept {
            return *Info;
        }

        [[nodiscard]] inline bool IsAtEnd() const noexcept {
            return Info->getStackList().empty();
        }

        [[nodiscard]] constexpr inline bool hasError() const noexcept {
            return ParseError.hasValue();
        }

        [[nodiscard]] inline Error getError() const noexcept {
            return ParseError.getValue();
        }

        inline ExportTrieIterator &operator++() noexcept {
            assert(!hasError());
            ParseError = Advance();

            return *this;
        }

        inline ExportTrieIterator &operator++(int) noexcept {
            return ++(*this);
        }

        inline ExportTrieIterator &operator+=(uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] inline ExportTrieIterateInfo &operator*() noexcept {
            return *Info;
        }

        [[nodiscard]]
        inline const ExportTrieIterateInfo &operator*() const noexcept {
            return *Info;
        }

        [[nodiscard]] inline ExportTrieIterateInfo *operator->() noexcept {
            return Info.get();
        }

        [[nodiscard]]
        inline const ExportTrieIterateInfo *operator->() const noexcept {
            return Info.get();
        }

        [[nodiscard]] inline bool operator==(const ExportTrieIteratorEnd &End) {
            return IsAtEnd();
        }

        [[nodiscard]] inline bool operator!=(const ExportTrieIteratorEnd &End) {
            return !(*this == End);
        }
    };

    struct ExportTrieList {
    public:
        using ParseOptions = ExportTrieParseOptions;
    protected:
        uint8_t *Begin;
        uint8_t *End;
    public:
        explicit
        ExportTrieList(uint8_t *Begin, uint8_t *End) noexcept
        : Begin(Begin), End(End) {}

        using IteratorType = ExportTrieIterator;

        [[nodiscard]] IteratorType begin() const noexcept {
            return IteratorType(Begin, End);
        }

        [[nodiscard]] IteratorType cbegin() const noexcept {
            return IteratorType(Begin, End);
        }

        [[nodiscard]]
        IteratorType begin(const ParseOptions &Options) const noexcept {
            return IteratorType(Begin, End, Options);
        }

        [[nodiscard]]
        IteratorType cbegin(const ParseOptions &Options) const noexcept {
            return IteratorType(Begin, End, Options);
        }

        [[nodiscard]] ExportTrieIteratorEnd end() const noexcept {
            return ExportTrieIteratorEnd();
        }

        [[nodiscard]] ExportTrieIteratorEnd cend() const noexcept {
            return ExportTrieIteratorEnd();
        }
    };

    struct ConstExportTrieList : public ExportTrieList {
    public:
        explicit
        ConstExportTrieList(const uint8_t *Begin, const uint8_t *End) noexcept
        : ExportTrieList(const_cast<uint8_t *>(Begin),
                         const_cast<uint8_t *>(End)) {}

        using IteratorType = ExportTrieIterator;

        [[nodiscard]] IteratorType begin() const noexcept {
            return IteratorType(Begin, End);
        }

        [[nodiscard]] IteratorType cbegin() const noexcept {
            return IteratorType(Begin, End);
        }

        [[nodiscard]]
        IteratorType begin(const ParseOptions &Options) const noexcept {
            return IteratorType(Begin, End, Options);
        }

        [[nodiscard]]
        IteratorType cbegin(const ParseOptions &Options) const noexcept {
            return IteratorType(Begin, End, Options);
        }

        [[nodiscard]] ExportTrieIteratorEnd end() const noexcept {
            return ExportTrieIteratorEnd();
        }

        [[nodiscard]] ExportTrieIteratorEnd cend() const noexcept {
            return ExportTrieIteratorEnd();
        }
    };

    struct ExportTrieExportIterator {
    public:
        using Error = ExportTrieParseError;
    protected:
        ExportTrieIterator Iterator;
        void Advance() noexcept;
    public:
        explicit
        ExportTrieExportIterator(const uint8_t *Begin,
                                 const uint8_t *End) noexcept;

        [[nodiscard]] inline ExportTrieIterateInfo &getInfo() noexcept {
            return Iterator.getInfo();
        }

        [[nodiscard]]
        inline const ExportTrieIterateInfo &getInfo() const noexcept {
            return Iterator.getInfo();
        }

        [[nodiscard]] inline bool IsAtEnd() const noexcept {
            return Iterator.IsAtEnd();
        }

        [[nodiscard]] inline bool hasError() const noexcept {
            return Iterator.hasError();
        }

        [[nodiscard]] inline Error getError() const noexcept {
            return Iterator.getError();
        }

        inline ExportTrieExportIterator &operator++() noexcept {
            Advance();
            return *this;
        }

        inline ExportTrieExportIterator &operator++(int) noexcept {
            return ++(*this);
        }

        inline ExportTrieExportIterator &operator+=(uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] inline ExportTrieIterateInfo &operator*() noexcept {
            return Iterator.getInfo();
        }

        [[nodiscard]]
        inline const ExportTrieIterateInfo &operator*() const noexcept {
            return Iterator.getInfo();
        }

        [[nodiscard]] inline ExportTrieIterateInfo *operator->() noexcept {
            return &Iterator.getInfo();
        }

        [[nodiscard]]
        inline const ExportTrieIterateInfo *operator->() const noexcept {
            return &Iterator.getInfo();
        }

        [[nodiscard]] inline bool operator==(const ExportTrieIteratorEnd &End) {
            return (Iterator == End);
        }

        [[nodiscard]] inline bool operator!=(const ExportTrieIteratorEnd &End) {
            return !(*this == End);
        }
    };

    struct ExportTrieExportList {
    protected:
        uint8_t *Begin;
        uint8_t *End;
    public:
        explicit ExportTrieExportList(uint8_t *Begin, uint8_t *End) noexcept
        : Begin(Begin), End(End) {}

        [[nodiscard]] ExportTrieExportIterator begin() const noexcept {
            return ExportTrieExportIterator(Begin, End);
        }

        [[nodiscard]] ExportTrieExportIterator cbegin() const noexcept {
            return ExportTrieExportIterator(Begin, End);
        }

        [[nodiscard]] ExportTrieIteratorEnd end() noexcept {
            return ExportTrieIteratorEnd();
        }

        [[nodiscard]] ExportTrieIteratorEnd cend() const noexcept {
            return ExportTrieIteratorEnd();
        }
    };

    struct ConstExportTrieExportList {
        const uint8_t *Begin;
        const uint8_t *End;
    public:
        constexpr explicit
        ConstExportTrieExportList(const uint8_t *Begin,
                                  const uint8_t *End) noexcept
        : Begin(Begin), End(End) {}

        [[nodiscard]] inline ExportTrieExportIterator begin() const noexcept {
            return ExportTrieExportIterator(Begin, End);
        }

        [[nodiscard]]
        constexpr inline ExportTrieIteratorEnd end() const noexcept {
            return ExportTrieIteratorEnd();
        }

        [[nodiscard]] constexpr inline uint64_t size() const noexcept {
            return static_cast<uint64_t>(End - Begin);
        }
    };

    TypedAllocationOrError<ExportTrieList, SizeRangeError>
    GetExportTrieList(const MemoryMap &Map,
                      uint32_t ExportOff,
                      uint32_t ExportSize) noexcept;

    TypedAllocationOrError<ConstExportTrieList, SizeRangeError>
    GetConstExportTrieList(const ConstMemoryMap &Map,
                           uint32_t ExportOff,
                           uint32_t ExportSize) noexcept;

    TypedAllocationOrError<ExportTrieExportList, SizeRangeError>
    GetExportTrieExportList(const MemoryMap &Map,
                            uint32_t ExportOff,
                            uint32_t ExportSize) noexcept;

    TypedAllocationOrError<ConstExportTrieExportList, SizeRangeError>
    GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                 uint32_t ExportOff,
                                 uint32_t ExportSize) noexcept;

    SizeRangeError
    GetExportListFromExportTrie(
        const ConstMemoryMap &Map,
        uint32_t ExportOff,
        uint32_t ExportSize,
        std::vector<ExportTrieExportInfo> &ExportListOut) noexcept;
}
