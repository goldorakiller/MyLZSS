//
//  main.cpp
//  MyLZSS
//
//  Created by shenqi329 on 2017/3/14.
//  Copyright © 2017年 shenqi329. All rights reserved.
//

#include <iostream>
#include "LZSS.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    
    LZSS *lzss = new LZSS();
    
    char inputChar[1024] = "abcdabcabcdeabcdefabcdefgabcdefgabc";
    char outPutChar[1024] = "";
    
    lzss->Compress(inputChar, strlen(inputChar), outPutChar, sizeof(outPutChar));
    
    return 0;
}
