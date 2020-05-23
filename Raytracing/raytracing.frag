#version 430
#define BIG 10000.0
#define EPSILON 0.01

in vec3 interpolated_vertex;
out vec4 FragColor;

struct Camera
{
vec3 position;
vec3 view;
vec3 up;
vec3 side;
};

uniform Camera camera;
uniform vec2 scale;

struct Ray
{
vec3 origin;
vec3 direction;
};

struct Sphere
{
vec3 center;
float radius;
vec3 color;
int material_idx;
};

layout(std430, binding = 0) buffer SphereBuffer
{
 Sphere sphere_data[];
};


struct Material
{
 vec4 LightCoeffs;
};

Material material = {vec4(0.4, 0.9, 0.0, 512.0)};
vec3 light_pos = vec3(6, 0, -8);


struct Intersection
{
float time;
vec3 point;
vec3 normal;
vec3 color;
int material_idx;
vec4 LightCoeffs;
};







bool IntersectSphere(Sphere sphere,Ray ray, out float time)
{
ray.origin-=sphere.center;
float A = dot(ray.direction,ray.direction);
float B = dot ( ray.direction, ray.origin );
float C = dot ( ray.origin, ray.origin ) - sphere.radius * sphere.radius;
float D = B * B - A * C;
if ( D > 0.0 )
{
D = sqrt ( D );
float t1 = ( -B - D ) / A;
float t2 = ( -B + D ) / A;
if(t1 < 0 && t2 < 0) return false;

if(min(t1, t2) < 0)
{
time = max(t1,t2);
return true;
}
time = min(t1, t2);
return true;
}
return false;
}



bool Intersect(Ray ray, float start, float final, inout Intersection intersect)
{
bool result = false;
float time = start;
intersect.time = final;

for(int i = 0; i < sphere_data.length(); i++)
{
if (IntersectSphere(sphere_data[i], ray, time) && time < intersect.time)
{
intersect.time = time;
intersect.point = ray.origin + ray.direction * time;
intersect.normal = normalize(intersect.point - sphere_data[i].center);
intersect.color = sphere_data[i].color;
intersect.LightCoeffs = material.LightCoeffs;
intersect.material_idx= sphere_data[i].material_idx;
result = true;
}
}
return result;
}



float Shadow(vec3 pos_light, Intersection intersect)
{
float shadowing = 1.0;
vec3 direction = normalize(pos_light - intersect.point);
float distanceLight = distance(pos_light, intersect.point);
Ray shadowRay = Ray(intersect.point + direction * EPSILON, direction);
Intersection shadowIntersect;
shadowIntersect.time = BIG;
if(Intersect(shadowRay,0,distanceLight,shadowIntersect))
{
shadowing = 0.0;
}
return shadowing;
} 



vec3 Phong (Intersection intersect, vec3 pos_light, float shadow)
{
 vec3 light = normalize ( pos_light - intersect.point );
 float diffuse = max(dot(light, intersect.normal), 0.0);
 vec3 view = normalize(camera.position - intersect.point);
 vec3 reflected= reflect( -view, intersect.normal );
 float specular = pow(max(dot(reflected, light), 0.0), intersect.LightCoeffs.w);

 return intersect.LightCoeffs.x * intersect.color +intersect.LightCoeffs.y * diffuse * intersect.color*shadow + intersect.LightCoeffs.z * specular;

}



vec4 Raytrace(Ray primary_ray)
{
vec4 resultColor = vec4(0, 0, 0, 0);
Ray ray = primary_ray;

Intersection intersect;
intersect.time = BIG;
float start = 0;
float final = BIG;
if (Intersect(ray,start, final, intersect))
{
float shadowing = Shadow(light_pos, intersect);
resultColor += vec4(Phong(intersect, light_pos, shadowing), 0);
}
return resultColor;
}




Ray GenerateRay(Camera camera)
{
vec2 coords = interpolated_vertex.xy * normalize(scale);
vec3 direction = camera.view + camera.side * coords.x + camera.up * coords.y;
return Ray(camera.position, normalize(direction));
}



void main ( void )
{
/*FragColor = vec4 ( abs(interpolated_vertex.xy), 0, 1.0);
Ray ray=GenerateRay(camera);
FragColor=vec4 (abs(ray.direction.xy),0,1.0);
FragColor=vec4 (abs(camera.side),1.0);*/

Ray ray = GenerateRay(camera);
FragColor = Raytrace(ray);
}

