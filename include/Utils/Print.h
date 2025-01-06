//
//  Utils/Print.h
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#pragma once

#include <format>
#include "MachO/LoadCommands.h"

#include "Utils/Misc.h"
#include "Utils/Templates.h"

namespace Utils {
    template <std::unsigned_integral T>
    [[nodiscard]]
    constexpr auto GetIntegerDigitCount(T Integer) noexcept -> uint8_t {
        auto DigitLength = uint8_t();
        do {
            DigitLength++;
        } while (Integer /= 10);

        return DigitLength;
    }

    template <std::signed_integral T>
    [[nodiscard]]
    constexpr auto GetIntegerDigitCount(const T Integer) noexcept -> uint8_t {
        if (Integer < 0) {
            return GetIntegerDigitCount(-Integer) + 1;
        }

        return GetIntegerDigitCount(
            static_cast<std::make_unsigned_t<T>>(Integer));
    }

    auto PadSpaces(FILE *OutFile, uint32_t SpaceAmount) noexcept -> int;
    auto RightPadSpaces(FILE *OutFile, int WrittenOut, int RightPad) -> int;

    auto
    PrintMultTimes(FILE *OutFile,
                   std::string_view String,
                   uint64_t Times,
                   std::string_view Prefix = "",
                   std::string_view Suffix = "") -> int;

    template <std::integral T>
    struct FormattedNumber {
        T Value;

        constexpr explicit FormattedNumber(const T Value) noexcept
        : Value(Value) {}
    };

    struct ByteSize {
        constexpr static std::array<std::string_view, 9> FormatSizeNames = {
            "KiB",
            "MiB",
            "GiB",
            "TiB",
            "PiB",
            "EiB",
            "ZiB",
            "YiB",
            "BiB"
        };

        uint64_t Value;
    };

    template <std::unsigned_integral T>
    struct Address {
        T Value;

        constexpr explicit Address(const T Value) noexcept : Value(Value) {}
    };

    struct CustomAddress {
        uint64_t Value;
        bool Is64Bit; // Else 32-bit
    };

    struct Uuid {
        std::array<uint8_t, 16> Bytes;

        constexpr explicit Uuid(const uint8_t Bytes[16]) noexcept {
            std::copy(Bytes, Bytes + 16, this->Bytes.begin());
        }

        constexpr explicit Uuid(std::array<uint8_t, 16> Bytes) noexcept
        : Bytes(Bytes) {}
    };

    template <std::unsigned_integral T = uint64_t,
              std::unsigned_integral U = T>
    struct PrintRange {
    protected:
        T Begin;
        U Size;
    public:
        explicit PrintRange(const T Begin, const U Size) noexcept
        : Begin(Begin), Size(Size) {}

        explicit PrintRange(const ADT::Range Range) noexcept
        : Begin(Range.front()), Size(Range.size()) {}

        [[nodiscard]] constexpr auto front() const noexcept {
            return this->Begin;
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return this->Size;
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return Utils::AddAndCheckOverflow<typename largest_type<T, U>::type>(
                this->front(), this->size());
        }

        [[nodiscard]] constexpr auto range() const noexcept {
            return ADT::Range::FromSize(this->Begin, this->Size);
        }
    };

    struct SegmentSectionPair {
        std::string_view SegmentName;
        std::string_view SectionName;

        bool PadSegment : 1;
        bool PadSection : 1;
    };

    struct Timestamp {
        time_t Value;
    };

    struct Boolean {
        bool Value;
    };

    auto
    PrintOffsetSizeInfo(FILE *OutFile,
                        const ADT::Range Range,
                        bool Is64Bit,
                        bool IsSize64Bit,
                        bool IsOutOfBounds,
                        std::string_view OffsetKey,
                        std::string_view SizeKey,
                        std::string_view Prefix = "",
                        std::string_view Suffix = "") noexcept -> int;

    auto
    PrintDylibOrdinalPath(FILE *OutFile,
                          uint8_t DylibOrdinal,
                          std::string_view DylibPath,
                          bool PrintPath,
                          bool IsOutOfBounds,
                          std::string_view Prefix = "",
                          std::string_view Suffix = "") noexcept -> int;

    auto
    PrintDylibOrdinalInfo(FILE *OutFile,
                          uint8_t DylibOrdinal,
                          std::string_view DylibPath,
                          bool PrintPath,
                          bool IsOutOfBounds,
                          std::string_view Prefix = "",
                          std::string_view Suffix = "") noexcept -> int;

