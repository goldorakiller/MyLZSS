//
//  main.cpp
//  MyLZSS
//
//  Created by shenqi329 on 2017/3/14.
//  Copyright © 2017年 shenqi329. All rights reserved.
//

#include <iostream>
#include "LZSS.h"

using namespace std;

int main(int argc, const char * argv[]) {
    const char  *s;
    if (argc != 4) {
        printf("'lzss e file1 file2' encodes file1 into file2.\n"
               "'lzss d file2 file1' decodes file2 into file1.\n");
        return EXIT_FAILURE;
    }
    if ((s = argv[1], s[1] || strpbrk(s, "DEde") == NULL)) {
        printf("??? %s\n", s);
        return EXIT_FAILURE;
    }
    
    FILE* inputFile  = fopen( argv[ argc - 2 ], "rb" );
    
    LZSS *lzss = new LZSS();
    
    char readBuf[1024*1024] = "";
    size_t readSize;
    do{
        readSize = fread(readBuf, 1,sizeof(readBuf), inputFile);
        if (readSize > 0) {
            lzss->Compress(readBuf, readSize);
        }
    }while (readSize > 0);
    
	lzss->DeCompress();
    
    return EXIT_SUCCESS;
}
