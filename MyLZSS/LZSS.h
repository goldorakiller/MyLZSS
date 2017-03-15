//
//  main.h
//  MyLZSS
//
//  Created by shenqi329 on 2017/3/14.
//  Copyright © 2017年 shenqi329. All rights reserved.
//

#include <sys/types.h>
#include <ctype.h>
#include <iostream>

namespace
{
    const int32_t  NO_MATCH                = -1;
    const int32_t  EMPTY_NODE              = -1;
    const uint32_t WINDOW_BITS             = 16;
    const uint32_t MAX_MATCH_COUNT         = 18;
    const uint32_t MIN_MATCH_COUNT         = 3;
    const uint32_t LZ_WINDOW_SIZE          = 1 << WINDOW_BITS;  //2^16
    const uint32_t LZ_WINDOW_MASK          = LZ_WINDOW_SIZE - 1;
    const int32_t  ROOT_SIZE               = LZ_WINDOW_SIZE;
}

struct TreeNode
{
    int32_t lChild;
    int32_t rChild;
    int32_t dad;
};

class LZSS
{
public:
    LZSS();
    void Compress(const void *inputChar,int32_t inputLength,void* outputChar, size_t outputLength);
    void DeCompress();
protected:
    int32_t InsertNode(int32_t windowCursor,const uint8_t *inputChar,int32_t inputLength);
    uint32_t CalculateHash(const uint8_t* inputCursor);
private:
    int32_t  m_root[ROOT_SIZE];
    TreeNode m_window[LZ_WINDOW_SIZE];
    
    char m_Buff[LZ_WINDOW_SIZE];
    int32_t m_CurPos;
    bool m_IsFill;
};

