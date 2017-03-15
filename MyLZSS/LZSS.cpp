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
    m_IsFill = false;
}

uint32_t LZSS::CalculateHash(const uint8_t* inputCursor){
    return uint32_t(inputCursor[0]);// + uint32_t(inputCursor[1]<<8);
}

int32_t LZSS::InsertNode(int32_t windowCursor,const uint8_t *inputChar,int32_t inputLength)
{
    const uint8_t* inputCursor = (uint8_t*)inputChar;
    int32_t cmp = 1;
    uint32_t hash =  CalculateHash(inputCursor);
    int32_t treeCursor = m_root[hash];
    int32_t matchLength = 0;
    
    for (; ;) {
        
        if (treeCursor == NO_MATCH) {
            
            m_root[hash] = windowCursor;
            m_window[windowCursor].lChild = NO_MATCH;
            m_window[windowCursor].rChild = NO_MATCH;
            break;

        }else{
            
            //int32_t maxMatchLength = windowCursor - treeCursor;
            //maxMatchLength =  maxMatchLength < MAX_MATCH_COUNT ? maxMatchLength:MAX_MATCH_COUNT;
            int32_t maxMatchLength = MAX_MATCH_COUNT;
            matchLength = 1;
            
            do
            {
                cmp = inputCursor[matchLength] - m_Buff[(treeCursor + matchLength)%sizeof(m_Buff)];
                if(cmp == 0 ){
                    matchLength++;
                }
            }while(cmp == 0 && matchLength < maxMatchLength);

            if (cmp > 0)
            {
                if (m_window[treeCursor].rChild == NO_MATCH)
                {
                    m_window[treeCursor].rChild = windowCursor;
                    m_window[windowCursor].dad = treeCursor;
                    break;
                }
                else
                {
                    treeCursor = m_window[treeCursor].rChild;
                    continue;
                }
            }
            else if(cmp < 0)
            {
                if (m_window[treeCursor].lChild == NO_MATCH)
                {
                    m_window[treeCursor].lChild = windowCursor;
                    m_window[windowCursor].dad = treeCursor;
                    break;
                }
                else
                {
                    treeCursor = m_window[treeCursor].lChild;
                    continue;
                }
            }
            else //matchLength > MAX_MATCH_COUNT
            {
                m_window[windowCursor].rChild = m_window[treeCursor].rChild;
                m_window[windowCursor].lChild = m_window[treeCursor].lChild;
                
                break;
            }
        }
    }
    return matchLength;
}

void LZSS::Compress(const void *inputChar,int32_t inputLength,void* outputChar, size_t outputLength){
    
    const uint8_t* input  = reinterpret_cast< const uint8_t* >( inputChar );
    //uint8_t*       output = reinterpret_cast< uint8_t* >( outputChar );
    
    for (int i = 0; i < inputLength; ++i) {
        int32_t matchLength = InsertNode(m_CurPos,input + i,inputLength - i);
        m_Buff[m_CurPos] = input[i];
        
        m_CurPos++;
        if (m_CurPos >= sizeof(m_Buff)) {
            m_CurPos %= m_CurPos/sizeof(m_Buff);
            m_IsFill = true;
        }
    }
}


void LZSS::DeCompress(){
    
}







