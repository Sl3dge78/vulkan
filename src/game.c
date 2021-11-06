#include <stdint.h>
#include <stdlib.h>

#include "utils/sl3dge.h"

#include "platform/platform.h"

#include "utils/sGltf.c"

#include "renderer/pushbuffer.h"
#include "renderer/renderer.h"


#ifdef RENDERER_VULKAN
#include "renderer/vulkan/vulkan_renderer.c"
//typedef VulkanRenderer Renderer;
#elif RENDERER_OPENGL
#include "renderer/opengl/opengl_renderer.h"
#include "renderer/opengl/opengl_renderer.c"
#include "renderer/opengl/opengl_mesh.c"
#endif

#include "renderer/pushbuffer.c"


#include "console.h"
#include "event.h"
#include "game.h"

#include "renderer/renderer.c"

global Renderer *global_renderer;
global PlatformAPI *platform;

#include "console.c"
#include "event.c"

#include "collision.c"

DLL_EXPORT u32 GameGetSize() {
    return sizeof(GameData);
}


// ---------------
// Exported functions

/// This is called when the game is (re)loaded
DLL_EXPORT void GameInit(GameData *game_data, Renderer *renderer, PlatformAPI *platform_api) {
    global_renderer = renderer;
    platform = platform_api;
    
    ConsoleInit(&game_data->console);
    global_console = &game_data->console;
    sLogSetCallback(&ConsoleLogMessage);
    
    Leak_SetList(platform_api->DebugInfo);
}

/// This is called ONCE before the first frame. Won't be called upon reloading.
DLL_EXPORT void GameStart(GameData *game_data) {
    
    game_data->light_dir = (Vec3){0.67f, -0.67f, 0.1f};
    game_data->camera.position = (Vec3){0.0f, 1.7f, 0.0f};
    game_data->cos = 0;
    RendererSetSunDirection(global_renderer, vec3_normalize(vec3_fmul(game_data->light_dir, -1.0)));
    
    platform->SetCaptureMouse(true);
    
    //LoadFromGLTF("resources/3d/character/character.gltf", global_renderer, platform, &game_data->character, 0, 0);
    LoadFromGLTF("resources/3d/skintest.gltf", global_renderer, platform, &game_data->cylinder_mesh, &game_data->cylinder_skin, 0);
    
    game_data->floor = RendererLoadQuad(global_renderer);
    game_data->floor_xform = RendererAllocateTransforms(global_renderer, 1);
    game_data->floor_xform->scale = (Vec3){100.0f, 1.0f, 100.0f};
    
    game_data->npc_xform = RendererAllocateTransforms(global_renderer, 1);
    game_data->npc_xform->translation.z = 5.0f;
    
    game_data->cylinder_xform = RendererAllocateTransforms(global_renderer, 1);
    game_data->cylinder_xform->translation.x = 0.0f;
    
    game_data->interact_sphere_diameter = 1.0f;
    
#if 0
    game_data->anim_time = 0.0f;
    
    
    {
        Animation *a = &game_data->anim;
        a->track_count = 2;
        a->tracks = sCalloc(a->track_count, sizeof(AnimationTrack));
        {
            a->tracks[0].type = ANIM_TYPE_QUATERNION;
            a->tracks[0].key_count = 3;
            a->tracks[0].key_times = sCalloc(a->tracks[0].key_count, sizeof(f32));
            
            Quat *keys = sCalloc(a->tracks[0].key_count, sizeof(Quat));
            
            keys[0] = quat_from_axis((Vec3){0.0f,0.0f,1.0f}, 0.0f);
            a->tracks[0].key_times[0] = 0.0f;
            keys[1] = quat_from_axis((Vec3){0.0f,0.0f,1.0f}, 1.14f);
            a->tracks[0].key_times[1] = 1.0f;
            keys[2] = quat_from_axis((Vec3){0.5f,0.0f,0.5f}, -3.14f);
            a->tracks[0].key_times[2] = 3.0f;
            a->tracks[0].keys = keys;
            a->tracks[0].target = &game_data->skinned_mesh->joints[1].rotation;
            a->length = a->tracks[0].key_times[a->tracks[0].key_count - 1];
        }
        {
            a->tracks[1].type = ANIM_TYPE_VEC3;
            a->tracks[1].key_count = 3;
            a->tracks[1].key_times = sCalloc(a->tracks[1].key_count, sizeof(f32));
            
            Vec3 * keys = sCalloc(a->tracks[1].key_count, sizeof(Vec3));
            keys[0] = (Vec3){0.0f,0.0f,1.0f};
            a->tracks[1].key_times[0] = 0.0f;
            keys[1] = (Vec3){0.0f,0.0f,2.0f};
            a->tracks[1].key_times[1] = 1.0f;
            keys[2] = (Vec3){0.0f,1.0f,1.0f};
            a->tracks[1].key_times[2] = 2.0f;
            a->tracks[1].keys = keys;
            a->tracks[1].target = &game_data->skinned_mesh->joints[0].translation;
            if (a->length < a->tracks[1].key_times[a->tracks[1].key_count - 1]) 
                a->length = a->tracks[1].key_times[a->tracks[1].key_count - 1];
        }
        
    }
#endif
}

