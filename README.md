# ktool
Command-line tool and mini-framework to analyze Apple mach-o and dyld_shared_cache files

# Usage Menu
```
Usage: ktool [Operation] [Operation-Options] [Path] [Path-Options]
Options:
	     --help,                    Print this Menu
	     --usage,                   Print this Menu
Operations:
	-h,  --header,                  Print header of a Mach-O File
		Supports: Mach-O Files │ FAT Mach-O Files │ Apple dyld_shared_cache Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		    -v, --verbose, Print more Verbose Information

	-l,  --lc,                      Print Load-commands of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		    -v, --verbose, Print more Verbose Information

	-L,  --libraries,               Print imported Shared-Libraries of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		        --sort-by-current-version, Sort by Current-Version
		        --sort-by-compat-version,  Sort by Compat-Version
		        --sort-by-index,           Sort by LC Index
		        --sort-by-name,            Sort by Name
		        --sort-by-timestamp,       Sort by TimeStamp
		        --verbose,                 Print more Verbose Information

	-Id, --Identity,                Print Identification of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		    -v, --verbose, Print more Verbose Information

	     --list-archs,              List Archs of a FAT Mach-O File
		Supports: FAT Mach-O Files
		Options:
		    -v, --verbose, Print more Verbose Information

	     --list-export-trie,        List Export-Trie of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		        --only-count,      Only print the count of exports, or with --tree, print the number of nodes
		        --require-kind,    Only print exports with a specific kind
		        --require-segment, Only print exports with a specific segment
		        --require-section, Only print exports with a specific segment & section
		        --sort,            Sort every node alphabetically
		    -v, --verbose,         Print more Verbose Information

	     --list-objc-classes,       List Objc-Classes of a Thin Mach-O File
		Supports: Mach-O Files
		Options:
		        --include-categories,    Include Objective-C Class Categories
		        --sort-by-dylib-ordinal, Sort Objective-C Classes by Dylib-Ordinal
		        --sort-by-kind,          Sort Objective-C Classes by Kind
		        --sort-by-name,          Sort Objective-C Classes by Name
		        --tree,                  Print Objective-C Classes in a tree
		    -v, --verbose,               Print more Verbose Information

	     --list-bind-actions,       List Bind-Actions of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		        --only-normal,           Only Print Normal-Bind Action-List
		        --only-lazy,             Only Print Lazy-Bind Action-List
		        --only-weak,             Only Print Weak-Bind Action-List
		        --sort-by-dylib-ordinal, Sort Bind-Actions by Dylib-Ordinal
		        --sort-by-name,          Sort Bind-Actions by Symbol-Name
		        --sort-by-type,          Sort Bind-Actions by Type
		    -v, --verbose,               Print more Verbose Information

	     --list-bind-opcodes,       List Bind-Opcodes of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
			    --only-normal,           Only Print Normal-Bind Opcode-List
			    --only-lazy,             Only Print Lazy-Bind Opcode-List
			    --only-weak,             Only Print Weak-Bind Opcode-List
			-v, --verbose,               Print more Verbose Information

	     --list-bind-symbols,       List Bind-Symbols of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		        --only-normal,           Only Print Normal-Bind Action-List
		        --only-lazy,             Only Print Lazy-Bind Action-List
		        --only-weak,             Only Print Weak-Bind Action-List
		        --sort-by-dylib-ordinal, Sort Bind-Actions by Dylib-Ordinal
		        --sort-by-name,          Sort Bind-Actions by Symbol-Name
		        --sort-by-type,          Sort Bind-Actions by Type
		    -v, --verbose,               Print more Verbose Information

	     --list-rebase-actions,     List Rebase-Actions of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		        --sort,    Sort Rebase-Actions
		    -v, --verbose, Print more Verbose Information

	     --list-rebase-opcodes,     List Rebase-Opcodes of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		    -v, --verbose, Print more Verbose Information

	     --list-c-string-section,   List C-Strings of a C-String Section of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		        --sort,    Sort C-String List
		    -v, --verbose, Print more Verbose Information

	     --list-symbol-ptr-section, List Symbols of a Symbol-Ptr Section of a Thin Mach-O File
		Supports: Mach-O Files │ Apple dyld_shared_cache Mach-O Images
		Options:
		        --sort-by-dylib-ordinal, Sort C-String List by Dylib-Ordinal
		        --sort-by-index,         Sort C-String List by Index
		        --sort-by-symbol,        Sort C-String List by Symbol-Name
		    -v, --verbose,               Print more Verbose Information

	     --list-dsc-images,         List Images of a Dyld Shared-Cache File
		Supports: Apple dyld_shared_cache Files
		Options:
		        --count,              Only print image-count
		        --sort-by-address,    Sort Image List by Image-Address
		        --sort-by-inode,      Sort Image List by Inode
		        --sort-by-modtime,    Sort Image List by Modification-Time
		        --sort-by-name,       Sort Image List by Name
		    -v, --verbose,            Print more Verbose Information
Path-Options:
	--arch <ordinal>,          Select arch (at ordinal) of a FAT Mach-O File
	--image <path-or-ordinal>, Select image (at path or ordinal) of an Apple dyld_shared_cache file
```
