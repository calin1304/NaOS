#include "vfs.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

size_t vfs_fread(void *ptr, size_t size, size_t count, FILE* f)
{
    return f->fread(ptr, size, count, f);
}

void vfsInitNode(VFSNode *r, const char *name, const char *path)
{
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->path = malloc(strlen(path) + strlen(name) + 1);
    strcpy(r->path, path);
    strcpy(r->path + strlen(r->path), name);
    r->childNodesCount = 0;
    r->childNodeSize = 0;
    r->childNodes = NULL;
    r->mountedFS = NULL;
}

void vfsInit()
{
    vfsInitNode(&gVFSRoot, "/", "");
}

VFSNode* vfsFind(VFSNode *r, const char *name)
{
    unsigned int i;
    for (i = 0; i < r->childNodesCount; ++i) {
        if (strcmp(r->childNodes[i]->name, name) == 0) {
            return r->childNodes[i];
        }
    }
    return NULL;
}

void vfsResize(VFSNode *r, int newSize)
{
    VFSNode **newMem = malloc(sizeof(VFSNode*) * newSize);
    memcpy(newMem, r->childNodes, sizeof(VFSNode) * r->childNodesCount);
    // free(r->childNodes);
    r->childNodes = newMem;
    r->childNodeSize = newSize;
}

VFSNode* vfsNodeAddChild(VFSNode *r, const char *name)
{
    if (r->childNodeSize == r->childNodesCount) {
        vfsResize(r, r->childNodeSize * 2);
    }
    VFSNode *newNode = vfsFind(r, name);
    if (newNode == NULL) {
        malloc(sizeof(VFSNode));
        vfsInitNode(newNode, name, r->path);
        r->childNodes[r->childNodesCount] = newNode;
        r->childNodesCount += 1;
    }
    return newNode;
}

void vfsMount(const char *path, FileSystem *fs)
{
    if (path[0] == '/') {
        char *s = malloc(strlen(path) + 1);
        strcpy(s, path);
        VFSNode *currNode = &gVFSRoot;
        char *p = strtok(s, "/");
        if (p == NULL) {
            currNode->mountedFS = fs;
        } else {
            while (p != NULL) {
                currNode = vfsNodeAddChild(currNode, p);
                p = strtok(NULL, "/");
            }
            currNode->mountedFS = fs;
        }
        // free(s);
    }
}

VFSNode* _vfsGetFileSystemNode(VFSNode *r, char *p)
{
    if (p == NULL) {
        return NULL;
    }
    VFSNode* s = _vfsGetFileSystemNode(r, strtok(NULL, "/"));
    if (s == NULL) {
        return r;
    } else {
        return s;
    }
}

VFSNode* vfsGetFileSystemNode(VFSNode *r, const char *path)
{
    char *s = malloc(strlen(path) + 1);
    strcpy(s, path);
    char *p = strtok(s, "/");
    return _vfsGetFileSystemNode(r, p);
}

char* vfsGetFilePath(VFSNode* r, char *path)
{
    return path + strlen(r->path);
}

FILE* vfsFOpen(const char *path, const char *mode)
{
    VFSNode *node = vfsGetFileSystemNode(&gVFSRoot, path);
    if (node == NULL) {
        return NULL;
    }
    if (node) {
        char *fsFilePath = vfsGetFilePath(node, path);
        return node->mountedFS->fopen(fsFilePath, mode);
    }
    return NULL;
}