/// Do deallocation here
DLL_EXPORT void GameEnd(GameData *game_data) {
    sFree(game_data->event_queue.queue);
    
    //DestroyAnimation(&game_data->anim);
}

internal void FPSCamera(Camera *camera, Input *input, bool is_free_cam) {
    f32 look_speed = 0.01f;
    
    if(input->mouse_delta_x != 0) {
        camera->spherical_coordinates.x += look_speed * input->mouse_delta_x;
    }
    if(input->mouse_delta_y != 0) {
        float new_rot = camera->spherical_coordinates.y + look_speed * input->mouse_delta_y;
        if(new_rot > -PI / 2.0f && new_rot < PI / 2.0f) {
            camera->spherical_coordinates.y = new_rot;
        }
    }
    
    camera->forward = spherical_to_carthesian(camera->spherical_coordinates);
    
    Vec3 flat_forward = vec3_normalize((Vec3){camera->forward.x, 0.0f, camera->forward.z});
    Vec3 right = vec3_cross(flat_forward, (Vec3){0.0f, 1.0f, 0.0f});
    
    // --------------
    // Move
    
    f32 move_speed = 0.1f;
    Vec3 movement = {0};
    
    if(input->keyboard[SCANCODE_LSHIFT]) {
        move_speed *= 5.0f;
    }
    
    if(input->keyboard[SCANCODE_W]) {
        movement = vec3_add(movement, vec3_fmul(flat_forward, move_speed));
    }
    if(input->keyboard[SCANCODE_S]) {
        movement = vec3_add(movement, vec3_fmul(flat_forward, -move_speed));
    }
    if(input->keyboard[SCANCODE_A]) {
        movement = vec3_add(movement, vec3_fmul(right, -move_speed));
    }
    if(input->keyboard[SCANCODE_D]) {
        movement = vec3_add(movement, vec3_fmul(right, move_speed));
    }
    if(is_free_cam) {
        if(input->keyboard[SCANCODE_Q]) {
            movement.y -= move_speed;
        }
        if(input->keyboard[SCANCODE_E]) {
            movement.y += move_speed;
        }
    }
    
    camera->position = vec3_add(camera->position, movement);
    
    if(!is_free_cam) {
        camera->position.y = 1.7f;
    }
    
    Mat4 cam;
    mat4_look_at(vec3_add(camera->forward, camera->position), camera->position, (Vec3){0.0f, 1.0f, 0.0f}, cam);
    
    RendererSetCamera(global_renderer, cam, camera->position);
}

