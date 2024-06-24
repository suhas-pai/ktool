/*
 * MachO/ObjcInfo.h
 * © suhas pai
 */

#pragma once

#include <algorithm>
#include <string_view>
#include <vector>

#include "ADT/DeVirtualizer.h"
#include "ADT/MemoryMap.h"
#include "ADT/Tree.h"

#include "MachO/BindInfo.h"
#include "MachO/SegmentList.h"

#include "ObjC/Info.h"

namespace MachO {
    struct ObjcClassCategoryInfo;
    struct ObjcClassInfo : public ADT::TreeNode {
    public:
        using CategoryListType = std::vector<ObjcClassCategoryInfo *>;
    protected:
        union {
            uint64_t Addr = 0;
            uint64_t BindAddr;
        };

        std::string Name;
        uint64_t DylibOrdinal = 0;

        bool sIsExternal : 1 = false;
        bool sIsNull : 1 = false;
        bool sIsSwift : 1 = false;

        ObjC::ClassRoFlags Flags;
        CategoryListType CategoryList;
    public:
        explicit ObjcClassInfo() noexcept {}
        explicit ObjcClassInfo(const std::string_view Name) noexcept
        : Name(Name) {}

        [[nodiscard]] inline TreeNode *createNew() const noexcept override {
            return new ObjcClassInfo;
        }

        [[nodiscard]] static inline auto &get(TreeNode &Node) noexcept {
            return reinterpret_cast<ObjcClassInfo &>(Node);
        }

        [[nodiscard]]
        static inline const auto &get(const TreeNode &Node) noexcept {
            return reinterpret_cast<const ObjcClassInfo &>(Node);
        }

        [[nodiscard]] static inline auto get(TreeNode *const Node) noexcept {
            return reinterpret_cast<ObjcClassInfo *>(Node);
        }

        [[nodiscard]] inline auto parent() const noexcept {
            return get(Parent);
        }

        [[nodiscard]] inline auto prevSibling() const noexcept {
            return get(PrevSibling);
        }

        [[nodiscard]] inline auto nextSibling() const noexcept {
            return get(NextSibling);
        }

        [[nodiscard]] inline auto firstChild() const noexcept {
            return get(FirstChild);
        }

        [[nodiscard]] inline auto lastChild() const noexcept {
            return get(LastChild);
        }

        [[nodiscard]]
        static inline auto get(const TreeNode *const Node) noexcept {
            return reinterpret_cast<const ObjcClassInfo *>(Node);
        }

        [[nodiscard]] inline uint64_t getLength() const noexcept override {
            return Name.length();
        }

        [[nodiscard]] inline auto super() const noexcept {
            return reinterpret_cast<ObjcClassInfo *>(Parent);
        }

        inline auto setSuper(ObjcClassInfo *const Parent) noexcept
            -> decltype(*this)
        {
            this->Parent = reinterpret_cast<TreeNode *>(Parent);
            return *this;
        }

        inline auto
        setLastChild(ObjcClassInfo *const LastChild) noexcept -> decltype(*this)
        {
            this->LastChild = LastChild;
            return *this;
        }

        [[nodiscard]] constexpr auto name() const noexcept -> std::string_view {
            return Name;
        }

        [[nodiscard]] constexpr auto dylibOrdinal() const noexcept {
            return DylibOrdinal;
        }

        [[nodiscard]] constexpr auto external() const noexcept {
            return sIsExternal;
        }

        [[nodiscard]] constexpr auto null() const noexcept {
            return sIsNull;
        }

        [[nodiscard]] constexpr auto isSwift() const noexcept {
            return sIsSwift;
        }

        [[nodiscard]] constexpr auto flags() const noexcept {
            return Flags;
        }

        [[nodiscard]] constexpr auto address() const noexcept {
            assert(!this->external());
            return Addr;
        }

        [[nodiscard]] constexpr auto bindAddress() const noexcept {
            assert(this->external());
            return BindAddr;
        }

