/**
 * 可以包含在其他着色器中的常量，使用：
 *
 * #include "constants.glsl"
 */

const vec4 BACKGROUND_COLOR = vec4(0.5, 0.5, 0.5, 1.0);

// 头发渐变
const float MIN_COLOR = 0.5;
const float MIN_COLOR_END = 0.0;
const float MAX_COLOR_START = 0.3;

//         MIN_COLOR       mix(MIN_COLOR, 1.0, ...)           1.0
// root------------------|--------------------------|--------------------tip
//                 MIN_COLOR_END     MAX_COLOR_START

// 头发材质属性
const float HAIR_SHININESS = 50.0;

// mesh材质属性
const vec3 MESH_COLOR = vec3(221.0, 211.0, 238.0) / 255.0;
const float MESH_DIFFUSE_INTENSITY = 0.6;
const float MESH_AMBIENT_INTENSITY = 0.2;

// 填充(辅助 fill)灯光属性
const vec4 FILL_LIGHT_POS = vec4(-2.0, 1.0, 1.0, 1.0);
const float FILL_LIGHT_INTENSITY_HAIR = 0.6;
const float FILL_LIGHT_INTENSITY_MESH = 0.4;

// Opacity map constants不透明度映射常数
const float OPACITY_MAP_LAYER_SIZE = 0.0005;