    auto
    PrintOnlyOnce(FILE *OutFile,
                  std::string_view String,
                  bool &DidPrint) noexcept -> int;

    auto
    PrintOnlyAfterFirst(FILE *OutFile,
                        std::string_view String,
                        bool &DidPassFirst) noexcept -> int;

    // 32 for Segment+Section Name Max Length, 4 for apostrophes, 1 for comma
    constexpr static inline auto SegmentSectionPairMaxLen =
        uint32_t(32 + 4 + 1);

    // 2 for '0x', 8 for digits
    constexpr static inline auto Address32Length = uint32_t(2 + 8);

    // 2 for '0x', 16 for digits
    constexpr static inline auto Address64Length = uint32_t(2 + 16);

    constexpr static inline auto AddressLength(const bool Is64Bit) noexcept {
        return Is64Bit ? Address64Length : Address32Length;
    }

    template <std::unsigned_integral T>
    struct AddressLengthCalc {

    };

    template <>
    struct AddressLengthCalc<const uint32_t> {
        constexpr static inline auto Length = Address32Length;
    };

    template <>
    struct AddressLengthCalc<uint32_t> {
        constexpr static inline auto Length = Address32Length;
    };

    template <>
    struct AddressLengthCalc<const uint64_t> {
        constexpr static inline auto Length = Address64Length;
    };

    template <>
    struct AddressLengthCalc<uint64_t> {
        constexpr static inline auto Length = Address64Length;
    };
}

#define STR_LENGTH(s) (sizeof(s) - 1)

template <std::integral T>
struct std::formatter<Utils::FormattedNumber<T>> :
    public std::formatter<std::string_view>
{
    auto format(const auto &Number, auto &ctx) const noexcept {
        auto Buffer = std::array<char, 20>();
        std::to_chars(Buffer.begin(), Buffer.end(), Number.Value);

        auto Sv =
            std::string_view(Buffer.begin(),
                             strnlen(Buffer.begin(), Buffer.size()));

        const auto DigitCount =
            std::is_signed_v<T> && Number.Value < 0 ?
                Sv.length() - 1 : Sv.length();

        if (DigitCount < 4) {
            return std::formatter<std::string_view>::format(Sv, ctx);
        }

        const auto StartIndex = std::is_signed_v<T> && Number.Value < 0 ? 1 : 0;
        auto Result = std::string(Sv);

        for (auto I = int64_t(Sv.length() - 3); I > StartIndex; I -= 3) {
            Result.insert(static_cast<uint64_t>(I), 1, ',');
        }

        return std::formatter<std::string_view>::format(Result, ctx);
    }
};

template <>
struct std::formatter<Utils::Address<uint32_t>> {
    constexpr auto parse(auto &ctx) noexcept {
        return ctx.begin();
    }

    inline auto
    format(const Utils::Address<uint32_t> &Address, auto &ctx) const noexcept {
        return std::format_to(ctx.out(), "0x{:08x}", Address.Value);
    }
};

template <>
struct std::formatter<Utils::Address<uint64_t>> {
    constexpr auto parse(auto &ctx) noexcept {
        return ctx.begin();
    }

    inline auto
    format(const Utils::Address<uint64_t> &Address, auto &ctx) const noexcept {
        return std::format_to(ctx.out(), "0x{:016x}", Address.Value);
    }
};

template <>
struct std::formatter<Utils::CustomAddress> {
    constexpr auto parse(auto &ctx) noexcept {
        return ctx.begin();
    }

    auto
    format(const Utils::CustomAddress &Address, auto &ctx) const noexcept {
        if (Address.Is64Bit) {
            return std::format_to(ctx.out(),
                                  "{}",
                                  Utils::Address(Address.Value));
        }

        return std::format_to(ctx.out(),
                              "{}",
                              Utils::Address<uint32_t>(
                                static_cast<uint32_t>(Address.Value)));
    }
};

