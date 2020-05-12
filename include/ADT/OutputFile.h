//
//  include/ADT/OutputFile.h
//  stool
//
//  Created by Suhas Pai on 4/29/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
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

    inline bool IsOpen() const noexcept { return Tmp.IsOpen(); }
    inline bool IsEmpty() const noexcept { return Tmp.IsEmpty(); }
    inline bool HasError() const noexcept { return Tmp.HasError(); }

    bool Read(void *Buf, size_t Size) const noexcept;
    bool Write(const void *Buf, size_t Size) noexcept;
    bool TruncateToSize(uint64_t Length) noexcept;

    bool Finalize(const char *Path) noexcept;
    inline void Close() noexcept { Tmp.Close(); }
};