        [[nodiscard]] constexpr auto &categoryList() const noexcept {
            return CategoryList;
        }

        [[nodiscard]] constexpr auto &categoryListRef() noexcept {
            return CategoryList;
        }

        inline auto setName(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->Name = Value;
            return *this;
        }

        inline auto setDylibOrdinal(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->DylibOrdinal = Value;
            return *this;
        }

        inline auto setIsExternal(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->sIsExternal = Value;
            return *this;
        }

        constexpr auto setIsNull(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->sIsNull = Value;
            return *this;
        }

        constexpr auto setIsSwift(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->sIsSwift = Value;
            return *this;
        }

        constexpr auto setFlags(const ObjC::ClassRoFlags &Value) noexcept
            -> decltype(*this)
        {
            this->Flags = Value;
            return *this;
        }

        constexpr auto setAddr(const uint64_t Value) noexcept -> decltype(*this)
        {
            assert(!this->external());

            this->Addr = Value;
            return *this;
        }

        constexpr auto setBindAddr(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            assert(this->external());

            this->BindAddr = Value;
            return *this;
        }

        [[nodiscard]]
        constexpr auto operator==(const ObjcClassInfo &Rhs) const noexcept {
            return Addr == Rhs.Addr;
        }

        [[nodiscard]]
        constexpr auto operator!=(const ObjcClassInfo &Rhs) const noexcept {
            return !operator==(Rhs);
        }
    };

    struct ObjcClassCategoryInfo {
    protected:
        std::string Name;
        const ObjcClassInfo *Class = nullptr;
        uint64_t Address = 0;
        bool sIsNull : 1 = false;
    public:
        explicit ObjcClassCategoryInfo() {}
        [[nodiscard]] constexpr auto name() const noexcept -> std::string_view {
            return Name;
        }

        [[nodiscard]] constexpr auto getClass() const noexcept {
            return Class;
        }

        [[nodiscard]] constexpr auto address() const noexcept {
            return Address;
        }

        [[nodiscard]] constexpr auto null() const noexcept {
            return sIsNull;
        }

        constexpr auto setName(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->Name = Value;
            return *this;
        }

        constexpr auto setClass(const ObjcClassInfo *const Value) noexcept
            -> decltype(*this)
        {
            this->Class = Value;
            return *this;
        }

        constexpr auto setAddress(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->Address = Value;
            return *this;
        }

        constexpr auto setIsNull(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->sIsNull = Value;
            return *this;
        }
    };

    namespace ObjcParse {
        enum class Error {
            None,
            NoObjcData,
            UnalignedSection,
            DataOutOfBounds
        };

        template <bool Is64Bit>
        using ObjcClassType =
            std::conditional_t<Is64Bit, ObjC::Class64, ObjC::Class>;

        template <bool Is64Bit>
        using ObjcClassRoType =
            std::conditional_t<Is64Bit, ObjC::ClassRo64, ObjC::ClassRo>;

        template <bool Is64Bit>
        using ObjcClassCategoryType =
            std::conditional_t<Is64Bit,
                            ObjC::ClassCategory64,
                            ObjC::ClassCategory>;

        using ObjcClassCollectionType =
            std::unordered_map<uint64_t, std::unique_ptr<ObjcClassInfo>>;

        inline auto
        ClassCollectionTypeAddClass(ObjcClassCollectionType &List,
                                    ObjcClassInfo &&Class,
                                    const uint64_t ClassAddr) noexcept
        {
            return List.insert({
                ClassAddr, std::make_unique<ObjcClassInfo>(Class)
            }).first->second.get();
        }

        inline void
        SetSuperClassForClassInfo(ObjcClassInfo *const Super,
                                  ObjcClassInfo *const Info) noexcept
        {
            if (Super != Info) {
                Super->addChild(*Info);
            } else {
                Info->setSuper(nullptr);
            }
        }

