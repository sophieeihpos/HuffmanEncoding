
#include "encoding.h"
#include "mymap.h"
#include <istream>
#include "pqueue.h"
#include "filelib.h"

MyMap buildFrequencyTable(istream& input) {
    MyMap freqTable;
    int currentKey;
    while(true){
        currentKey=input.get();
        if(freqTable.containsKey(currentKey)){
            int currentValue=freqTable.get(currentKey)+1;
            freqTable.put(currentKey,currentValue);
        }else{
            freqTable.put(currentKey,1);
        }
        if(currentKey==-1){
            break;
        }
    }
    return freqTable;
}

HuffmanNode* buildEncodingTree(const MyMap& freqTable) {
    HuffmanNode* zero;
    HuffmanNode* one;
    HuffmanNode* newNode;
    PriorityQueue<HuffmanNode*> pQ;
    Vector<int> keys=freqTable.keys();
    int count;
    for(int key:keys){
        count=freqTable.get(key);
        newNode=new HuffmanNode(key,count,NULL,NULL);
        pQ.add(newNode,count);
    }

    int sumCount;
    while(true){
        zero=pQ.dequeue();
        one=pQ.dequeue();
        sumCount=zero->count+one->count;
        newNode=new HuffmanNode(257,sumCount,zero,one);
        if(!pQ.isEmpty()){
            pQ.add(newNode,sumCount);
        }else{
            break;
        }
    }
    return newNode;
}

void encodeNode(Map<int,string> & encodingMap,HuffmanNode* node,string encoding){
    if(node->zero==NULL && node->one==NULL){
        encodingMap.add(node->character,encoding);
    }else{
        encodeNode(encodingMap,node->zero,encoding+"0");
        encodeNode(encodingMap,node->one,encoding+"1");
    }
    return;
}

Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    if(encodingTree==NULL){
        return encodingMap;
    }
    string encoding="";
    encodeNode(encodingMap,encodingTree,encoding);
    return encodingMap;
}

void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    int character;
    string encoding;
    int bit;

    do{
        character=input.get();
        if(character==-1){
            character=256;
        }
        encoding=encodingMap.get(character);
        for(char c :encoding){
            bit=stringToInteger(charToString(c));
            output.writeBit(bit);
        }
    }while(character!=256);
    return;
}

void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    int bit;
    HuffmanNode* node=encodingTree;
    while(node!=NULL){
        if(node->zero==NULL && node->one==NULL){
            output.put(node->character);
            if(node->character==256){
                break;
            }
            node=encodingTree;
        }else{
            bit = input.readBit();
            if(bit==0){
                node=node->zero;
            }else if (bit==1){
                node=node->one;
            }
        }
    }
    return;
}

void compress(istream& input, obitstream& output) {
    MyMap freqTable = buildFrequencyTable(input);
    HuffmanNode* encodingTree=buildEncodingTree(freqTable);
    Map<int,string> encodingMap = buildEncodingMap(encodingTree);
    output<<freqTable;
    rewindStream(input);
    encodeData(input,encodingMap,output);
    freeTree(encodingTree);
    return;
}

void decompress(ibitstream& input, ostream& output) {
    MyMap freqTable;
    input>>freqTable;
    HuffmanNode* encodingTree=buildEncodingTree(freqTable);
    Map<int,string> encodingMap = buildEncodingMap(encodingTree);
    decodeData(input,encodingTree,output);
    freeTree(encodingTree);
    return;
}

void freeTree(HuffmanNode* node) {
    if(node==NULL){
        delete node;
    }else if(node->zero==NULL && node->one==NULL){
        delete node;
    }else{
        if(node->zero!=NULL){
            freeTree(node->zero);
        }
        if(node->one!=NULL){
            freeTree(node->one);
        }
    }
    return;
}
