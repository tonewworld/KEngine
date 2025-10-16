#include "kepch.h"
#include "Object.h"

namespace KEngine {
    // 从 1 开始分配 ID，避免 0（保留为“无对象”）
    unsigned int Object::IDCounter = 0;
}