        inline auto
        GetNameFromBindActionSymbol(const std::string_view Symbol) noexcept {
            constexpr auto Prefix = std::string_view("_OBJC_CLASS_$_");
            if (Symbol.compare(0, Prefix.size(), Prefix) == 0) {
                return std::string_view(Symbol.data() + Prefix.length());
            }

            return Symbol;
        }

        inline void
        SetSuperWithBindAction(
            ObjcClassInfo *const Info,
            const BindActionList::Info &Action,
            const SegmentList &SegmentList,
            ObjcClassCollectionType &List,
            std::vector<ObjcClassInfo *> &ExternalAndRootClassList) noexcept
        {
            const auto ActionSymbol =
                GetNameFromBindActionSymbol(Action.SymbolName);

            auto Address = uint64_t();
            if (const auto Segment =
                    SegmentList.atOrNull(
                        static_cast<uint64_t>(Action.SegmentIndex)))
            {
                Address = Segment->VmRange.locForIndex(Action.AddrInSeg);
            }

            const auto Pred = [&](const auto &Lhs) noexcept {
                if (Action.DylibOrdinal <= 0) {
                    return false;
                }

                const auto DylibOrdinal =
                    static_cast<uint64_t>(Action.DylibOrdinal);

                if (Lhs->dylibOrdinal() != DylibOrdinal) {
                    return false;
                }

                return Lhs->name() == ActionSymbol;
            };

            const auto Begin = ExternalAndRootClassList.cbegin();
            const auto End = ExternalAndRootClassList.cend();
            const auto Iter = std::find_if(Begin, End, Pred);

            if (Iter != End) {
                SetSuperClassForClassInfo(*Iter, Info);
                return;
            }

            auto SuperInfo = ObjcClassInfo(ActionSymbol);

            SuperInfo.setIsExternal();
            SuperInfo.setBindAddr(Address);
            SuperInfo.setDylibOrdinal(
                Action.DylibOrdinal >= 0 ?
                    static_cast<uint64_t>(Action.DylibOrdinal) : 0);

            const auto Ptr =
                ClassCollectionTypeAddClass(List,
                                            std::move(SuperInfo),
                                            Address);

            SetSuperClassForClassInfo(Ptr, Info);
            ExternalAndRootClassList.emplace_back(Ptr);
        }

        template <bool Is64Bit>
        static void
        ParseObjcClass(const uint64_t Addr,
                       const ADT::DeVirtualizer &DeVirtualizer,
                       ObjcClassInfo &Info,
                       const bool IsBigEndian) noexcept
        {
            using ClassType = ObjcClassType<Is64Bit>;
            using ClassRoType = ObjcClassRoType<Is64Bit>;

            const auto Class =
                reinterpret_cast<const ClassType *>(
                    DeVirtualizer.getPtrForAddress(Addr));

            if (Class == nullptr) {
                Info.setAddr(Addr);
                Info.setIsNull();

                return;
            }

            const auto SuperAddr = Class->superClassAddress(IsBigEndian);
            const auto RoAddr = Class->data(IsBigEndian);
            const auto ClassRo =
                reinterpret_cast<const ClassRoType *>(
                    DeVirtualizer.getPtrForAddress(RoAddr));

            if (ClassRo == nullptr) {
                Info.setAddr(Addr);
                Info.setIsNull();

                return;
            }

            const auto NameAddr = ClassRo->nameAddress(IsBigEndian);
            if (auto String = DeVirtualizer.getStringAtAddress(NameAddr)) {
                Info.setName(String.value());
            }

            Info.setAddr(Addr);
            Info.setFlags(ClassRo->flags(IsBigEndian));
            Info.setIsSwift(Class->isSwift(IsBigEndian));
            Info.setSuper(reinterpret_cast<ObjcClassInfo *>(SuperAddr | 1));
        }

