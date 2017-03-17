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
    m_CurPos = 0;
	m_Mask = 0;
    m_Total = 0;
    m_IsFill = false;
    m_CompressIndex = 0;
    m_CompressMaskIndex = 0;
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
        if (m_window[replaceCursor].rChild != NO_MATCH) {
            do {
                replaceCursor = m_window[replaceCursor].rChild;
            } while (m_window[replaceCursor].rChild != NO_MATCH);
           
            m_window[m_window[replaceCursor].dad].rChild = m_window[replaceCursor].lChild;
            m_window[m_window[replaceCursor].lChild].dad = m_window[replaceCursor].dad;
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

Match LZSS::InsertNode(int32_t windowCursor,const uint8_t *inputChar,int32_t inputLength)
{
    Match match{NO_MATCH,1,0};
    
    const uint8_t* inputCursor = (uint8_t*)inputChar;
    int32_t cmp = 1;
    uint32_t hash =  CalculateHash(inputCursor);
    int32_t treeCursor = m_window[hash + LZ_WINDOW_SIZE].rChild;
    int32_t matchLength = 0;
    
    m_window[windowCursor].lChild = NO_MATCH;
    m_window[windowCursor].rChild = NO_MATCH;
    
    if (treeCursor == NO_MATCH) {
        m_window[hash + LZ_WINDOW_SIZE].rChild = windowCursor;
        m_window[windowCursor].dad = hash + LZ_WINDOW_SIZE;
        return match;
    }
    if (inputLength < MIN_MATCH_COUNT) {
        return match;
    }
    if (m_Total == 12) {
        windowCursor = windowCursor;
    }
    for (; ;) {
        
        int32_t maxMatchLength = MAX_MATCH_COUNT > inputLength ? inputLength : MAX_MATCH_COUNT;
        matchLength = 1;
        
        do{
            if(m_IsFill && treeCursor > windowCursor){
                if (treeCursor + matchLength < windowCursor + LZ_WINDOW_SIZE) {
                    cmp = inputCursor[matchLength] - m_Buff[(treeCursor + matchLength)%LZ_WINDOW_SIZE];
                }else{
                    cmp = inputCursor[matchLength] - inputCursor[(treeCursor + matchLength - windowCursor)%LZ_WINDOW_SIZE];
                }
            }else{//treeCursor必然小于windowCursor
                if (treeCursor + matchLength >= windowCursor) {
                    cmp = inputCursor[matchLength] - inputCursor[(treeCursor + matchLength - windowCursor)%LZ_WINDOW_SIZE];
                }else{
                    cmp = inputCursor[matchLength] - m_Buff[(treeCursor + matchLength)%LZ_WINDOW_SIZE];
                }
            }
            if(cmp == 0 ){
                matchLength++;
            }
        }while(cmp == 0 && matchLength < maxMatchLength);
        
        if (matchLength > match.length) {
            match.position = treeCursor;
            match.length = matchLength;
            if (windowCursor < treeCursor) {
                match.offset = windowCursor + LZ_WINDOW_SIZE - treeCursor;
            }else{
                match.offset = windowCursor - treeCursor;
            }
        }
        
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
    
    return match;
}

void LZSS::Compress(const void *inputChar,int32_t inputLength){
    
    const uint8_t* input  = reinterpret_cast< const uint8_t* >( inputChar );
	int32_t matchPos = -1;
    for (int i = 0; i < inputLength; ++i) {
        
        if (m_CurPos >= LZ_WINDOW_SIZE) {
            m_CurPos %= m_CurPos/LZ_WINDOW_SIZE;
            m_IsFill = true;
        }
        if (m_IsFill) {
            DeleteNode(m_CurPos);
        }
        m_Buff[m_CurPos] = input[i];
        
        if (i == 256){
            i = i;//"abcdabcabcdeabcdefabcdefgabcdefgabcdefgabcdefg";
        }
        
        Match result = InsertNode(m_CurPos,input + i,inputLength - i);
        //printf("(%5d %5d %5c %5d %5zu %5d)\n",m_Total,m_CurPos,char(input[i]),result.position,result.length,result.offset);
		if (m_Total > matchPos) {
			if (result.length < MIN_MATCH_COUNT) {
				if (m_Mask == 0) {
					m_CompressMaskIndex = m_CompressIndex;
					m_CompressIndex++;
                    m_Mask = 1;
				}
				m_CompressBuff[m_CompressMaskIndex] = m_Mask;
				m_CompressBuff[m_CompressIndex++] = input[i];
				matchPos = m_Total;
                m_Mask++;
                if (m_Mask > 8) {
                    m_Mask = 0;
                }
			}
			else {
				m_Mask = 0;
				matchPos = m_Total + int32_t(result.length) - 1;
				m_CompressBuff[m_CompressIndex] = 0;
				m_CompressBuff[m_CompressIndex++] = (result.length - MIN_MATCH_COUNT) << 4;
				*(uint16_t*)(&(m_CompressBuff[m_CompressIndex])) = result.offset;
                m_CompressIndex += 2;
			}
		}
        m_CurPos++;
        m_Total++;
    }
    printf("压缩率%4f\n",1 - (float(m_CompressIndex))/m_Total);
}

void LZSS::DeCompress(){
	
	const uint8_t* input = reinterpret_cast< const uint8_t* >(m_CompressBuff);
	int32_t inputLength = m_CompressIndex;

	char output[1024*1024] = "";
    FILE* outputFile  = fopen( "decompress.txt", "wb+" );
    FILE* compressFile  = fopen( "compress.txt", "wb+" );
    fwrite(input, 1, inputLength, compressFile);
    
	int32_t outPutPos = 0;
	int i = 0;

	while(i < inputLength) {

		uint8_t mark = input[i];

		if (mark & 0x0f) {
			int32_t count = (mark & 0x0f);
			++i;
			while (count > 0) {
				output[outPutPos++] = input[i++];
				count--;
			}
		}else {
			int32_t matchLength = ((mark & 0xf0) >> 4) + MIN_MATCH_COUNT;
			int32_t offset = *((uint16_t*)(&(input[i + 1])));
			int32_t matchPos = outPutPos - offset;
			i += 3;
			while (matchLength > 0) {
				output[outPutPos++] = output[matchPos++];
				matchLength--;
			}
		}
	}
    fwrite(output, 1, outPutPos, outputFile);
}







