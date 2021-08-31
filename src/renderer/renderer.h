#ifndef RENDERER_H
#define RENDERER_H

#include <sl3dge-utils/sl3dge.h>

#include "platform/platform.h"
#include "renderer/renderer_api.h"

// Forward declarations
typedef struct Renderer Renderer;
typedef struct GameData GameData;

typedef struct Vertex {
    Vec3 pos;
    Vec3 normal;
    Vec2 uv;
} Vertex;

typedef u32 MeshHandle;

// -----------
// Push Buffer

typedef struct PushBuffer {
    u32 size;
    u32 max_size;
    void *buf;
} PushBuffer;

typedef enum PushBufferEntryType {
    PushBufferEntryType_UIQuad,
    PushBufferEntryType_Text,
    PushBufferEntryType_Mesh
} PushBufferEntryType;

typedef struct PushBufferEntryUIQuad {
    PushBufferEntryType type;
    u32 l, t, r, b;
    Vec4 colour;
} PushBufferEntryUIQuad;

typedef struct PushBufferEntryText {
    PushBufferEntryType type;
    const char *text;
    u32 x, y;
    Vec4 colour;
} PushBufferEntryText;

typedef struct PushBufferEntryMesh {
    PushBufferEntryType type;
    MeshHandle mesh_handle;
    Mat4 *transform;
    Vec3 diffuse_color;
} PushBufferEntryMesh;

// Game functions
MeshHandle RendererLoadMesh(Renderer *renderer, const char *path);
MeshHandle RendererLoadMeshFromVertices(Renderer *renderer, const Vertex *vertices, const u32 vertex_count, const u32 *indices, const u32 index_count);
void RendererSetCamera(Renderer *renderer, const Mat4 *view);
void RendererSetSunDirection(Renderer *renderer, const Vec3 direction);

internal void UIPushQuad(Renderer *renderer, const u32 x, const u32 y, const u32 w, const u32 h, const Vec4 color);
internal void UIPushText(Renderer *renderer, const char *text, const u32 x, const u32 y, const Vec4 color);
internal void PushMesh(Renderer *renderer, MeshHandle mesh, Mat4 *transform, Vec3 diffuse_color);

#endif
