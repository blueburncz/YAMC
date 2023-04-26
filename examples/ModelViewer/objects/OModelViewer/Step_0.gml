var _windowWidth = window_get_width();
var _windowHeight = window_get_height();

// Resize the app. surface to match the window size (x2 for AA)
if (surface_get_width(application_surface) != _windowWidth * 2
	|| surface_get_height(application_surface) != _windowHeight * 2)
{
	surface_resize(application_surface, _windowWidth * 2, _windowHeight * 2);
}

// Control camera
var _mouseX = window_mouse_get_x();
var _mouseY = window_mouse_get_y();

if (mouse_check_button(mb_any))
{
	var _mouseSpeed = 0.5;
	direction += (mouseLastX - _mouseX) * _mouseSpeed;
	directionUp = clamp(directionUp - (mouseLastY - _mouseY) * _mouseSpeed, -89, 89);
}

mouseLastX = _mouseX;
mouseLastY = _mouseY;

zoom = max(zoom + (mouse_wheel_down() - mouse_wheel_up()) * 0.1, 1);

x = dcos(direction) * dcos(directionUp) * zoom;
y = -dsin(direction) * dcos(directionUp) * zoom;
z = dsin(directionUp) * zoom;

// Update camera's view and projection matrices
camera_set_view_mat(camera, matrix_build_lookat(
	x, y, z, 0, 0, 0, 0, 0, 1));

camera_set_proj_mat(camera, matrix_build_projection_perspective_fov(
	-70, -_windowWidth / _windowHeight, 0.1, 1000));

// Handle keyboard shortcuts
if (keyboard_check(vk_control))
{
	// Load model
	if (keyboard_check_pressed(ord("L")))
	{
		var _path = get_open_filename_ext("Vertex buffers|*.bin", "", openPath, "Select model");
		if (_path != "")
		{
			LoadModel(_path);
			openPath = filename_dir(_path);
		}
	}
	// Load diffuse texture
	else if (keyboard_check_pressed(ord("D")))
	{
		var _path = get_open_filename_ext("Sprite|*.png;*.gif;*.jpg;*.jpeg", "", openPath, "Select diffuse texture");
		if (_path != "")
		{
			LoadDiffuse(_path);
			openPath = filename_dir(_path);
		}
	}
	// Load normal texture
	else if (keyboard_check_pressed(ord("N")))
	{
		var _path = get_open_filename_ext("Sprite|*.png;*.gif;*.jpg;*.jpeg", "", openPath, "Select normal texture");
		if (_path != "")
		{
			LoadNormal(_path);
			openPath = filename_dir(_path);
		}
	}
	// Load specular color texture
	else if (keyboard_check_pressed(ord("S")))
	{
		var _path = get_open_filename_ext("Sprite|*.png;*.gif;*.jpg;*.jpeg", "", openPath, "Select specular color texture");
		if (_path != "")
		{
			LoadSpecular(_path);
			openPath = filename_dir(_path);
		}
	}
	// Load glossiness texture
	else if (keyboard_check_pressed(ord("G")))
	{
		var _path = get_open_filename_ext("Sprite|*.png;*.gif;*.jpg;*.jpeg", "", openPath, "Select glossiness texture");
		if (_path != "")
		{
			LoadGloss(_path);
			openPath = filename_dir(_path);
		}
	}
	// Toggle texture filtering
	else if (keyboard_check_pressed(ord("F")))
	{
		textureFilter = !textureFilter;
	}
}