/// Called every frame
DLL_EXPORT void GameLoop(float delta_time, GameData *game_data, Input *input) {
    // ----------
    // Input
    
    // Console
    if(input->keyboard[SCANCODE_TILDE] && !input->old_keyboard[SCANCODE_TILDE]) {
        if(game_data->console.console_target <= 0) { // Console is closed, open it
            game_data->console.console_target = 300;
            input->read_text_input = true;
            platform->SetCaptureMouse(false);
        } else { // Console is opened, close it
            game_data->console.console_target = 0;
            input->read_text_input = false;
            platform->SetCaptureMouse(true);
        }
    }
    
    if(game_data->console.console_open) {
        InputConsole(&game_data->console, input, game_data);
    } else {
        FPSCamera(&game_data->camera, input, game_data->is_free_cam);
        
        // Interact sphere
        game_data->interact_sphere_pos = vec3_add(game_data->camera.position, game_data->camera.forward);
        
        if(input->keyboard[SCANCODE_E] && !input->old_keyboard[SCANCODE_E]) {
            if(IsLineIntersectingBoundingBox(game_data->camera.position, game_data->interact_sphere_pos, game_data->npc_xform)) {
                game_data->npc_xform->rotation.y += 1.0f;
            }
        }
        
        // Move the sun
        if(input->keyboard[SCANCODE_P]) {
            game_data->cos += delta_time;
            game_data->light_dir.x = cos(game_data->cos);
            game_data->light_dir.y = sin(game_data->cos);
            
            if(game_data->cos > 2.0f * PI) {
                game_data->cos = 0.0f;
            }
            RendererSetSunDirection(global_renderer, game_data->light_dir);
        }
        
        if(input->keyboard[SCANCODE_O]) {
            game_data->cos -= delta_time;
            game_data->light_dir.x = cos(game_data->cos);
            game_data->light_dir.y = sin(game_data->cos);
            if(game_data->cos < 0.0f) {
                game_data->cos = 2.0f * PI;
            }
            
            RendererSetSunDirection(global_renderer, game_data->light_dir);
        }
        
        if(input->keyboard[SCANCODE_Y]) {
            game_data->cylinder_skin->joints[2].rotation = quat_identity();
        }
        if(input->keyboard[SCANCODE_U]) {
            game_data->cylinder_skin->joints[2].rotation = quat_from_axis((Vec3){1.0f, 0.0f, 0.0f}, 90.f);
        }
        
    }
    
    // --------------
    // Event
    
    EventType e;
    while(EventConsume(&game_data->event_queue, &e)) {
        switch(e) {
            case(EVENT_TYPE_QUIT): {
                platform->RequestExit();
            } break;
            case(EVENT_TYPE_RESTART): {
                GameStart(game_data);
            } break;
            case(EVENT_TYPE_RELOAD): {
                platform->RequestReload();
            } break;
            default:
            
            break;
        };
    }
    
    // -------------
    // Drawing
    
    DrawConsole(&game_data->console, game_data);
#if 0
    { // Animation
        game_data->anim_time = fmod(game_data->anim_time + delta_time, game_data->anim.length);
        AnimationEvaluate(&game_data->anim, game_data->anim_time);
    }
#endif
    
    PushMesh(&global_renderer->scene_pushbuffer, game_data->floor, game_data->floor_xform, (Vec3){0.5f, 0.5f, 0.5f});
    //PushMesh(global_renderer, game_data->cylinder_mesh, game_data->npc_xform, (Vec3){1.0f, 1.0f, 1.0f});
    PushSkin(&global_renderer->scene_pushbuffer, game_data->cylinder_mesh, game_data->cylinder_skin, game_data->cylinder_xform, (Vec3){1.0f, 1.0f, 1.0f});
    
    for (u32 i = 0; i < 4; i++) {
        PushBone(&global_renderer->debug_pushbuffer, game_data->cylinder_skin->global_joint_mats[i]);
    }
    
    if(game_data->show_shadowmap)
        PushUITexture(&global_renderer->ui_pushbuffer, global_renderer->backend->shadowmap_pass.texture, 0, 0, 500, 500);
    
    RendererSetSunDirection(global_renderer, game_data->light_dir);
}