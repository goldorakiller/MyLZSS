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

void LZSS::InsertNode(int32_t windowCursor,const void *inputChar,int32_t inputLength)
{
    const uint8_t* inputCursor = input;
    int32_t cmp = 1;
    uint32_t hash =  CalculateHash(inputCursor);
    int32_t treeCursor = m_root[hash];
    
    for (; ; ) {    
        
        if (treeCursor == NO_MATCH) {
            
            m_root[hash] = windowCursor;
            m_window[windowCursor].lChild = NO_MATCH;
            m_window[windowCursor].rChild = NO_MATCH;
            break;

        }else{
            
            int32_t maxMatchLength = windowCursor - treeCursor;
            maxMatchLength = maxMatchLength < 16 ? maxMatchLength ：16;
            
            int32_t matchLength = 2;
            do
            {
                cmp = inputChar[treeCursor + matchLength] - inputChar[windowCursor + matchLength]
                matchLength++;

            }while(cmp == 0 && matchLength < maxMatchLength)

            if (cmp > 0)
            {
                if (m_window[treeCursor].rChild == NO_MATCH)
                {
                    m_window[treeCursor].rChild = windowCursor;
                }
                else
                {
                    treeCursor = m_window[treeCursor].rChild;
                    continue;
                }
            }
            else if(cmp < 0)
            {
                m_window[treeCursor].lChild = windowCursor;
                if (m_window[treeCursor].lChild == NO_MATCH)
                {
                    m_window[treeCursor].lChild = windowCursor;
                }
                else
                {
                    treeCursor = m_window[treeCursor].lChild;
                    continue
                }
            }
            else //matchLength > 16
            {

            }
        }
    }
}

void LZSS::Compress(const void *inputChar,int32_t inputLength,void* outputChar, size_t outputLength){
    
    const uint8_t* input  = reinterpret_cast< const uint8_t* >( inputChar );
    //uint8_t*       output = reinterpret_cast< uint8_t* >( outputChar );
    
    int32_t windowCursor = m_CurPos;

    InsertNode(windowCursor,inputChar,inputLength);
    
}


void LZSS::DeCompress(){
    
}