        inline bool
        SetSuperIfExists(ObjcClassCollectionType &List,
                        ObjcClassInfo *const Info) noexcept
        {
            const auto Addr =
                reinterpret_cast<uint64_t>(Info->super()) &
                static_cast<uint64_t>(~1);

            if (const auto Iter = List.find(Addr); Iter != List.end()) {
                SetSuperClassForClassInfo(Iter->second.get(), Info);
                return true;
            }

            return false;
        }

        template <bool Is64Bit>
        static void
        FixSuperForClassInfo(
            ObjcClassInfo *const Info,
            const ADT::DeVirtualizer &DeVirtualizer,
            const SegmentList &SegmentList,
            const BindActionList::UnorderedMap &BindCollection,
            ObjcClassCollectionType &List,
            std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
            const bool IsBigEndian) noexcept
        {
            // BindAddr points to the `SuperClass` field inside ObjcClass[64]
            const auto BindAddr =
                Info->address() + offsetof(ObjcClassType<Is64Bit>, SuperClass);

            if (auto Iter = BindCollection.find(BindAddr);
                Iter != BindCollection.end())
            {
                SetSuperWithBindAction(Info,
                                       Iter->second,
                                       SegmentList,
                                       List,
                                       ExternalAndRootClassList);
                return;
            }

            const auto SuperAddr =
                reinterpret_cast<uint64_t>(Info->super()) & uint64_t(~1);

            if (SuperAddr == 0) {
                Info->setSuper(nullptr);
                ExternalAndRootClassList.emplace_back(Info);

                return;
            }

            if (SetSuperIfExists(List, Info)) {
                return;
            }

            auto SuperInfo = ObjcClassInfo();
            ParseObjcClass<Is64Bit>(SuperAddr,
                                    DeVirtualizer,
                                    SuperInfo,
                                    IsBigEndian);

            const auto Ptr =
                ClassCollectionTypeAddClass(List,
                                            std::move(SuperInfo),
                                            SuperAddr);

            SetSuperClassForClassInfo(Ptr, Info);
            FixSuperForClassInfo<Is64Bit>(Ptr,
                                          DeVirtualizer,
                                          SegmentList,
                                          BindCollection,
                                          List,
                                          ExternalAndRootClassList,
                                          IsBigEndian);
        }

        template <bool Is64Bit>
        static void
        HandleAddrForObjcClass(const uint64_t Addr,
                               const ADT::DeVirtualizer &DeVirtualizer,
                               ObjcClassCollectionType &List,
                               const bool IsBigEndian) noexcept
        {
            if (Addr == 0) {
                return;
            }

            const auto SwitchedAddr = ADT::SwitchEndianIf(Addr, IsBigEndian);

            auto Info = ObjcClassInfo();
            ParseObjcClass<Is64Bit>(SwitchedAddr,
                                    DeVirtualizer,
                                    Info,
                                    IsBigEndian);

            ClassCollectionTypeAddClass(List, std::move(Info), SwitchedAddr);
        }

        template <bool Is64Bit>
        static void
        FixSuperClassForClassList(
            const ADT::DeVirtualizer &DeVirtualizer,
            const SegmentList &SegmentList,
            const BindActionList::UnorderedMap &BindCollection,
            ObjcClassCollectionType &List,
            std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
            const bool IsBigEndian) noexcept
        {
            for (auto &Iter : List) {
                const auto Info = Iter.second.get();
                const auto SuperAddr =
                    reinterpret_cast<uint64_t>(Info->super());

                if ((SuperAddr & 1) == 0) {
                    continue;
                }

                if (Info->external()) {
                    continue;
                }

                FixSuperForClassInfo<Is64Bit>(Info,
                                              DeVirtualizer,
                                              SegmentList,
                                              BindCollection,
                                              List,
                                              ExternalAndRootClassList,
                                              IsBigEndian);
            }
        }

