
internal void PushUIQuad(PushBuffer *push_buffer, const u32 x, const u32 y, const u32 w, const u32 h, const Vec4 color) {
    ASSERT(push_buffer->size + sizeof(PushBufferEntryUIQuad) < push_buffer->max_size);
    PushBufferEntryUIQuad *entry = (PushBufferEntryUIQuad *)(push_buffer->buf + push_buffer->size);
    entry->type = PushBufferEntryType_UIQuad;
    entry->l = x;
    entry->t = y;
    entry->r = w + x;
    entry->b = h + y;
    entry->colour = color;
    push_buffer->size += sizeof(PushBufferEntryUIQuad);
}

// TODO(Guigui): Maybe remove the memallocs ??
internal void PushUIText(PushBuffer *push_buffer, const char *text, const u32 x, const u32 y, const Vec4 color) {
    ASSERT(push_buffer->size + sizeof(PushBufferEntryText) < push_buffer->max_size);
    PushBufferEntryText *entry = (PushBufferEntryText *)(push_buffer->buf + push_buffer->size);
    entry->type = PushBufferEntryType_Text;
    char *saved = sCalloc(128, sizeof(char));
    StringCopyLength(saved, text, 128);
    entry->text = saved;
    entry->x = x;
    entry->y = y;
    entry->colour = color;
    push_buffer->size += sizeof(PushBufferEntryText);
}

internal void PushUIFmt(PushBuffer *push_buffer, const u32 x, const u32 y, const Vec4 color, const char *fmt, ...) {
    ASSERT(push_buffer->size + sizeof(PushBufferEntryText) < push_buffer->max_size);
    PushBufferEntryText *entry = (PushBufferEntryText *)(push_buffer->buf + push_buffer->size);
    entry->type = PushBufferEntryType_Text;
    entry->text = sCalloc(128, sizeof(char));
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(entry->text, 128, fmt, ap);
    va_end(ap);
    entry->x = x;
    entry->y = y;
    entry->colour = color;
    push_buffer->size += sizeof(PushBufferEntryText);
}

internal void PushUITexture(PushBuffer *push_buffer, const u32 texture, const u32 x, const u32 y, const u32 w, const u32 h) {
    ASSERT(push_buffer->size + sizeof(PushBufferEntryTexture) < push_buffer->max_size);
    PushBufferEntryTexture *entry = (PushBufferEntryTexture *)(push_buffer->buf + push_buffer->size);
    entry->type = PushBufferEntryType_Texture;
    entry->l = x;
    entry->t = y;
    entry->r = w + x;
    entry->b = h + y;
    entry->texture = texture;
    push_buffer->size += sizeof(PushBufferEntryTexture);
}

/// Adds a mesh to the scene draw calls
/// The transform pointer needs to be alive until drawing happens
internal void PushMesh(PushBuffer *push_buffer, Mesh *mesh, Transform *transform, Vec3 diffuse_color) {
    ASSERT(push_buffer->size + sizeof(PushBufferEntryMesh) < push_buffer->max_size);
    PushBufferEntryMesh *entry = (PushBufferEntryMesh *)(push_buffer->buf + push_buffer->size);
    entry->type = PushBufferEntryType_Mesh;
    entry->mesh = mesh;
    entry->transform = transform;
    entry->diffuse_color = diffuse_color;
    
    push_buffer->size += sizeof(PushBufferEntryMesh);
}

internal void PushSkin(PushBuffer *push_buffer, Mesh *mesh, Skin *skin, Transform *xform, Vec3 diffuse_color) {
    ASSERT(push_buffer->size + sizeof(PushBufferEntrySkin) < push_buffer->max_size);
    PushBufferEntrySkin *entry = (PushBufferEntrySkin *)(push_buffer->buf + push_buffer->size);
    entry->type = PushBufferEntryType_Skin;
    entry->mesh = mesh;
    entry->skin = skin;
    entry->transform = xform;
    entry->diffuse_color = diffuse_color;
    
    push_buffer->size += sizeof(PushBufferEntrySkin);
}

/// Adds a bone to the scene draw calls
/// The matrix needs to be local
internal void PushBone(PushBuffer *push_buffer, Mat4 bone_matrix) {
    
    ASSERT(push_buffer->size + sizeof(PushBufferEntryBone) < push_buffer->max_size);
    PushBufferEntryBone *entry = (PushBufferEntryBone *)(push_buffer->buf + push_buffer->size);
    
    entry->type = PushBufferEntryType_Bone;
    entry->line[0] = mat4_mul_vec3(bone_matrix, (Vec3){0.0f, 0.0f, 0.0f});
    entry->line[1] = mat4_mul_vec3(bone_matrix, (Vec3){0.1f, 0.0f, 0.0f});
    entry->line[2] = mat4_mul_vec3(bone_matrix, (Vec3){0.0f, 0.1f, 0.0f});
    entry->line[3] = mat4_mul_vec3(bone_matrix, (Vec3){0.0f, 0.0f, 0.1f});
    
    push_buffer->size += sizeof(PushBufferEntryBone);
    
}