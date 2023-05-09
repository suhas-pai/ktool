/*
 * MachO/OpcodeList.h
 * © suhas pai
 */

#pragma once

#include <optional>
#include <string_view>

#include "Utils/Leb128.h"
#include "Utils/Overflow.h"

namespace MachO {
    struct OpcodeList {
    protected:
        const uint8_t *Begin;
        const uint8_t *End;
    public:
        explicit
        OpcodeList(const uint8_t *const Begin,
                   const uint8_t *const End) noexcept
        : Begin(Begin), End(End) {}

        explicit
        OpcodeList(const uint8_t *const Begin, const uint64_t Size) noexcept
        : Begin(Begin),
          End(Utils::AddPtrAndCheckOverflow(this->Begin, Size).value()) {}

        struct EndIterator {};

        struct Iterator;
        struct IteratorInfo {
            friend Iterator;
        protected:
            const uint8_t *Iter;
            const uint8_t *End;
        public:
            explicit
            IteratorInfo(const uint8_t *const Iter,
                         const uint8_t *const End) noexcept
            : Iter(Iter), End(End) {}

            [[nodiscard]] constexpr auto ptr() const noexcept {
                return Iter;
            }

            template <typename T>
            [[nodiscard]] inline auto asByte() const noexcept {
                return reinterpret_cast<const T *>(Iter);
            }

            template <typename T = uint64_t>
            [[nodiscard]] auto ReadUleb128() noexcept -> std::optional<T> {
                return Utils::ReadUleb128(Iter, End, &Iter);
            }

            template <typename T = int64_t>
            [[nodiscard]] auto ReadSleb128() noexcept -> std::optional<T> {
                return Utils::ReadSleb128(Iter, End, &Iter);
            }

            [[nodiscard]]
            auto ParseString() noexcept -> std::optional<std::string_view>;

            [[nodiscard]] constexpr auto isAtEnd() const noexcept {
                return Iter >= End;
            }
        };

        struct Iterator {
        protected:
            IteratorInfo Info;
        public:
            explicit
            Iterator(const uint8_t *const Iter,
                     const uint8_t *const End) noexcept
            : Info(Iter, End) {}

            [[nodiscard]] constexpr auto ptr() const noexcept {
                return Info.ptr();
            }

            [[nodiscard]] constexpr auto &operator*() noexcept {
                return Info;
            }

            [[nodiscard]] constexpr auto operator->() noexcept {
                return &Info;
            }

            [[nodiscard]] constexpr auto &operator*() const noexcept {
                return Info;
            }

            [[nodiscard]] constexpr auto operator->() const noexcept {
                return &Info;
            }

            [[nodiscard]] constexpr auto isAtEnd() const noexcept {
                return Info.isAtEnd();
            }

            [[nodiscard]]
            inline auto operator==(const EndIterator &) const noexcept {
                return isAtEnd();
            }

            [[nodiscard]]
            inline auto operator!=(const EndIterator &End) const noexcept {
                return !operator==(End);
            }

            constexpr auto operator++() noexcept -> decltype(*this) {
                Info.Iter++;
                return *this;
            }

            inline auto operator++(int) noexcept -> decltype(*this) {
                return operator++();
            }

            inline auto operator+=(const uint64_t Amt) noexcept
                -> decltype(*this)
            {
                for (auto I = uint64_t(); I != Amt; I++) {
                    operator++();
                }

                return *this;
            }
        };

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(Begin, End);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return EndIterator();
        }
    };
}