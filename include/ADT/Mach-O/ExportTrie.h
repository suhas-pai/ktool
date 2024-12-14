//
//  ADT/Mach-O/ExportTrie.h
//  ktool
//
//  Created by Suhas Pai on 5/18/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>

#include "ADT/BasicMasksHandler.h"
#include "ADT/EnumHelper.h"
#include "ADT/ExpectedAlloc.h"
#include "ADT/Range.h"
#include "ADT/MemoryMap.h"

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
        constexpr ExportTrieFlags(const uint64_t Byte) noexcept
        : ExportSymbolMasksHandler(Byte) {}

        using Masks = ExportSymbolMasks;
        using Kind = ExportSymbolKind;

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return Kind(this->getValueForMask(Masks::KindMask));
        }

        [[nodiscard]] constexpr auto isAbsolute() const noexcept {
            return this->getKind() == ExportSymbolKind::Absolute;
        }

        [[nodiscard]] constexpr auto isRegular() const noexcept {
            return this->getKind() == ExportSymbolKind::Absolute;
        }

        [[nodiscard]] constexpr auto isReexport() const noexcept {
            return this->hasValueForMask(Masks::Reexport);
        }

        [[nodiscard]] constexpr auto isStubAndResolver() const noexcept {
            return this->hasValueForMask(Masks::StubAndResolver);
        }

        [[nodiscard]] constexpr auto isThreadLocal() const noexcept {
            return this->getKind() == ExportSymbolKind::ThreadLocal;
        }

        [[nodiscard]] constexpr auto isWeak() const noexcept {
            return this->hasValueForMask(Masks::WeakDefinition);
        }

        constexpr auto setKind(const Kind Value) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::KindMask, static_cast<uint64_t>(Value));
            return *this;
        }

        constexpr auto setIsAbsolute() noexcept -> decltype(*this) {
            return this->setKind(ExportSymbolKind::Absolute);
        }

        constexpr auto setIsRegular() noexcept -> decltype(*this) {
            return this->setKind(ExportSymbolKind::Regular);
        }

        constexpr auto setIsReexport(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::Reexport, Value);
            return *this;
        }

        constexpr auto setIsStubAndResolver(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::StubAndResolver, Value);
            return *this;
        }

        constexpr auto setIsThreadLocal() noexcept -> decltype(*this) {
            return this->setKind(ExportSymbolKind::ThreadLocal);
        }

        constexpr auto setIsWeak(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(Masks::WeakDefinition, Value);
            return *this;
        }

        constexpr auto clear() noexcept -> decltype(*this) {
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

    [[nodiscard]] constexpr auto
    ExportTrieExportKindGetName(const ExportTrieExportKind Kind) noexcept
        -> std::optional<std::string_view>
    {
        using Enum = ExportTrieExportKind;
        switch (Kind) {
            case Enum::None:
                break;
            case Enum::Regular:
                return "EXPORT_SYMBOL_FLAGS_KIND_REGULAR";
            case Enum::Absolute:
                return "EXPORT_SYMBOL_FLAGS_KIND_ABSOLUTE";
            case Enum::Reexport:
                return "EXPORT_SYMBOL_FLAGS_REEXPORT";
            case Enum::WeakDefinition:
                return "EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION";
            case Enum::StubAndResolver:
                return "EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER";
            case Enum::ThreadLocal:
                return "EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL";
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    ExportTrieExportKindGetDescription(
        const ExportTrieExportKind Kind) noexcept
        -> std::optional<std::string_view>
    {
        using Enum = ExportTrieExportKind;
        switch (Kind) {
            case Enum::None:
                break;
            case Enum::Regular:
                return "Regular";
            case Enum::Absolute:
                return "Absolute";
            case Enum::Reexport:
                return "Re-export";
            case Enum::WeakDefinition:
                return "Weak-Definition";
            case Enum::StubAndResolver:
                return "Stub-Resolver";
            case Enum::ThreadLocal:
                return "Thread-Local";
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    ExportTrieExportKindFromString(const std::string_view String) noexcept {
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
    constexpr auto ExportTrieExportKindGetLongestNameLength() noexcept {
        const auto Result =
            EnumHelper<ExportTrieExportKind>::GetLongestAssocLength(
                [](const ExportTrieExportKind Kind) {
                    return ExportTrieExportKindGetName(Kind).value_or("");
                });

        return Result;
    }

    [[nodiscard]] inline auto
    ExportTrieExportKindGetLongestDescriptionLength() noexcept {
        const auto Result =
            EnumHelper<ExportTrieExportKind>::GetLongestAssocLength(
                [](const ExportTrieExportKind Kind) {
                    return
                        ExportTrieExportKindGetDescription(Kind).value_or("");
                });

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
        [[nodiscard]] constexpr auto getString() const noexcept {
            return this->String;
        }

        [[nodiscard]] constexpr auto getFlags() const noexcept {
            return this->Flags;
        }

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->getFlags().getKind();
        }

        [[nodiscard]] constexpr auto isAbsolute() const noexcept {
            return this->getFlags().isAbsolute();
        }

        [[nodiscard]] constexpr auto isReexport() const noexcept {
            return this->getFlags().isReexport();
        }

        [[nodiscard]] constexpr bool isStubAndResolver() const noexcept {
            return this->getFlags().isStubAndResolver();
        }

        [[nodiscard]] constexpr auto isThreadLocal() const noexcept {
            return this->getFlags().isThreadLocal();
        }

        [[nodiscard]] constexpr auto isWeak() const noexcept {
            return this->getFlags().isWeak();
        }

        [[nodiscard]] constexpr auto getReexportImportName() const noexcept {
            assert(this->isReexport());
            return this->ReexportImportName;
        }

        [[nodiscard]] constexpr auto getReexportDylibOrdinal() const noexcept {
            assert(this->isReexport());
            return this->ReexportDylibOrdinal;
        }

        [[nodiscard]] constexpr auto getResolverStubAddress() const noexcept {
            assert(this->isStubAndResolver());
            return this->ResolverStubAddress;
        }

        [[nodiscard]] constexpr auto getImageOffset() const noexcept {
            assert(!this->isReexport());
            return this->ImageOffset;
        }

        constexpr auto setString(std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->String = Value;
            return *this;
        }

        constexpr auto setString(const std::string &&Value) noexcept
            -> decltype(*this)
        {
            this->String = std::move(Value);
            return *this;
        }

        constexpr auto
        setReexportImportName(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->ReexportImportName = Value;
            return *this;
        }

        constexpr auto setReexportDylibOrdinal(const uint32_t Value) noexcept
            -> decltype(*this)
        {
            assert(this->isReexport());

            this->ReexportDylibOrdinal = Value;
            return *this;
        }

        constexpr auto setResolverStubAddress(uint64_t Value) noexcept
            -> decltype(*this)
        {
            assert(this->isStubAndResolver());

            this->ResolverStubAddress = Value;
            return *this;
        }

        constexpr auto setImageOffset(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            assert(!this->isReexport());

            this->ImageOffset = Value;
            return *this;
        }

        constexpr auto setFlags(const ExportTrieFlags &Value) noexcept
            -> decltype(*this)
        {
            this->Flags = Value;
            return *this;
        }

        constexpr auto clearExclusiveInfo() noexcept -> decltype(*this) {
            this->ImageOffset = 0;
            this->ReexportDylibOrdinal = 0;
            this->ResolverStubAddress = 0;

            this->getFlags().clear();
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
        [[nodiscard]] constexpr auto getOffset() const noexcept {
            return this->Offset;
        }

        [[nodiscard]] constexpr auto &getOffsetRef() noexcept {
            return this->Offset;
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return this->Size;
        }

        [[nodiscard]] constexpr auto getChildCount() const noexcept {
            return this->ChildCount;
        }

        [[nodiscard]] constexpr auto getPrefix() const noexcept {
            return this->Prefix;
        }

        constexpr auto setOffset(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->Offset = Value;
            return *this;
        }

        constexpr auto setSize(uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->Size = Value;
            return *this;
        }

        constexpr auto setChildCount(uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->ChildCount = Value;
            return *this;
        }

        constexpr auto setPrefix(std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->Prefix = Value;
            return *this;
        }

        [[nodiscard]] constexpr auto isExport() const noexcept {
            return this->Size != 0;
        }
    };

    struct ExportTrieStackInfo {
    public:
        using NodeInfo = ExportTrieNodeInfo;
    protected:
        NodeInfo Node;
        uint16_t ChildOrdinal = 0;
        std::vector<Range>::size_type RangeListSize = 0;
    public:
        explicit ExportTrieStackInfo() noexcept = default;
        explicit ExportTrieStackInfo(const NodeInfo &Node) noexcept
        : Node(Node) {}

        [[nodiscard]]
        constexpr auto &getNode() const noexcept {
            return this->Node;
        }

        [[nodiscard]] constexpr auto &getNode() noexcept {
            return this->Node;
        }

        [[nodiscard]] constexpr auto getChildOrdinal() const noexcept {
            return this->ChildOrdinal;
        }

        [[nodiscard]] constexpr auto &getChildOrdinalRef() noexcept {
            return this->ChildOrdinal;
        }

        [[nodiscard]] constexpr auto getRangeListSize() const noexcept {
            return this->RangeListSize;
        }

        constexpr auto setChildOrdinal(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->ChildOrdinal = Value;
            return *this;
        }

        constexpr auto
        setRangeListSize(const decltype(RangeListSize) Value) noexcept
            -> decltype(*this)
        {
            this->RangeListSize = Value;
            return *this;
        }
    };

    struct ExportTrieIterateInfo {
        using NodeInfo = ExportTrieNodeInfo;
        using StackInfo = ExportTrieStackInfo;
    protected:
        uint64_t MaxDepth;
        std::string String;

        std::vector<Range> RangeList;
        std::vector<StackInfo> StackList;

        ExportTrieExportKind Kind;
        ExportTrieExportInfo Export;
    public:
        [[nodiscard]] constexpr auto getMaxDepth() const noexcept {
            return this->MaxDepth;
        }

        [[nodiscard]] constexpr auto getString() const noexcept {
            return this->String;
        }

        [[nodiscard]] constexpr auto &getStringRef() noexcept {
            return this->String;
        }

        [[nodiscard]] constexpr const auto &getRangeList() const noexcept {
            return this->RangeList;
        }

        [[nodiscard]] constexpr auto &getRangeListRef() noexcept {
            return this->RangeList;
        }

        [[nodiscard]] constexpr auto &getStackList() const noexcept {
            return this->StackList;
        }

        [[nodiscard]] constexpr auto &getStackListRef() noexcept {
            return this->StackList;
        }

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }

        [[nodiscard]] constexpr auto &getExportInfo() const noexcept {
            return this->Export;
        }

        [[nodiscard]] constexpr auto &getExportInfoRef() noexcept {
            return this->Export;
        }

        constexpr auto setMaxDepth(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->MaxDepth = Value;
            return *this;
        }

        constexpr auto setKind(const ExportTrieExportKind Value) noexcept
            -> decltype(*this)
        {
            this->Kind = Value;
            return *this;
        }

        [[nodiscard]] inline auto &getStack() noexcept {
            assert(!this->getStackListRef().empty());
            return this->getStackListRef().back();
        }

        [[nodiscard]] inline const auto &getStack() const noexcept {
            assert(!this->getStackList().empty());
            return this->getStackList().back();
        }

        [[nodiscard]] inline auto &getNode() noexcept {
            return this->getStack().getNode();
        }

        [[nodiscard]] inline const auto &getNode() const noexcept {
            return this->getStack().getNode();
        }

        [[nodiscard]] constexpr auto isExport() const noexcept {
            return this->getKind() != ExportTrieExportKind::None;
        }

        [[nodiscard]] constexpr auto isAbsolute() const noexcept {
            return this->getKind() == ExportTrieExportKind::Absolute;
        }

        [[nodiscard]] constexpr auto isReexport() const noexcept {
            return this->getKind() == ExportTrieExportKind::Reexport;
        }

        [[nodiscard]] constexpr auto isRegular() const noexcept {
            return this->getKind() == ExportTrieExportKind::Regular;
        }

        [[nodiscard]] constexpr auto isStubAndResolver() const noexcept {
            return this->getKind() == ExportTrieExportKind::StubAndResolver;
        }

        [[nodiscard]] constexpr auto isWeak() const noexcept {
            return this->getKind() == ExportTrieExportKind::WeakDefinition;
        }

        [[nodiscard]] constexpr auto isThreadLocal() const noexcept {
            return this->getKind() == ExportTrieExportKind::ThreadLocal;
        }

        constexpr auto setIsAbsolute() noexcept -> decltype(*this) {
            this->setKind(ExportTrieExportKind::Absolute);
            return *this;
        }

        constexpr auto setIsReexport() noexcept -> decltype(*this) {
            this->setKind(ExportTrieExportKind::Reexport);
            return *this;
        }

        constexpr auto setIsRegular() noexcept -> decltype(*this) {
            this->setKind(ExportTrieExportKind::Regular);
            return *this;
        }

        constexpr auto setIsStubAndResolver() noexcept -> decltype(*this) {
            this->setKind(ExportTrieExportKind::StubAndResolver);
            return *this;
        }

        constexpr auto setIsWeak() noexcept -> decltype(*this) {
            this->setKind(ExportTrieExportKind::WeakDefinition);
            return *this;
        }

        constexpr auto setIsThreadLocal() noexcept -> decltype(*this) {
            this->setKind(ExportTrieExportKind::ThreadLocal);
            return *this;
        }

        [[nodiscard]] inline auto getDepthLevel() const noexcept {
            assert(!this->getStackList().empty());
            return static_cast<uint8_t>(this->getStackList().size());
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
        const uint8_t *Begin;
        ExpectedPointer<const uint8_t, Error> EndOrError;

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

        [[nodiscard]] inline auto &getInfo() noexcept {
            return *this->Info;
        }

        [[nodiscard]] inline auto &getInfo() const noexcept {
            return *this->Info;
        }

        [[nodiscard]] inline auto isAtEnd() const noexcept {
            return this->getInfo().getStackListRef().empty();
        }

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return this->EndOrError.hasError();
        }

        [[nodiscard]] constexpr auto getError() const noexcept {
            return this->EndOrError.getError();
        }

        inline auto operator++() noexcept -> decltype(*this) {
            assert(!this->hasError());

            this->EndOrError = this->Advance();
            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            return ++(*this);
        }

        inline auto operator+=(const uint64_t Amt) noexcept -> decltype(*this) {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] inline auto &operator*() noexcept {
            return this->getInfo();
        }

        [[nodiscard]] inline auto &operator*() const noexcept {
            return *Info;
        }

        [[nodiscard]] inline auto *operator->() noexcept {
            return this->Info.get();
        }

        [[nodiscard]] inline auto *operator->() const noexcept {
            return this->Info.get();
        }

        [[nodiscard]] inline auto operator==(const ExportTrieIteratorEnd &) {
            return this->isAtEnd();
        }

        [[nodiscard]] inline auto operator!=(const ExportTrieIteratorEnd &End) {
            return !(*this == End);
        }
    };

    struct ConstExportTrieList {
    public:
        using ParseOptions = ExportTrieParseOptions;
    protected:
        const uint8_t *Begin;
        const uint8_t *End;
    public:
        explicit
        ConstExportTrieList(const uint8_t *const Begin,
                            const uint8_t *const End) noexcept
        : Begin(Begin), End(End) {}

        using IteratorType = ExportTrieIterator;

        [[nodiscard]] inline auto begin() const noexcept {
            return IteratorType(this->Begin, this->End);
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return IteratorType(this->Begin, this->End);
        }

        [[nodiscard]]
        inline auto begin(const ParseOptions &Options) const noexcept {
            return IteratorType(Begin, End, Options);
        }

        [[nodiscard]]
        inline auto cbegin(const ParseOptions &Options) const noexcept {
            return IteratorType(Begin, End, Options);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return ExportTrieIteratorEnd();
        }

        [[nodiscard]] inline auto cend() const noexcept {
            return ExportTrieIteratorEnd();
        }
    };

    struct ExportTrieList : ConstExportTrieList {
    public:
        explicit
        ExportTrieList(uint8_t *const Begin, uint8_t *const End) noexcept
        : ConstExportTrieList(Begin, End) {}

        using IteratorType = ExportTrieIterator;

        [[nodiscard]] inline auto begin() const noexcept {
            return IteratorType(this->Begin, this->End);
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return IteratorType(this->Begin, this->End);
        }

        [[nodiscard]]
        inline auto begin(const ParseOptions &Options) const noexcept {
            return IteratorType(this->Begin, this->End, Options);
        }

        [[nodiscard]]
        inline auto cbegin(const ParseOptions &Options) const noexcept {
            return IteratorType(this->Begin, this->End, Options);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return ExportTrieIteratorEnd();
        }

        [[nodiscard]] inline auto cend() const noexcept {
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

        [[nodiscard]] inline auto &getInfo() noexcept {
            return this->Iterator.getInfo();
        }

        [[nodiscard]] inline auto &getInfo() const noexcept {
            return this->Iterator.getInfo();
        }

        [[nodiscard]] inline auto isAtEnd() const noexcept {
            return this->Iterator.isAtEnd();
        }

        [[nodiscard]] inline auto hasError() const noexcept {
            return this->Iterator.hasError();
        }

        [[nodiscard]] inline auto getError() const noexcept {
            return this->Iterator.getError();
        }

        inline auto operator++() noexcept -> decltype(*this) {
            this->Advance();
            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            return ++(*this);
        }

        inline auto operator+=(uint64_t Amt) noexcept -> decltype(*this) {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] inline auto &operator*() noexcept {
            return this->Iterator.getInfo();
        }

        [[nodiscard]] inline auto &operator*() const noexcept {
            return this->Iterator.getInfo();
        }

        [[nodiscard]] inline auto *operator->() noexcept {
            return &this->Iterator.getInfo();
        }

        [[nodiscard]] inline auto *operator->() const noexcept {
            return &this->Iterator.getInfo();
        }

        [[nodiscard]] inline auto operator==(const ExportTrieIteratorEnd &End) {
            return this->Iterator == End;
        }

        [[nodiscard]] inline auto operator!=(const ExportTrieIteratorEnd &End) {
            return !(*this == End);
        }
    };

    struct ConstExportTrieExportList {
    protected:
        const uint8_t *Begin;
        const uint8_t *End;
    public:
        constexpr explicit
        ConstExportTrieExportList(const uint8_t *const Begin,
                                  const uint8_t *const End) noexcept
        : Begin(Begin), End(End) {}

        [[nodiscard]] inline auto begin() const noexcept {
            return ExportTrieExportIterator(this->Begin, this->End);
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return ExportTrieIteratorEnd();
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return static_cast<uint64_t>(End - Begin);
        }
    };

    struct ExportTrieExportList : ConstExportTrieExportList {
    public:
        explicit
        ExportTrieExportList(uint8_t *const Begin,
                             uint8_t *const End) noexcept
        : ConstExportTrieExportList(Begin, End) {}
    };

    auto
    GetExportTrieList(const MemoryMap &Map,
                      uint32_t ExportOff,
                      uint32_t ExportSize) noexcept
        -> ExpectedAlloc<ExportTrieList, SizeRangeError>;

    auto
    GetConstExportTrieList(const ConstMemoryMap &Map,
                           uint32_t ExportOff,
                           uint32_t ExportSize) noexcept
        -> ExpectedAlloc<ConstExportTrieList, SizeRangeError>;

    auto
    GetExportTrieExportList(const MemoryMap &Map,
                            uint32_t ExportOff,
                            uint32_t ExportSize) noexcept
        -> ExpectedAlloc<ExportTrieExportList, SizeRangeError>;

    auto
    GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                 uint32_t ExportOff,
                                 uint32_t ExportSize) noexcept
        -> ExpectedAlloc<ConstExportTrieExportList, SizeRangeError>;

    auto
    GetExportListFromExportTrie(
        const ConstMemoryMap &Map,
        uint32_t ExportOff,
        uint32_t ExportSize,
        std::vector<ExportTrieExportInfo> &ExportListOut) noexcept
            -> SizeRangeError;
}