template<>
struct std::formatter<Utils::ByteSize> :
    public std::formatter<std::string_view>
{
    auto format(const Utils::ByteSize &ByteSize, auto &ctx) const {
        constexpr auto Base = 1024;
        auto Result = std::string();

        if (ByteSize.Value < Base) {
            std::format_to(std::back_inserter(Result),
                           "{} bytes",
                           Utils::FormattedNumber(ByteSize.Value));

            return std::formatter<std::string_view>::format(Result, ctx);
        }

        auto Index = uint32_t();
        auto ResultAmount = static_cast<double>(ByteSize.Value);

        ResultAmount /= Base;
        while (ResultAmount >= Base) {
            ResultAmount /= Base;
            Index++;
        };

        assert(Index < Utils::ByteSize::FormatSizeNames.size());

        const auto &Name = Utils::ByteSize::FormatSizeNames[Index];
        if (floor(ResultAmount) == ResultAmount) {
            std::format_to(std::back_inserter(Result),
                           "{} {}",
                           Utils::FormattedNumber(
                           static_cast<uint64_t>(ResultAmount)),
                           Name);

            return std::formatter<std::string_view>::format(Result, ctx);
        }

        std::format_to(std::back_inserter(Result),
                       "{:.3f} {}",
                       ResultAmount,
                       Name);

        return std::formatter<std::string_view>::format(Result, ctx);
    }
};

template<>
struct std::formatter<Utils::Uuid> : public std::formatter<std::string_view> {
    auto format(const Utils::Uuid &Uuid, auto &Ctx) const {
        auto Result = std::string();
        Result.reserve(35);

        std::format_to(std::back_inserter(Result),
                       "{:02X}{:02X}{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-"
                       "{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
                       Uuid.Bytes[0], Uuid.Bytes[1], Uuid.Bytes[2],
                       Uuid.Bytes[3], Uuid.Bytes[4], Uuid.Bytes[5],
                       Uuid.Bytes[6], Uuid.Bytes[7], Uuid.Bytes[8],
                       Uuid.Bytes[9], Uuid.Bytes[10], Uuid.Bytes[11],
                       Uuid.Bytes[12], Uuid.Bytes[13], Uuid.Bytes[14],
                       Uuid.Bytes[15]);

        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};

template <std::unsigned_integral T, std::unsigned_integral U>
struct std::formatter<Utils::PrintRange<T, U>> {
    constexpr auto parse(auto &ctx) noexcept {
        auto Iter = ctx.begin();
        for (; Iter != ctx.end() && *Iter != '}'; Iter++) {
            switch (*Iter) {
                case '>':
                    this->Padded = Padded::Left;
                    break;
                case '<':
                    this->Padded = Padded::Right;
                    break;
            }
        }

        return Iter;
    }

    auto
    format(const Utils::PrintRange<T, U> &Range, auto &ctx) const noexcept {
        constexpr auto FullRangeLength =
            Utils::AddressLengthCalc<T>::Length +
            Utils::AddressLengthCalc<U>::Length +
            STR_LENGTH("-");

        if (Range.front() == 0) {
            if (Range.size() == 0) {
                switch (this->Padded) {
                    case Padded::None:
                        return std::format_to(ctx.out(), "0x0-0x0");
                    case Padded::Left:
                        return std::format_to(ctx.out(),
                                              "{:>{}}",
                                              "0x0-0x0",
                                              FullRangeLength);
                    case Padded::Right:
                        return std::format_to(ctx.out(),
                                              "{:<{}}",
                                              "0x0-0x0",
                                              FullRangeLength);
                }

                VERIFY_NOT_REACHED();
            }

            switch (this->Padded) {
                case Padded::None:
                    return std::format_to(ctx.out(),
                                          "0x0-{}",
                                          Utils::Address(Range.size()));
                case Padded::Left:
                    return std::format_to(ctx.out(),
                                          "{:>{}}",
                                          std::format("0x0-{}",
                                                      Utils::Address(
                                                        Range.size())),
                                          FullRangeLength);
                case Padded::Right:
                    return std::format_to(ctx.out(),
                                          "{:<{}}",
                                          std::format("0x0-{}",
                                                      Utils::Address(
                                                        Range.size())),
                                          FullRangeLength);
            }

            VERIFY_NOT_REACHED();
        }

        if (const auto EndOpt = Range.end()) {
            const auto End = EndOpt.value();
            switch (this->Padded) {
                case Padded::None:
                    return std::format_to(ctx.out(),
                                          "{}-{}",
                                          Utils::Address(Range.front()),
                                          Utils::Address(End));
                case Padded::Left:
                    return std::format_to(ctx.out(),
                                          "{:>{}}",
                                          std::format("{}-{}",
                                                      Utils::Address(
                                                        Range.front()),
                                                      Utils::Address(End)),
                                          FullRangeLength);
                case Padded::Right:
                    return std::format_to(ctx.out(),
                                          "{:<{}}",
                                          std::format("{}-{}",
                                                      Utils::Address(
                                                        Range.front()),
                                                      Utils::Address(End)),
                                          FullRangeLength);
            }

            VERIFY_NOT_REACHED();
        }

        switch (this->Padded) {
            case Padded::None:
                return std::format_to(ctx.out(),
                                      "{}-0x0 (Overflows)",
                                      Utils::Address(Range.front()));
            case Padded::Left:
                return std::format_to(ctx.out(),
                                      "{:>{}}",
                                      std::format("{}-0x0 (Overflows)",
                                                  Utils::Address(
                                                    Range.front())),
                                        FullRangeLength);
            case Padded::Right:
                return std::format_to(ctx.out(),
                                      "{:<{}}",
                                      std::format("{}-0x0 (Overflows)",
                                                  Utils::Address(
                                                      Range.front())),
                                      FullRangeLength);
        }

        VERIFY_NOT_REACHED();
    }

