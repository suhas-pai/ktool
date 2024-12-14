//
//  ADT/OutputFile.h
//  ktool
//
//  Created by Suhas Pai on 4/29/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include "FileDescriptor.h"

struct OutputFile {
protected:
    char *TmpPath;
    FileDescriptor Tmp;
public:
    explicit OutputFile() noexcept;
    ~OutputFile() noexcept;

    [[nodiscard]] inline bool isOpen() const noexcept { return Tmp.isOpen(); }
    [[nodiscard]] inline bool isEmpty() const noexcept { return Tmp.isEmpty(); }
    [[nodiscard]] inline bool hasError() const noexcept {
        return Tmp.hasError();
    }

    bool Read(void *Buf, size_t Size) const noexcept;
    bool Write(const void *Buf, size_t Size) noexcept;
    bool TruncateToSize(uint64_t Length) noexcept;

    bool Finalize(const char *Path) noexcept;
    inline void Close() noexcept { Tmp.Close(); }
};
