#pragma once
#include "DGRCore.h"

using namespace DGREngine;

int main(){
    Core danger = Core();
    danger.init();
    while (!danger.shouldClose())
    {
        danger.preLoopProcesses();
        danger.setBlankCol(0.2f, 0.3f, 0.8f, 0.8f);
        danger.draw();
        danger.postLoopProcesses();
    }
    danger.terminate();
 
    return 0;
}