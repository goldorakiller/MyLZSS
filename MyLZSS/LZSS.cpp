//
//  main.cpp
//  MyLZSS
//
//  Created by shenqi329 on 2017/3/14.
//  Copyright © 2017年 shenqi329. All rights reserved.
//

#include "LZSS.h"
#include <iostream>
#include <stack>
#include <queue>

using namespace std;

LZSS::LZSS()
{
    for (int32_t i = 0; i < ROOT_SIZE; ++i) {
        m_root[i] = EMPTY_NODE;
    }
    for (int32_t i = 0 ; i < LZ_WINDOW_SIZE; ++i){
        m_window[i].lChild = EMPTY_NODE;
        m_window[i].rChild = EMPTY_NODE;
    }
    m_CurPos = 0;
}

uint32_t LZSS::CalculateHash(const uint8_t* inputCursor){
    return uint32_t(inputCursor[0]) + uint32_t(inputCursor[1]<<8);
}

void LZSS::InsertNode(){
    
}

void LZSS::Compress(const void *inputChar,int32_t inputLength,void* outputChar, size_t outputLength){
    
    const uint8_t* input  = reinterpret_cast< const uint8_t* >( inputChar );
    //uint8_t*       output = reinterpret_cast< uint8_t* >( outputChar );
    
    int32_t position = m_CurPos;
    
    const uint8_t* inputCursor = input;
    
    for (; ; ) {
        uint32_t hash =  CalculateHash(inputCursor);
        int32_t treeCursor = m_root[hash];
        
        if (treeCursor == NO_MATCH) {
            m_root[hash] = position;
            m_window[treeCursor].lChild = NO_MATCH;
            m_window[treeCursor].rChild = NO_MATCH;
        }else{
            
            
            
        }
    }
}

void LZSS::DeCompress(){
    
}