        template <bool Is64Bit>
        static auto
        ParseObjcClassListSection(
            const SectionInfo &SectionInfo,
            const ADT::DeVirtualizer &DeVirtualizer,
            const SegmentList &SegmentList,
            const BindActionList::UnorderedMap &BindList,
            ObjcClassCollectionType &ClassList,
            std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
            const bool IsBigEndian) noexcept
        {
            using PtrAddrType = Utils::PointerAddrConstType<Is64Bit>;
            const auto SectionMapOpt =
                DeVirtualizer.getMapForRange(SectionInfo.vmRange());

            if (!SectionMapOpt.has_value()) {
                return Error::DataOutOfBounds;
            }

            const auto SectionMap = SectionMapOpt.value();
            if (!Utils::IntegerIsPointerAligned<Is64Bit>(SectionMap.size())) {
                return Error::UnalignedSection;
            }

            const auto ListOpt = SectionMap.list<PtrAddrType>();
            if (!ListOpt.has_value()) {
                return Error::DataOutOfBounds;
            }

            for (const auto &Addr : ListOpt.value()) {
                HandleAddrForObjcClass<Is64Bit>(Addr,
                                                DeVirtualizer,
                                                ClassList,
                                                IsBigEndian);
            }

            FixSuperClassForClassList<Is64Bit>(DeVirtualizer,
                                               SegmentList,
                                               BindList,
                                               ClassList,
                                               ExternalAndRootClassList,
                                               IsBigEndian);
            return Error::None;
        }

        [[nodiscard]] inline auto
        CreateExternalClass(const std::string_view Name,
                            const uint64_t DylibOrdinal,
                            const uint64_t BindAddr) noexcept
        {
            auto NewInfo = ObjcClassInfo();

            NewInfo.setName(Name);
            NewInfo.setDylibOrdinal(DylibOrdinal);

            NewInfo.setIsExternal();
            NewInfo.setBindAddr(BindAddr);

            return NewInfo;
        }

        template <bool Is64Bit>
        static auto
        ParseObjcClassRefsSection(
            const SectionInfo &SectionInfo,
            const ADT::DeVirtualizer &DeVirtualizer,
            const SegmentList &SegmentList,
            const BindActionList::UnorderedMap &BindCollection,
            ObjcClassCollectionType &ClassList,
            std::vector<ObjcClassInfo *> &ExternalAndRootClassList,
            const bool IsBigEndian) noexcept
        {
            using PointerAddrType = Utils::PointerAddrConstType<Is64Bit>;
            const auto SectionMapOpt =
                DeVirtualizer.getMapForRange(SectionInfo.vmRange());

            if (!SectionMapOpt.has_value()) {
                return Error::DataOutOfBounds;
            }

            const auto SectionMap = SectionMapOpt.value();
            if (!Utils::IntegerIsPointerAligned<Is64Bit>(SectionMap.size())) {
                return Error::UnalignedSection;
            }

            const auto ListOpt = SectionMap.list<PointerAddrType>();
            if (!ListOpt.has_value()) {
                return Error::DataOutOfBounds;
            }

            auto ListAddr = SectionInfo.vmRange().begin();
            for (const auto &Addr : ListOpt.value()) {
                if (const auto Iter = BindCollection.find(ListAddr);
                    Iter != BindCollection.end())
                {
                    const auto &It = Iter->second;
                    const auto Name =
                        GetNameFromBindActionSymbol(It.SymbolName);
                    const auto DylibOrdinal =
                        static_cast<uint64_t>(It.DylibOrdinal);

                    auto NewInfo =
                        CreateExternalClass(Name, DylibOrdinal, ListAddr);

                    const auto Ptr =
                        ClassCollectionTypeAddClass(ClassList,
                                                    std::move(NewInfo),
                                                    ListAddr);

                    ExternalAndRootClassList.emplace_back(Ptr);
                } else {
                    HandleAddrForObjcClass<Is64Bit>(Addr,
                                                    DeVirtualizer,
                                                    ClassList,
                                                    IsBigEndian);
                }

                ListAddr += Utils::PointerSize<Is64Bit>();
            }

            FixSuperClassForClassList<Is64Bit>(DeVirtualizer,
                                               SegmentList,
                                               BindCollection,
                                               ClassList,
                                               ExternalAndRootClassList,
                                               IsBigEndian);
            return Error::None;
        }

