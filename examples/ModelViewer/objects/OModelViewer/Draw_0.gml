draw_clear(#202020);

// We don't need to do anything if there's no model loaded
if (model == undefined)
{
	exit;
}

// Apply our 3D camera
camera_apply(camera);

// Change GPU settings to enable 3D
gpu_push_state();
gpu_set_zwriteenable(true);
gpu_set_ztestenable(true);
gpu_set_tex_mip_enable(mip_on);
gpu_set_tex_filter(textureFilter);

// Use the basic shader
shader_set(ShBasic);
shader_set_uniform_f(shader_get_uniform(ShBasic, "u_vCamera"), x, y, z);
texture_set_stage(shader_get_sampler_index(ShBasic, "u_texNormal"),
	sprite_get_texture(spriteNormal ?? SprNormal, 0));
texture_set_stage(shader_get_sampler_index(ShBasic, "u_texSpecular"),
	sprite_get_texture(spriteSpecular ?? SprSpecular, 0));
texture_set_stage(shader_get_sampler_index(ShBasic, "u_texGloss"),
	sprite_get_texture(spriteGloss ?? SprGloss, 0));

// Draw the model with its transform matrix applied
matrix_set(matrix_world, matrix);
vertex_submit(model, pr_trianglelist, sprite_get_texture(spriteDiffuse ?? SprDiffuse, 0));
matrix_set(matrix_world, matrixIdentity);

// Unset shader
shader_reset();

// Restore previous GPU settings
gpu_pop_state();
