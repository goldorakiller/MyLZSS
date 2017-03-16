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
    for (int32_t i = 0 ; i < LZ_WINDOW_SIZE + ROOT_SIZE; ++i){
        m_window[i].lChild = NO_MATCH;
        m_window[i].rChild = NO_MATCH;
        m_window[i].dad = NO_MATCH;
    }
    m_root = &m_window[LZ_WINDOW_SIZE];
    m_CurPos = 0;
    m_IsFill = false;
}

uint32_t LZSS::CalculateHash(const uint8_t* inputCursor){
    return uint32_t(inputCursor[0]);// + uint32_t(inputCursor[1]<<8);
}

//将节点treeCursor从树中删除
void LZSS::DeleteNode(int32_t deleteCursor)
{
    int32_t replaceCursor = 0;//替换的
    
    if (m_window[deleteCursor].dad == NO_MATCH) {
        return;
    }
    
    if (m_window[deleteCursor].rChild == NO_MATCH) {      //被删除的节点只有左孩子，找到
        replaceCursor = m_window[deleteCursor].lChild;
    }else if (m_window[deleteCursor].lChild == NO_MATCH){ //被删除的节点只有右孩子
        replaceCursor = m_window[deleteCursor].rChild;
    }else{ //找到被删除的节点左节点的最右子节点，作为被删除节点的替换
        replaceCursor = m_window[deleteCursor].lChild;  //找到左孩子
        if (m_window[deleteCursor].rChild != NO_MATCH) {
            do {
                replaceCursor = m_window[replaceCursor].rChild;
            } while (m_window[replaceCursor].rChild != NO_MATCH);
            
            if (m_window[replaceCursor].lChild != NO_MATCH) {
                m_window[m_window[replaceCursor].dad].rChild = m_window[replaceCursor].lChild;
                m_window[m_window[replaceCursor].lChild].dad = m_window[replaceCursor].dad;
            }
            m_window[replaceCursor].lChild = m_window[deleteCursor].lChild;
            m_window[m_window[deleteCursor].lChild].dad = replaceCursor;
        }
        m_window[replaceCursor].rChild = m_window[deleteCursor].rChild;
        m_window[m_window[deleteCursor].rChild].dad = replaceCursor;
    }
    
    //更新替换节点的父节点为被删除节点的父节点
    m_window[replaceCursor].dad = m_window[deleteCursor].dad;
    
    //将父节点的子节点设置为要替换的节点
    if (m_window[m_window[deleteCursor].dad].rChild == deleteCursor) {//被删除的节点是右子节点
        m_window[m_window[deleteCursor].dad].rChild = replaceCursor;  //将右子节点设为替换的节点
    }else{
        m_window[m_window[deleteCursor].dad].lChild = replaceCursor;
    }
    
    //删除当前节点
    m_window[deleteCursor].dad = NO_MATCH;
}

int32_t LZSS::InsertNode(int32_t windowCursor,const uint8_t *inputChar,int32_t inputLength)
{
    const uint8_t* inputCursor = (uint8_t*)inputChar;
    int32_t cmp = 1;
    uint32_t hash =  CalculateHash(inputCursor);
    int32_t treeCursor = m_window[hash + LZ_WINDOW_SIZE].rChild;
    int32_t matchLength = 0;
    
    if (treeCursor == NO_MATCH) {
        m_window[hash + LZ_WINDOW_SIZE].rChild = windowCursor;
        m_window[windowCursor].lChild = NO_MATCH;
        m_window[windowCursor].rChild = NO_MATCH;
        m_window[windowCursor].dad = hash + LZ_WINDOW_SIZE;
        return matchLength;
    }
    
    for (; ;) {
        //int32_t maxMatchLength = windowCursor - treeCursor;
        //maxMatchLength =  maxMatchLength < MAX_MATCH_COUNT ? maxMatchLength:MAX_MATCH_COUNT;
        int32_t maxMatchLength = MAX_MATCH_COUNT;
        matchLength = 1;
        
        do{
            cmp = inputCursor[matchLength] - m_Buff[(treeCursor + matchLength)%sizeof(m_Buff)];
            if(cmp == 0 ){
                matchLength++;
            }
        }while(cmp == 0 && matchLength < maxMatchLength);
        
        if (cmp > 0){
            if (m_window[treeCursor].rChild == NO_MATCH){
                m_window[treeCursor].rChild = windowCursor;
                m_window[windowCursor].dad = treeCursor;
                break;
            }else{
                treeCursor = m_window[treeCursor].rChild;
            }
        }else if(cmp < 0){
            if (m_window[treeCursor].lChild == NO_MATCH){
                m_window[treeCursor].lChild = windowCursor;
                m_window[windowCursor].dad = treeCursor;
                break;
            }else{
                treeCursor = m_window[treeCursor].lChild;
            }
        }else{//matchLength > maxMatchLength 替换节点
            m_window[windowCursor].dad = m_window[treeCursor].dad;
            m_window[windowCursor].rChild = m_window[treeCursor].rChild;
            m_window[windowCursor].lChild = m_window[treeCursor].lChild;
            
            m_window[m_window[treeCursor].lChild].dad = windowCursor;
            m_window[m_window[treeCursor].rChild].dad = windowCursor;
            
            if (m_window[m_window[treeCursor].dad].lChild == treeCursor) {
                m_window[m_window[treeCursor].dad].lChild = windowCursor;
            }else{
                m_window[m_window[treeCursor].dad].rChild = windowCursor;
            }
            
            m_window[treeCursor].dad = NO_MATCH;
            break;
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