        static const auto ObjcClassRefsSegmentSectionNamePairList =
            std::initializer_list<SegmentList::SegmentSectionNameListPair>{
                SegmentList::SegmentSectionNameListPair {
                    "__OBJC2", { "__class_refs" }
                },
                SegmentList::SegmentSectionNameListPair {
                    "__DATA", { "__objc_classrefs" }
                }
            };

        static const auto ObjcClassListSegmentSectionNamePairList =
            std::initializer_list<SegmentList::SegmentSectionNameListPair>{
                SegmentList::SegmentSectionNameListPair{
                    "__OBJC2",  { "__class_list"     }
                },
                SegmentList::SegmentSectionNameListPair{
                    "__DATA_CONST", { "__objc_classlist" }
                },
                SegmentList::SegmentSectionNameListPair{
                    "__DATA", { "__objc_classlist" }
                },
                SegmentList::SegmentSectionNameListPair{
                    "__DATA_DIRTY", { "__objc_classlist" }
                },
            };

        static const auto ObjcClassCategoryListSegmentSectionNamePairList =
            std::initializer_list<SegmentList::SegmentSectionNameListPair>{
                SegmentList::SegmentSectionNameListPair{
                    "__DATA_CONST", { "__objc_catlist" }
                },
                SegmentList::SegmentSectionNameListPair{
                    "__DATA", { "__objc_catlist" }
                },
            };
    }

    struct ObjcClassInfoList : public ADT::Tree {
        friend struct ObjcClassCategoryInfoList;
    public:
        using Info = ObjcClassInfo;
    protected:
        std::unordered_map<uint64_t, std::unique_ptr<Info>> List;
        void AdjustExternalAndRootClassList(std::vector<Info *> &List) noexcept;

        Info *Root;
    public:
        ObjcClassInfoList() noexcept = default;

        auto
        Parse(const ADT::DeVirtualizer &DeVirtualizer,
              const SegmentList &SegmentList,
              const BindActionList::UnorderedMap &BindList,
              bool IsBigEndian,
              bool Is64Bit) noexcept -> ObjcParse::Error;

        [[nodiscard]] inline ADT::TreeNode *root() const noexcept {
            return Root;
        }

        inline ADT::Tree &setRoot(ADT::TreeNode *const Root) noexcept {
            this->Root = static_cast<Info *>(Root);
            return *this;
        }

        [[nodiscard]] auto getAsList() const noexcept -> std::vector<Info *>;

        auto addNullClass(uint64_t BindAddress) noexcept -> ObjcClassInfo *;

        auto
        addExternalClass(std::string_view Name,
                         uint64_t DylibOrdinal,
                         uint64_t BindAddress) noexcept -> ObjcClassInfo *;

        [[nodiscard]]
        auto getInfoForAddress(uint64_t Address) const noexcept
            -> ObjcClassInfo *;

        [[nodiscard]]
        auto getInfoForClassName(std::string_view Name) const noexcept
            -> ObjcClassInfo *;

        [[nodiscard]] inline auto &list() const noexcept {
            return List;
        }

        [[nodiscard]] inline auto &list() noexcept {
            return List;
        }
    };

    struct ObjcClassCategoryInfoList {
    public:
        using Info = ObjcClassCategoryInfo;
    protected:
        std::vector<std::unique_ptr<Info>> List;
    public:
        ObjcClassCategoryInfoList() noexcept = default;

        auto
        CollectFrom(const ADT::MemoryMap &Map,
                    const ADT::DeVirtualizer &DeVirtualizer,
                    const SegmentList &SegmentList,
                    const BindActionList::UnorderedMap &BindCollection,
                    ObjcClassInfoList *ClassInfoTree,
                    bool IsBigEndian,
                    bool Is64Bit) noexcept -> ObjcParse::Error;
    };
}