    enum class Padded {
        None,
        Left,
        Right
    };

    Padded Padded = Padded::None;
};

template <>
struct std::formatter<Utils::SegmentSectionPair> :
    public std::formatter<std::string_view>
{
    auto
    format(const Utils::SegmentSectionPair &Pair, auto &Ctx) const noexcept {
        auto Result = std::string();
        if (Pair.PadSegment) {
            const auto PadLength =
                MachO::SegmentSectionMaxNameLength - Pair.SegmentName.length();

            std::format_to(std::back_inserter(Result),
                           "{:>{}}",
                           "",
                           PadLength);
        }

        std::format_to(std::back_inserter(Result),
                       "\"{}\",\"{}\"",
                       Pair.SegmentName,
                       Pair.SectionName);

        if (Pair.PadSection) {
            std::format_to(
                std::back_inserter(Result),
                "{:<{}}",
                "",
                MachO::SegmentSectionMaxNameLength - Pair.SectionName.length());
        }

        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};

template <>
struct std::formatter<Dyld3::PackedVersion> :
    public std::formatter<std::string_view>
{
    auto format(const Dyld3::PackedVersion &Version, auto &Ctx) const {
        const auto Result =
            std::format("{}.{}.{}",
                        Version.major(),
                        Version.minor(),
                        Version.revision());

        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};

template <>
struct std::formatter<Dyld3::PackedVersion64> :
    public std::formatter<std::string_view>
{
    auto format(const Dyld3::PackedVersion64 &Version, auto &Ctx) const {
        const auto Result =
            std::format("{}.{}.{}.{}.{}",
                        Version.major(),
                        Version.minor(),
                        Version.revision1(),
                        Version.revision2(),
                        Version.revision3());

        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};

template <>
struct std::formatter<ADT::Range> {
    constexpr auto parse(auto &ctx) {
        return ctx.begin();
    }

    auto format(const ADT::Range Range, auto &ctx) const {
        if (Range.front() == 0) {
            return std::format_to(ctx.out(), "0x0-0x{:08x}", Range.size());
        }

        if (const auto End = Range.end()) {
            return std::format_to(ctx.out(),
                                  "0x{:08x}-0x{:08x}",
                                  Range.front(),
                                  End.value());
        }

        return std::format_to(ctx.out(),
                              "0x{:08x}-0x0 (Overflows)",
                              Range.front(),
                              Range.size());
    }
};

template <>
struct std::formatter<Mach::VmProt> : public std::formatter<std::string_view> {
    auto format(const Mach::VmProt &Prot, auto &Ctx) const noexcept {
        const auto Result =
            std::format("{}{}{}",
                        Prot.readable() ? 'R' : '-',
                        Prot.writable() ? 'W' : '-',
                        Prot.executable() ? 'X' : '-');

        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};

template <>
struct std::formatter<Mach::VmProtInitMax> :
    public std::formatter<std::string_view>
{
    auto format(const Mach::VmProtInitMax &Prot, auto &Ctx) const noexcept {
        const auto Result = std::format("{}/{}", Prot.getInit(), Prot.getMax());
        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};

template <>
struct std::formatter<Utils::Timestamp> :
    public std::formatter<std::string_view>
{
    auto format(const Utils::Timestamp &Timestamp, auto &Ctx) const noexcept {
        const auto Result = Utils::GetHumanReadableTimestamp(Timestamp.Value);
        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};

template <>
struct std::formatter<Utils::Boolean> :
    public std::formatter<std::string_view>
{
    auto format(const Utils::Boolean &Bool, auto &Ctx) const noexcept {
        const auto Result = Bool.Value ? "Yes" : "No";
        return std::formatter<std::string_view>::format(Result, Ctx);
    }
};
