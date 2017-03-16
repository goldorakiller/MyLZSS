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
    const uint32_t WINDOW_BITS             = 5;
    const uint32_t MAX_MATCH_COUNT         = 16;
    const uint32_t MIN_MATCH_COUNT         = 2;
    const uint32_t LZ_WINDOW_SIZE          = 1 << WINDOW_BITS;  //2^16
    const uint32_t LZ_WINDOW_MASK          = LZ_WINDOW_SIZE - 1;
    const int32_t  ROOT_SIZE               = 1<<8;
}

struct Match{
    int32_t  position;
    size_t   length;
    uint16_t offset;
};

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
    void DeleteNode(int32_t deleteCursor);
    Match InsertNode(int32_t windowCursor,const uint8_t *inputChar,int32_t inputLength);
    uint32_t CalculateHash(const uint8_t* inputCursor);
private:
    TreeNode m_window[LZ_WINDOW_SIZE + ROOT_SIZE];
    TreeNode *m_root;
    char m_Buff[LZ_WINDOW_SIZE];
    int32_t m_CurPos;
    int32_t m_Total;
    bool m_IsFill;
};

