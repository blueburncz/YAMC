display_set_gui_maximize(1, 1);

// The camera used when rendering the 3D model
camera = camera_create();

// This object's (camera's) position on z axis
z = 0;

// Camera's horizontal direction
direction = -45;

// Camera's vertical direction
directionUp = 25;

// Camera's distance from 0, 0, 0
zoom = 1;

// Mouse X position from previous frame
mouseLastX = 0;

// Mouse Y position from previous frame
mouseLastY = 0;

// The loaded vertex buffer or undefined
model = undefined;

// The model's diffuse texture
spriteDiffuse = undefined;

// The model's normal map texture
spriteNormal = undefined;

// The model's specular color texture
spriteSpecular = undefined;

// The model's glossiness texture (0 = rough, 255 = smooth)
spriteGloss = undefined;

// Use true to enable texture filtering
textureFilter = true;

// Cache the identity matrix just so we dont make a new one every frame
matrixIdentity = matrix_build_identity();

// The model's transformation matrix
matrix = matrixIdentity;

// Open file dialog path
openPath = working_directory;

// Frees memory after loaded models and sprites
FreeModelAndSprites = function ()
{
	if (model != undefined)
	{
		vertex_delete_buffer(model);
		model = undefined;
	}

	if (spriteDiffuse != undefined)
	{
		sprite_delete(spriteDiffuse);
		spriteDiffuse = undefined;
	}

	if (spriteNormal != undefined)
	{
		sprite_delete(spriteNormal);
		spriteNormal = undefined;
	}

	if (spriteSpecular != undefined)
	{
		sprite_delete(spriteSpecular);
		spriteSpecular = undefined;
	}

	if (spriteGloss != undefined)
	{
		sprite_delete(spriteGloss);
		spriteGloss = undefined;
	}
};

// Frees current model and sprites from memory and loads a new model
LoadModel = function (_path)
{
	FreeModelAndSprites();
	var _buffer = buffer_load(_path);
	model = vertex_create_buffer_from_buffer(_buffer, vertex_format_pnuct);

	// Compute model's bounding box
	var _xmin = undefined;
	var _xmax = undefined;
	var _ymin = undefined;
	var _ymax = undefined;
	var _zmin = undefined;
	var _zmax = undefined;
	var _position = 0;
	var _stride = (3 * 4) + (3 * 4) + (2 * 4) + 4 + (4 * 4); // pnuct

	repeat (vertex_get_number(model))
	{
		var _x = buffer_peek(_buffer, _position + 0, buffer_f32);
		var _y = buffer_peek(_buffer, _position + 4, buffer_f32);
		var _z = buffer_peek(_buffer, _position + 8, buffer_f32);

		_xmin = min(_xmin ?? _x, _x);
		_xmax = max(_xmax ?? _x, _x);
		_ymin = min(_ymin ?? _y, _y);
		_ymax = max(_ymax ?? _y, _y);
		_zmin = min(_zmin ?? _z, _z);
		_zmax = max(_zmax ?? _z, _z);

		_position += _stride;
	}

	// Scale down the model and center it around 0, 0, 0
	var _scale = 1 / max(_xmax - _xmin, _ymax - _ymin, _zmax - _zmin);

	matrix = matrix_build(
		-((_xmin + _xmax) / 2) * _scale,
		-((_ymin + _ymax) / 2) * _scale,
		-((_zmin + _zmax) / 2) * _scale,
		0, 0, 0,
		_scale, _scale, _scale);

	vertex_freeze(model);
	buffer_delete(_buffer);
};

LoadDiffuse = function (_path)
{
	if (spriteDiffuse != undefined)
	{
		sprite_delete(spriteDiffuse)
	}
	spriteDiffuse = sprite_add(_path, 1, false, true, 0, 0);
};

LoadNormal = function (_path)
{
	if (spriteNormal != undefined)
	{
		sprite_delete(spriteNormal);
	}
	spriteNormal = sprite_add(_path, 1, false, true, 0, 0);
};

LoadSpecular = function (_path)
{
	if (spriteSpecular != undefined)
	{
		sprite_delete(spriteSpecular);
	}
	spriteSpecular = sprite_add(_path, 1, false, true, 0, 0);
};

LoadGloss = function (_path)
{
	if (spriteGloss != undefined)
	{
		sprite_delete(spriteGloss);
	}
	spriteGloss = sprite_add(_path, 1, false, true, 0, 0);
};

LoadModel("firetruck/firetruck.bin");
