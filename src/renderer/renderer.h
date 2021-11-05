#pragma once

// Forward declarations
typedef struct GameData GameData;

typedef struct Vertex {
    Vec3 pos;
    Vec3 normal;
    Vec2 uv;
} Vertex;

typedef struct SkinnedVertex {
    Vec3 pos;
    Vec3 normal;
    Vec2 uv;
    u8 joints[4];
    f32 weights[4];
} SkinnedVertex;

typedef struct Mesh {
    u32 vertex_array;
    u32 vertex_buffer;
    u32 index_buffer;
    
    u32 vertex_count;
    u32 index_count;
} Mesh;

typedef Mesh *MeshHandle;

typedef struct Skin {
    u32 joint_count;
    
    Transform *joints;
    Mat4 *global_joint_mats;
    
    i32 *joint_parents;
    
    u32 *joint_child_count;
    u32 **joint_children;
    Mat4 *inverse_bind_matrices;
} Skin;

typedef Skin *SkinHandle;

// --------
// Animation

typedef enum AnimationType {
    ANIM_TYPE_QUATERNION,
    ANIM_TYPE_VEC3,
    ANIM_TYPE_FLOAT,
    ANIM_TYPE_TRANSFORM,
} AnimationType;

typedef struct AnimationTrack {
    AnimationType type;
    u32 key_count;
    f32 *key_times;
    void *keys;
    void *target;
} AnimationTrack;

typedef struct Animation {
    f32 length;
    u32 track_count;
    AnimationTrack *tracks;
} Animation;

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
    PushBufferEntryType_Mesh,
    PushBufferEntryType_Skin,
    PushBufferEntryType_Texture,
    PushBufferEntryType_Bone,
} PushBufferEntryType;

typedef struct PushBufferEntryUIQuad {
    PushBufferEntryType type;
    u32 l, t, r, b;
    Vec4 colour;
} PushBufferEntryUIQuad;

typedef struct PushBufferEntryText {
    PushBufferEntryType type;
    char *text;
    u32 x, y;
    Vec4 colour;
} PushBufferEntryText;

typedef struct PushBufferEntryMesh {
    PushBufferEntryType type;
    MeshHandle mesh;
    Transform *transform;
    Vec3 diffuse_color;
} PushBufferEntryMesh;

typedef struct PushBufferEntrySkin {
    PushBufferEntryType type;
    MeshHandle mesh;
    SkinHandle skin;
    Transform *transform;
    Vec3 diffuse_color;
} PushBufferEntrySkin;

typedef struct PushBufferEntryBone {
    PushBufferEntryType type;
    Vec3 line[4]; // 0 = bone origin, 1 = X axis, 2 = Y axis, 3 = Z axis
} PushBufferEntryBone;

typedef struct PushBufferEntryTexture {
    PushBufferEntryType type;
    u32 l, t, r, b;
    u32 texture;
} PushBufferEntryTexture;

// --------
// Renderer
typedef struct Renderer {
    RendererBackend backend;
    PlatformWindow *window;
    
    u32 width;
    u32 height;
    
    PushBuffer scene_pushbuffer;
    
    Mesh *meshes;
    u32 mesh_capacity;
    u32 mesh_count;
    
    Skin *skins;
    u32 skin_capacity;
    u32 skin_count;
    
    Transform *transforms;
    u32 transform_capacity;
    u32 transform_count;
    
    // Uniform data
    Mat4 camera_proj;
    Mat4 camera_proj_inverse;
    Mat4 camera_view;
    Mat4 camera_view_inverse;
    Mat4 camera_vp;
    Vec3 camera_pos;
    Mat4 light_matrix;
    Vec3 light_dir;
    
    PushBuffer ui_pushbuffer;
    
    PushBuffer debug_pushbuffer;
    
} Renderer;

void CalcChildXform(u32 joint, Skin *skin);
void UpdateCameraProj(Renderer *renderer);

Transform *RendererAllocateTransforms(Renderer *renderer, const u32 count);
void RendererDestroyTransforms(Renderer *renderer, const u32 count, const Transform *transforms);

/*
// Game functions
MeshHandle RendererLoadMesh(Renderer *renderer, const char *path);
MeshHandle RendererLoadMeshFromVertices(Renderer *renderer, const Vertex *vertices, const u32 vertex_count, const u32 *indices, const u32 index_count);

Transform *RendererAllocateTransforms(Renderer *renderer, const u32 count);


void RendererSetCamera(Renderer *renderer, const Mat4 view, const Vec3 pos);
void RendererSetSunDirection(Renderer *renderer, const Vec3 direction);


internal void UIPushQuad(Renderer *renderer, const u32 x, const u32 y, const u32 w, const u32 h, const Vec4 color);
internal void UIPushText(Renderer *renderer, const char *text, const u32 x, const u32 y, const Vec4 color);
internal void PushMesh(Renderer *renderer, MeshHandle mesh, Transform *transform, Vec3 diffuse_color);
internal void PushSkin(Renderer *renderer, MeshHandle mesh, SkinHandle skin, Transform *transform, Vec3 diffuse_color);

*/