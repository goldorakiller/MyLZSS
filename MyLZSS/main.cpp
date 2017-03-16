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
    {
        //char inputChar[1024] = {0x01,0x02,0x03,0x04,0x01,0x02,0x03,0x01,0x02,0x03,0x04,0x05,0x01,0x02,0x03,0x04,0x05,0x06,0x01,0x01,0x01,};
        char inputChar[1024] = "abcdabcabcdeabcdefabcdefgabcdefgabcdefgabcdefgabcdefgabcdefgabcdefg";
        lzss->Compress(inputChar, strlen(inputChar));
    }
    {
        char inputChar[1024] = "abcdabcabcdeabcdefabcdefgabcdefgabcdefgabcdefgabcdefgabcdefgabcdefg";//
        lzss->Compress(inputChar, strlen(inputChar));
    }
    return 0;
}
